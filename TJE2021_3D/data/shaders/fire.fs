//example of some shaders compiled
flat basic.vs flat.fs
texture basic.vs texture.fs
depth quad.vs depth.fs
multi basic.vs multi.fs
phong_singlepass basic.vs phong_singlepass.fs
phong_multipass basic.vs phong_multipass.fs
phong_shadows basic.vs phong_shadows.fs
phong_deferred quad.vs phong_deferred.fs
phong_deferred_geo basic.vs phong_deferred.fs
fxaa_filter quad.vs fxaa.fs
degamming quad.vs degamming.fs
ssao quad.vs ssao.fs
blur quad.vs blur.fs
IrrCoeffs basic.vs IrrCoeffs.fs
IrrScene quad.vs IrrScene.fs
cubemap basic.vs cubemap.fs
cubemap_texture basic.vs cubemap_texture.fs
deferred_reflections quad.vs reflections.fs
extra_buffer quad.vs extra_buffer.fs
volumetric_light quad.vs volumetric_light.fs
volumetric_light_geo basic.vs volumetric_light.fs
screen_ref quad.vs screen_ref.fs

//****************************************************************//
//************************MY FUNCTIONS****************************//
//****************************************************************//
\Irradiance
const float Pi = 3.141592654;
const float CosineA0 = Pi;
const float CosineA1 = (2.0 * Pi) / 3.0;
const float CosineA2 = Pi * 0.25;
struct SH9 { float c[9]; }; //to store weights
struct SH9Color { vec3 c[9]; }; //to store colors

layout(std430, binding = 3) buffer irradiance_data
{
	int probes_num;
    float probes[];
};

void SHCosineLobe(in vec3 dir, out SH9 sh) //SH9
{
	// Band 0
	sh.c[0] = 0.282095 * CosineA0;
	// Band 1
	sh.c[1] = 0.488603 * dir.y * CosineA1; 
	sh.c[2] = 0.488603 * dir.z * CosineA1;
	sh.c[3] = 0.488603 * dir.x * CosineA1;
	// Band 2
	sh.c[4] = 1.092548 * dir.x * dir.y * CosineA2;
	sh.c[5] = 1.092548 * dir.y * dir.z * CosineA2;
	sh.c[6] = 0.315392 * (3.0 * dir.z * dir.z - 1.0) * CosineA2;
	sh.c[7] = 1.092548 * dir.x * dir.z * CosineA2;
	sh.c[8] = 0.546274 * (dir.x * dir.x - dir.y * dir.y) * CosineA2;
}

vec3 ComputeSHIrradiance(in vec3 normal, in SH9Color radiance)
{
	// Compute the cosine lobe in SH, oriented about the normal direction
	SH9 shCosine;
	SHCosineLobe(normal, shCosine);
	// Compute the SH dot product to get irradiance
	vec3 irradiance = vec3(0.0);
	for(int i = 0; i < 9; ++i)
		irradiance += radiance.c[i] * shCosine.c[i];

	return irradiance;
}

SH9Color getProbe(int index)
{
	SH9Color coeff;
	int j = 0;

	for(int i = 0; i < 9; ++i)
	{
		j = i * 3 + index;

		float x = probes[j];
		float y = probes[j+1];
		float z = probes[j+2];

		coeff.c[i] = vec3(x,y,z);
		//j = j + 3;
	}

	return coeff;
};

\Attenuation
float computeAttenuation(int light_type, vec3 light_pos, float max_dist, vec3 worldpos){
	
	//Directional has no attenuation
	if(light_type == DIRECTIONAL)
		return 1.0;
	
	//distance
	float light_distance =  length(light_pos - worldpos);
	float att_factor = (max_dist - light_distance) / max_dist;
	att_factor = max( att_factor, 0.0 );
	
	//quadratic
	return (att_factor * att_factor);	
	
}

\PBRFunctions
#define PI 3.14159265358979323846

// Normal Distribution Function using GGX Distribution
float D_GGX ( const in float NoH, 
			  const in float linearRoughness )
{
	float a2 = linearRoughness * linearRoughness;
	float f = (NoH * NoH) * (a2 - 1.0) + 1.0;
	return a2 / (PI * f * f);
}

// Fresnel term with scalar optimization(f90=1)
vec3 F_Schlick ( const in float VoH, 
				 const in vec3 f0 )
{
	float f = pow(1.0 - VoH, 5.0);
	return f0 + (vec3(1.0) - f0) * f;
}

// Geometry Term: Geometry masking/shadowing due to microfacets
float GGX ( float NdotV, float k )
{
	return NdotV / (NdotV * (1.0 - k) + k);
}
	
float G_Smith ( float NdotV, float NdotL, 
				float roughness )
{
	float k = pow(roughness + 1.0, 2.0) / 8.0;
	return GGX(NdotL, k) * GGX(NdotV, k);
}


//this is the cook torrance specular reflection model
vec3 specularBRDF( float roughness, vec3 f0, 
				   float NoH, float NoV, float NoL, float LoH )
{
	float a = roughness * roughness;

	// Normal Distribution Function
	float D = D_GGX( NoH, a );

	// Fresnel Function
	vec3 F = F_Schlick( LoH, f0 );

	// Visibility Function (shadowing/masking)
	float G = G_Smith( NoV, NoL, roughness );
		
	// Norm factor
	vec3 spec = D * G * F;
	spec /= (4.0 * NoL * NoV + 1e-6);

	return spec;
}

vec3 compute_light_PBR(bool pbr, vec3 eye, vec3 worldpos, vec3 N, vec3 L, vec3 color, vec3 lightParams, float metalness, float roughness)
{

	float NoL =  max(dot(N, L), 0);

	if(pbr)
	{
		vec3 V = normalize( eye - worldpos );
		vec3 H = normalize( L + V );

		float NoV = max(dot(N, V), 0);
		float NoH = max(dot(N, H), 0);
		float LoH = max(dot(L, H), 0);

		//we compute the reflection in base to the color and the metalness
		vec3 f0 = color.xyz * metalness + (vec3(0.5) * (1.0-metalness));

		//metallic materials do not have diffuse
		vec3 diffuseColor = (1.0 - metalness) * color.xyz;

		//compute the specular
		vec3 Fr_d = specularBRDF( roughness, f0, NoH, NoV, NoL, LoH );

		vec3 Fd_d = diffuseColor * NoL; 

		//add diffuse and specular reflection
		vec3 direct = Fr_d + Fd_d;

		return ( direct * lightParams );
	} 
	else
	{
		return ( NoL * lightParams );
	}

}


\ShadowsFunctions
vec2 vec3ToCubemap2D( vec3 v )
{
	vec3 abs_ = abs(v);
	
	//Get the largest component
	float max_ = max(max(abs_.x, abs_.y), abs_.z);
	
	//1.0 for the largest component, 0.0 for the others
	vec3 weights = step(max_, abs_);
	
	//0 or 1
	float sign_ = dot(weights, sign(v)) * 0.5 + 0.5; 
	float sc = dot(weights, mix(vec3(v.z,v.x,-v.x), vec3(-v.z, v.x, v.x), sign_));
	float tc = dot(weights, mix(vec3(-v.y, -v.z, -v.y), vec3(-v.y, v.z, -v.y), sign_));
	vec2 uv = (vec2(sc, tc) / max_) * 0.5 + 0.5;
	
	// Offset into the right region of the texture
	float offsetY = dot(weights, vec3(1.0, 3.0, 5.0)) - sign_;
	uv.y = (uv.y + offsetY) / 6.0;
	
	uv.x = 1 - uv.x;
	uv.y = 1 - uv.y;
	
	return uv;
}

vec3 computeShadows(int light_type, vec3 worldpos, vec3 light_position, 
					vec2 camera_nearfar, mat4 shadow_viewproj[4], sampler2D shadow_map)
{
	vec3 color =  vec3(1.0);

	if(light_type == POINT)
	{
		vec3 dir = worldpos - light_position;
		vec2 shadow_uv = vec3ToCubemap2D(dir);
		float z = length(dir);

		if( shadow_uv.x < 0.0 || shadow_uv.x > 1.0 ||
			shadow_uv.y < 0.0 || shadow_uv.y > 1.0 ) {
			return vec3(0.0);
		} else if (z < 0.0 || z > camera_nearfar.y) {
			return vec3(0.0);
		} else {
			float depth = texture(shadow_map, shadow_uv).x;

			float n = camera_nearfar.x;
			float f = camera_nearfar.y;
			float lz  = n * (depth + 1.0) / (f + n - depth * (f - n));					
			depth = (lz * (f - n)) + n;
			
			if(z > depth) 
				return vec3(0.0);
		}
	}
	else if(light_type == SPOT)
	{
		//project our 3D position to the shadowmap
		vec4 proj_pos = shadow_viewproj[0] * vec4(worldpos, 1.0);

		//from homogeneus space to clip space
		vec2 shadow_uv = proj_pos.xy / proj_pos.w;

		//from clip space to uv space
		shadow_uv = shadow_uv * 0.5 + vec2(0.5);
		
		if(shadow_uv.x < 0.0 || shadow_uv.x > 1.0 || shadow_uv.y < 0.0 || shadow_uv.y > 1.0)
			return 1.0;
		
		//get point depth (from -1 to 1)
		float z = (proj_pos.z - u_bias) / proj_pos.w;

		//normalize from [-1..+1] to [0..+1]
		z = z * 0.5 + 0.5;
		
		float depth = texture(shadow_map, shadow_uv).r;
		
		if(z > depth)
			return vec3(0.0);
	}
	else if(light_type == DIRECTIONAL)
	{
		int actual_cascade = 0;
		for(int j = 0; j < 4; ++j){
			vec4 proj_pos = shadow_viewproj[j] * vec4(worldpos, 1.0);

			vec2 shadow_uv = proj_pos.xy / proj_pos.w;
			shadow_uv = shadow_uv * 0.5 + 0.5;

			float real_depth = (proj_pos.z - u_bias) / proj_pos.w;
			real_depth = real_depth * 0.5 + 0.5;

			if( shadow_uv.x >= 0.0 && shadow_uv.x <= 1.0 &&
				shadow_uv.y >= 0.0 && shadow_uv.y <= 1.0 && 
				real_depth  >= 0.0 && real_depth  <= 1.0 ) {

				if(u_show_cascades){
					if(j == 0)
						color *= vec4(1.0, 0.0, 0.0, 1.0);
					else if(j == 1) 
						color *= vec4(0.0, 1.0, 0.0, 1.0);
					else if(j == 2) 
						color *= vec4(0.0,0.0,1.0,1.0);
					else 
						color *= vec4(1.0, 0.0, 1.0, 1.0);
				}

				actual_cascade = j;
				shadow_uv.y = mix((4-(j+1))/4.0, (4-j)/4.0, shadow_uv.y);

				float depth = texture(shadow_map, shadow_uv).r;
				
				if(real_depth > depth)
					return vec3(0.0);

				break;
			}
        }
	}

	return 1.0 * color;
}

vec3 computeSmoothShadows(int light_type, vec3 worldpos, vec3 light_position, vec2 iRes, 
						  vec2 camera_nearfar, mat4 shadow_viewproj[4], 
						  sampler2D shadow_map, sampler2DShadow shadow_smooth_map){
	vec2 shadow_uv;
	vec3 color = vec3(1.0);
	float shadow_factor = 0.0;
	float z;

	if(light_type == POINT)
	{
		vec3 dir = worldpos - light_position;
		vec2 shadow_uv = vec3ToCubemap2D(dir);
		z = length(dir);
		float depth = camera_nearfar.y;

		for (int y = -1; y <= 1; y++){
			for (int x = -1; x <= 1; x++){
				vec2 uv = vec2(shadow_uv.x + x*iRes.x, shadow_uv.y 
								+ y*iRes.y);

				if( uv.x < 0.0 || uv.x > 1.0 ||
				uv.y < 0.0 || uv.y > 1.0 ) {
					continue;
				} else if (z < 0.0 || z > depth) {
					continue;
				} else {
					float shadow_depth = texture(shadow_map, uv).x;

					float n = camera_nearfar.x;
					float f = camera_nearfar.y;
					float lz  = n * (shadow_depth + 1.0) / (f + n - shadow_depth * (f - n));					
					shadow_depth = (lz * (f - n)) + n;
			
					if(shadow_depth >= z) 
						shadow_factor++;
				}
			}
		}

		return  vec3(shadow_factor/9.0);
	}
	else if(light_type == SPOT)
	{
		//project our 3D position to the shadowmap
		vec4 proj_pos = shadow_viewproj[0] * vec4(worldpos, 1.0);

		//from homogeneus space to clip space
		shadow_uv = proj_pos.xy / proj_pos.w;

		//from clip space to uv space
		shadow_uv = shadow_uv * 0.5 + vec2(0.5);
	
		//get point depth (from -1 to 1)
		z = (proj_pos.z - u_bias) / proj_pos.w;

		//normalize from [-1..+1] to [0..+1]
		z = z * 0.5 + 0.5;
	}
	else if(light_type == DIRECTIONAL)
	{
		int actual_cascade = -1;

		for(int j = 0; j < 4; ++j){
			vec4 proj_pos = shadow_viewproj[j] * vec4(worldpos, 1.0);

			shadow_uv = proj_pos.xy / proj_pos.w;
			shadow_uv = shadow_uv * 0.5 + 0.5;

			z = (proj_pos.z - u_bias) / proj_pos.w;
			z = z * 0.5 + 0.5;

			if( shadow_uv.x >= 0.0 && shadow_uv.x <= 1.0 &&
				shadow_uv.y >= 0.0 && shadow_uv.y <= 1.0
				&& z >= 0.0 && z <= 1.0 ) {

				if(u_show_cascades){
					if(j == 0)
						color *= vec4(1.0, 0.0, 0.0, 1.0);
					else if(j == 1) 
						color *= vec4(0.0, 1.0, 0.0, 1.0);
					else if(j == 2) 
						color *= vec4(0.0,0.0,1.0,1.0);
					else 
						color *= vec4(1.0, 0.0, 1.0, 1.0);
				}

				actual_cascade = j;
				shadow_uv.y = mix((4-(j+1))/4.0, (4-j)/4.0, shadow_uv.y);

				break;
			}
        }

		if(actual_cascade == -1)
			return vec3(1.0);
	}

	for (int y = -1; y <= 1; y++){
		for (int x = -1; x <= 1; x++){
			vec2 offsets = vec2(x*u_iRes.x, y*u_iRes.y);
			vec2 uv = shadow_uv + offsets;

			if( uv.x < 0.0 || uv.x > 1.0 ||
				uv.y < 0.0 || uv.y > 1.0 ) {
				if(u_light_type == DIRECTIONAL)
					shadow_factor += 1.0;	
			} else if (z < 0.0 || z > 1.0) {
				if(u_light_type == DIRECTIONAL)
					shadow_factor += 1.0;
			} else {
				vec3 uvc = vec3(uv, z);
				shadow_factor += texture(shadow_smooth_map, uvc);
			}
		}
	}

	return color * ( (shadow_factor/9.0));
}

\NormalFunctions
//from this github repo
mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
	// get edge vectors of the pixel triangle
	vec3 dp1 = dFdx( p );
	vec3 dp2 = dFdy( p );
	vec2 duv1 = dFdx( uv );
	vec2 duv2 = dFdy( uv );
	
	// solve the linear system
	vec3 dp2perp = cross( dp2, N );
	vec3 dp1perp = cross( N, dp1 );
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
	// construct a scale-invariant frame 
	float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
	return mat3( T * invmax, B * invmax, N );
}

// assume N, the interpolated vertex normal and 
// WP the world position
//vec3 normal_pixel = texture2D(normalmap, uv ).xyz; 
vec3 perturbNormal(vec3 N, vec3 WP, vec2 uv, vec3 normal_pixel)
{
	normal_pixel = normal_pixel * 255./127. - 128./127.;
	mat3 TBN = cotangent_frame(N, WP, uv);
	return normalize(TBN * normal_pixel);
}

\RGBEFunctions
//takes float color and encodes it as RGBE [0..1]
vec4 EncodeRGBE8( in vec3 rgb )
{
  vec4 vEncoded;
  float maxComponent = max(max(rgb.r,rgb.g),rgb.b);
  float fExp = ceil( log2(maxComponent) );
  vEncoded.rgb = rgb / exp2(fExp);
  vEncoded.a = (fExp + 128.0) / 255.0;
  return vEncoded;
}

//decode from vec4 [0..1] to vec3 with better range
vec3 DecodeRGBE8( in vec4 rgbe )
{
  vec3 vDecoded;
  float fExp = rgbe.a * 255.0 - 128.0;
  vDecoded = rgbe.rgb * exp2(fExp);
  return vDecoded;
}

//****************************************************************//
//****************************************************************//
//****************************************************************//
\basic.vs

#version 330 core

precision highp float;

in vec3 a_vertex;
in vec3 a_normal;
in vec2 a_uv;
in vec4 a_color;

uniform vec3 u_camera_pos;

uniform mat4 u_model;
uniform mat4 u_viewprojection;

//this will store the color for the pixel shader
out vec3 v_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;
out vec4 v_color;

void main()
{	
	//calcule the normal in camera space (the NormalMatrix is like ViewMatrix but without traslation)
	v_normal = (u_model * vec4( a_normal, 0.0) ).xyz;
	
	//calcule the vertex in object space
	v_position = a_vertex;
	v_world_position = (u_model * vec4( v_position, 1.0) ).xyz;
	
	//store the color in the varying var to use it from the pixel shader
	v_color = a_color;

	//store the texture coordinates
	v_uv = a_uv;

	//calcule the position of the vertex using the matrices
	gl_Position = u_viewprojection * vec4( v_world_position, 1.0 );
}

\quad.vs

#version 330 core

in vec3 a_vertex;
in vec2 a_uv;
out vec2 v_uv;

void main()
{	
	v_uv = a_uv;
	gl_Position = vec4( a_vertex, 1.0 );
}


\flat.fs

#version 330 core

uniform vec4 u_color;

out vec4 FragColor;

void main()
{
	FragColor = u_color;
}


\texture.fs

#version 330 core

in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_alpha_cutoff;

out vec4 FragColor;

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color;

	color *= texture( u_texture, uv );

	if(color.a < u_alpha_cutoff)
		discard;

	color.xyz = pow( color.xyz, vec3(2.2) );

	FragColor = color;
}


\multi.fs

#version 330 core

precision highp float;

in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

uniform sampler2D u_texture;
uniform sampler2D u_texture_rough;
uniform sampler2D u_texture_emi;
uniform sampler2D u_texture_normal;

uniform vec4 u_color;
uniform float u_alpha_cutoff;
uniform vec3 u_emissive_factor;
uniform float u_metall_factor;
uniform float u_rough_factor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 NormalColor;
layout(location = 2) out vec4 EmiColor;

//********************//
//*****INCLUDES******//

#include "NormalFunctions"

//*******************//

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color;
	color *= texture( u_texture, uv );

	if(color.a < u_alpha_cutoff)
		discard;

	//Normals from 0 to 1
	vec4 n_color;
	vec3 N = normalize(v_normal);

	n_color = texture( u_texture_normal, uv );
	
	if(n_color == vec4(vec3(0.0), 1))
	{
		n_color = vec4(N * 0.5 + 0.5, 1.0);
	}
	else
	{
		vec3 local_space_normal = perturbNormal(N, v_world_position, uv, n_color.xyz);
		n_color.xyz = vec3(local_space_normal * 0.5 + 0.5);
	}

	//Extra colors
	vec4 extra_color = texture( u_texture_rough, uv );

	//Emissive is saved into alpha of every buffer
	vec4 emi = texture( u_texture_emi, uv );
	emi.xyz = emi.xyz * u_emissive_factor;

	FragColor = vec4( color.xyz + vec3(0.01), extra_color.x );
	NormalColor = vec4( n_color.xyz, extra_color.y * u_rough_factor);
	EmiColor = vec4( emi.xyz, extra_color.z * u_metall_factor);
}


\depth.fs

#version 330 core

precision highp float;

uniform vec2 u_camera_nearfar;
uniform sampler2D u_texture; //depth map
in vec2 v_uv;
out vec4 FragColor;

void main()
{
	float n = u_camera_nearfar.x;
	float f = u_camera_nearfar.y;
	float z = texture2D(u_texture,v_uv).x;
	float color = n * (z + 1.0) / (f + n - z * (f - n));
	FragColor = vec4(color);
}


\instanced.vs

#version 330 core

in vec3 a_vertex;
in vec3 a_normal;
in vec2 a_uv;

in mat4 u_model;

uniform vec3 u_camera_pos;

uniform mat4 u_viewprojection;

//this will store the color for the pixel shader
out vec3 v_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;

void main()
{	
	//calcule the normal in camera space (the NormalMatrix is like ViewMatrix but without traslation)
	v_normal = (u_model * vec4( a_normal, 0.0) ).xyz;
	
	//calcule the vertex in object space
	v_position = a_vertex;
	v_world_position = (u_model * vec4( a_vertex, 1.0) ).xyz;
	
	//store the texture coordinates
	v_uv = a_uv;

	//calcule the position of the vertex using the matrices
	gl_Position = u_viewprojection * vec4( v_world_position, 1.0 );
}

//***************************************************************//
//**************************MY SHADERS***************************//
//***************************************************************//
\screen_ref.fs
#version 330 core

in vec2 v_uv;
uniform sampler2D u_texture;

out vec4 FragColor;

void main() {
	vec4 t = texture( u_texture, v_uv );
	gl_FragColor = t;
};


\volumetric_light.fs
#version 330 core

precision highp float;

#define SAMPLES 64
#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2

uniform vec3 u_camera_pos;
uniform sampler2D u_depth_texture;
uniform mat4 u_inverse_viewprojection;
uniform vec2 u_iRes; //For the ShadowMap
uniform vec2 u_iRes_volFBO;
uniform sampler2D u_texture_noise;
uniform vec3 u_rand;

//Light uniforms
uniform int u_light_type;
uniform vec3 u_light_position;
uniform vec2 u_camera_nearfar;
uniform mat4 u_shadow_viewproj[4];
uniform sampler2D u_shadow_map;
uniform vec3 u_light_color;
uniform float u_bias;
uniform bool u_show_cascades;
uniform vec3 u_light_direction;
uniform float u_light_cutoffCos;
uniform float u_intensity;
uniform float u_light_maxdist;
uniform int u_light_exponent;

#include "Attenuation"
#include "ShadowsFunctions"

in vec2 v_uv;
out vec4 FragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy * u_iRes_volFBO;
	float depth  = texture(u_depth_texture, uv).x;

	//create screenpos with the right depth and reproject
	vec4 screen_position = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	vec4 proj_worldpos = u_inverse_viewprojection * screen_position;
	vec3 worldpos = proj_worldpos.xyz / proj_worldpos.w;

	vec3 ray_dir = worldpos - u_camera_pos;
	ray_dir /= float( SAMPLES );

	float noise = texture( u_texture_noise, uv + u_rand.xy ).x;
	vec3 current_pos = u_camera_pos + ray_dir * noise;

	float density = 0.0;
	vec3 color = vec3( 0.0 );
	vec3 l_color = pow(u_light_color, vec3(2.2)) * u_intensity;

	float density_att = 0.01;
	float spotFactor = 1.0;

	for(int i = 0; i < SAMPLES; ++i)
	{
		if(u_light_type == SPOT)
		{
			density_att = 0.05;
			vec3 L = normalize(u_light_position - current_pos);

			//test if point is inside spot angle
			float spotCos = dot(-u_light_direction, L);
		
			//inside spot cone
			if(spotCos < u_light_cutoffCos)
			{
				current_pos += ray_dir;
				continue;
			}
			else
			{
				spotFactor = pow(spotCos, u_light_exponent);
			}
		}

		float att = computeAttenuation(u_light_type, u_light_position, u_light_maxdist, current_pos);
		
		//evaluate contribution (Test if pixel is inside shadows)
		vec3 shadow_factor = computeShadows(u_light_type, current_pos, u_light_position, 
					u_camera_nearfar, u_shadow_viewproj, u_shadow_map);
		
		color += vec3( shadow_factor.x ) * att * spotFactor;
		density += shadow_factor.x * density_att * att * spotFactor;

		if( density >= 1.0 )
			break;

		//advance to next position
		current_pos += ray_dir;
	}

	color /= float( SAMPLES );
	color *= l_color;

	FragColor = vec4( color, density );
}

\extra_buffer.fs
#version 330 core

precision highp float;

uniform sampler2D u_texture; //depth map
in vec2 v_uv;
out vec4 FragColor;

void main()
{
	float z = texture2D(u_texture,v_uv).w;
	FragColor = vec4(z);
}

\reflections.fs
#version 330 core

precision highp float;

in vec2 v_uv;

uniform sampler2D u_normal_texture;
uniform sampler2D u_color_texture;
uniform sampler2D u_emissive_texture;
uniform sampler2D u_depth_texture;

uniform mat4 u_inverse_viewprojection;
uniform vec3 u_camera_position;
uniform vec2 u_iRes;
uniform int u_mipmap_level_decrease;

uniform bool u_is_probe;
uniform bool u_use_parallax;
uniform samplerCube u_probe_1;
uniform vec3 u_box_max;
uniform vec3 u_box_min;
uniform vec3 u_probe_pos;

out vec4 FragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy * u_iRes;

	vec3 basecolor = texture(u_color_texture, uv).xyz;
	vec4 normal = texture(u_normal_texture, uv);
	float depth  = texture(u_depth_texture,  uv).x;

	if(depth == 1.0)
		discard;

	float roughness = normal.w;
	float metalness = texture(u_emissive_texture, uv).w;

	//create screenpos with the right depth
	vec4 screen_position = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);

	//reproject
	vec4 proj_worldpos = u_inverse_viewprojection * screen_position;
	vec3 worldpos = proj_worldpos.xyz / proj_worldpos.w;

	vec3 N = normalize( normal.xyz * vec3(2.0) - vec3(1.0) );
	vec3 R;
	vec3 V;

	//Caclulation of the parallax
	if(u_is_probe)
	{
		V = normalize(worldpos - u_camera_position);
		R = reflect( V, N );
		
		if(u_use_parallax)
		{
			vec3 firstPlaneIntersect  = (u_box_max - worldpos) / R;
			vec3 secondPlaneIntersect = (u_box_min - worldpos) / R;

			vec3 furthestPlane = max( firstPlaneIntersect, secondPlaneIntersect );
		
			float distance = min(min(furthestPlane.x, furthestPlane.y), furthestPlane.z);

			vec3 intersectPos = worldpos + R * distance;

			R = intersectPos - u_probe_pos;
		}
	} 
	else 
	{
		V = normalize( u_camera_position - worldpos );
		R = reflect( V, N );
	}

	vec3 color = textureLod( u_probe_1, R, u_mipmap_level_decrease * roughness ).xyz;
	FragColor  = vec4(color, metalness);
}

\cubemap_texture.fs
#version 330 core

in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

uniform samplerCube u_texture;
uniform vec3 u_camera_position;

out vec4 FragColor;

void main()
{
	vec3 N = normalize( v_normal );
	vec3 V = normalize( u_camera_position - v_world_position );
	vec3 R = reflect(-V, N);

	vec4 color = textureLod( u_texture, R, 0.0 );
	color.xyz = pow( color.xyz, vec3(1.0/2.2) );

	FragColor = color;
}

\cubemap.fs

#version 430 core

in vec3 v_world_position;

uniform samplerCube u_texture;
uniform vec3 u_camera_position;

out vec4 FragColor;

void main()
{
	vec3 V = normalize( u_camera_position - v_world_position );
	FragColor = texture( u_texture, V );
}

\IrrScene.fs
#version 430 core

precision highp float;

uniform vec3 u_irr_start;
uniform vec3 u_irr_end;
uniform float u_irr_normal_distance;
uniform vec3 u_irr_delta;
uniform vec3 u_irr_dim;
uniform mat4 u_inverse_viewprojection;
uniform sampler2D u_depth_texture;
uniform sampler2D u_normal_texture;

uniform bool u_interpolate;

in vec2 v_uv;
out vec4 FragColor;

//********************//
//*****INCLUDES******//

#include "Irradiance"

//*******************//

int getIndex(vec3 local_indices){
	float indexf = local_indices.z + local_indices.y * u_irr_dim.z 
				   + local_indices.x * u_irr_dim.z * u_irr_dim.y;

	//We have to multiply for the size of every probe and substract one
	//probe in order to avoid an array bound exception
	float a_size =  probes_num * 27 - 27;
	int index = int(min(indexf * 27, a_size));

	return index;
}

void main()
{
	vec2 uv = v_uv;

	vec4 normal =  texture(u_normal_texture, uv);
	vec3 N = normal.xyz * 2.0 - 1.0;
	N = normalize(N);

	//read depth from depth buffer
	float depth = texture( u_depth_texture, uv ).x;

	//ignore pixels in the background
	if(depth >= 1)
	{
		FragColor = vec4(1.0);
		return;
	}

	//create screenpos with the right depth
	vec4 screen_position = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0,
								1.0);
	//reproject
	vec4 proj_worldpos = u_inverse_viewprojection * screen_position;
	vec3 worldpos = proj_worldpos.xyz / proj_worldpos.w;

	vec3 irr_range = u_irr_end - u_irr_start;
	vec3 irr_local_pos = clamp(worldpos - u_irr_start 
			+ N * u_irr_normal_distance, vec3(0.0), irr_range);

	vec3 irr_norm_pos = irr_local_pos / u_irr_delta;

	//Interpolation
	vec3 dim = u_irr_dim - vec3(1.0);
	vec3 local_indices = floor( irr_norm_pos );
	vec3 factors = irr_norm_pos - local_indices;
	
	vec3 P0 = local_indices;

	P0.x = min(P0.x, dim.x);
	P0.y = min(P0.y, dim.y);
	P0.z = min(P0.z, dim.z);

	int index  = getIndex( P0 ); 
	vec3 irradiance  = ComputeSHIrradiance( N, getProbe(index ) );
	vec3 c = irradiance;

	if(u_interpolate)
	{
		/*******************************/
		//  Get the neighbors probes   //
		/*******************************/
		vec3 P1 = vec3(min(P0.x + 1, dim.x), P0.y, P0.z);									// Next in X
		vec3 P2 = vec3(P0.x, min(P0.y + 1, dim.y), P0.z);									// Next in Y
		vec3 P3 = vec3(P0.x, P0.y,  min(P0.z + 1, dim.z));									// Next in Z
		vec3 P4 = vec3(min(P0.x + 1, dim.x), min(P0.y + 1, dim.y), P0.z);					// Next in XY
		vec3 P5 = vec3(min(P0.x + 1, dim.x), P0.y, min(P0.z + 1, dim.z));					// Next in XZ
		vec3 P6 = vec3(P0.x, min(P0.y + 1, dim.y), min(P0.z + 1, dim.z));					// Next in ZY
		vec3 P7 = vec3(min(P0.x + 1, dim.x), min(P0.y + 1, dim.y), min(P0.z + 1, dim.z));	// Next in XYZ
	
		int index1 = getIndex( P1 );
		int index2 = getIndex( P2 );
		int index3 = getIndex( P3 );
		int index4 = getIndex( P4 );
		int index5 = getIndex( P5 );
		int index6 = getIndex( P6 );
		int index7 = getIndex( P7 );
	
		vec3 irradiance1 = ComputeSHIrradiance( N, getProbe(index1) );
		vec3 irradiance2 = ComputeSHIrradiance( N, getProbe(index2) );
		vec3 irradiance3 = ComputeSHIrradiance( N, getProbe(index3) );
		vec3 irradiance4 = ComputeSHIrradiance( N, getProbe(index4) );
		vec3 irradiance5 = ComputeSHIrradiance( N, getProbe(index5) );
		vec3 irradiance6 = ComputeSHIrradiance( N, getProbe(index6) );
		vec3 irradiance7 = ComputeSHIrradiance( N, getProbe(index7) );

		vec3 c00 = mix(irradiance,  irradiance1, factors.x);
		vec3 c01 = mix(irradiance3, irradiance5, factors.x);
		vec3 c10 = mix(irradiance6, irradiance7, factors.x);
		vec3 c11 = mix(irradiance2, irradiance4, factors.x);

		vec3 c0 = mix(c00, c11, factors.y);
		vec3 c1 = mix(c01, c10, factors.y);

		c = mix(c0, c1, factors.z);
	}

	FragColor = vec4(c, 1.0);
}

\IrrCoeffs.fs

#version 430 core

precision highp float;

in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

uniform vec3 u_coeffs[9];
uniform int u_index;

out vec4 FragColor;

//********************//
//*****INCLUDES******//

#include "Irradiance"

//*******************//

void main()
{
	SH9Color radiance;
	//Old Coeffs
	//radiance.c = u_coeffs;
	radiance = getProbe(u_index);
	vec3 normal = normalize(v_normal);
	vec3 color = ComputeSHIrradiance(normal, radiance);
	//color = pow( color, vec3(1.0/2.2) );
	FragColor = vec4(color, 1.0);
}

\blur.fs

#version 330 core

precision highp float;

uniform vec2 u_offset;
uniform sampler2D u_texture;
in vec2 v_uv;
out vec4 FragColor;

void main()
{
	vec4 color = vec4(1.0);
	vec2 uv = v_uv;

	for (int x = -2; x <= 2; ++x)
		for (int y = -2; y <= 2; ++y)
			color += texture(u_texture, uv + u_offset * vec2(x, y) 
					+ vec2(0.5) * u_offset);
	
	color /= 25.0;
	FragColor = color;
}

\ssao.fs

#version 330 core

precision highp float;

uniform mat4 u_inverse_viewprojection;
uniform mat4 u_viewprojection;

uniform vec2 u_iRes;
uniform sampler2D u_depth_texture;
uniform sampler2D u_normal_texture;

#define MAX_POINTS 64
uniform vec3 u_points[MAX_POINTS];
uniform float u_radius;
uniform float u_bias;

in vec2 v_uv;
out vec4 FragColor;

//********************//
//*****INCLUDES******//

#include "NormalFunctions"

//*******************//

void main()
{
	//we want to center the sample in the center of the pixel
	vec2 uv = v_uv + (u_iRes * 0.5);

	vec4 normal = texture( u_normal_texture, uv );
	vec3 N = normal.xyz * 2.0 - 1.0;
	N = normalize(N);

	//read depth from depth buffer
	float depth = texture( u_depth_texture, uv ).x;

	//ignore pixels in the background
	if(depth >= 1)
	{
		FragColor = vec4(1.0);
		return;
	}

	//create screenpos with the right depth
	vec4 screen_position = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0,
								1.0);
	
	//reproject
	vec4 proj_worldpos = u_inverse_viewprojection * screen_position;
	vec3 worldpos = proj_worldpos.xyz / proj_worldpos.w;

	const int samples = MAX_POINTS;
	float num = samples;
	float radius = u_radius;

	//to create the matrix33 to convert from tangent to world
	mat3 rotmat = cotangent_frame( N, worldpos, uv );

	for( int i = 0; i < samples; ++i)
	{
		vec3 point = rotmat * u_points[i];
		point = point * radius + worldpos;
		
		vec4 proj = u_viewprojection * vec4(point, 1.0);
		proj.z = (proj.z - u_bias); //apply a tiny bias to its z before converting to clip-space 
		proj.xyz /= proj.w; //convert to clipspace from homogeneous
		proj.xyz = proj.xyz * 0.5 + vec3(0.5); //to [0..1]

		//read p true depth
		vec4 pdepth = texture( u_depth_texture, proj.xy );
		screen_position = vec4(proj.xy * 2.0 - 1.0, pdepth.z * 2.0 - 1.0, 1.0);
		proj_worldpos = u_inverse_viewprojection * screen_position;
		vec3 worldpos_point = proj_worldpos.xyz / proj_worldpos.w;

		//If the points are too far it does not oclude
		float dist = length(worldpos_point - point);
		float range_check = smoothstep(0.0, 1.0, radius / float(dist));

		//compare true depth with its depth
		if( pdepth.z < proj.z ) //if true depth smaller, is inside
			num -= range_check; //remove this point from the list of visible
	}
	
	float ao = num / float(samples);

	FragColor = vec4(ao);
}


\degamming.fs

#version 330 core

uniform vec4 u_color;
uniform sampler2D u_texture;

in vec2 v_uv;

out vec4 FragColor;

void main()
{

	vec4 color = texture( u_texture, v_uv );
	color.xyz = pow( color.xyz, vec3(1.0/2.2) );

	FragColor = color;
}


\fxaa.fs
#version 330 core

precision highp float;

in vec2 v_uv;

uniform sampler2D u_texture;
uniform vec2 u_iTexture_size;

out vec4 FragColor;

void main()
{

	float R_fxaaSpanMax = 8.0;
	float R_fxaaReduceMul = 1 / float(8.0);
	float R_fxaaReduceMin = 1/ float(128.0);

	vec2 texCoordOffset = u_iTexture_size;
	vec2 uv = v_uv;

	vec3 luma = vec3(0.299, 0.587, 0.114);	
	float lumaTL = dot(luma, texture2D(u_texture, uv + (vec2(-1.0, -1.0) * texCoordOffset)).xyz);
	float lumaTR = dot(luma, texture(u_texture, uv + (vec2(1.0, -1.0) * texCoordOffset)).xyz);
	float lumaBL = dot(luma, texture(u_texture, uv + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);
	float lumaBR = dot(luma, texture(u_texture, uv + (vec2(1.0, 1.0) * texCoordOffset)).xyz);
	float lumaM  = dot(luma, texture(u_texture, uv).xyz);

	vec2 dir;
	dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
	dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
	
	float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (R_fxaaReduceMul * 0.25), R_fxaaReduceMin);
	float inverseDirAdjustment = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(R_fxaaSpanMax, R_fxaaSpanMax), 
		max(vec2(-R_fxaaSpanMax, -R_fxaaSpanMax), dir * inverseDirAdjustment)) * texCoordOffset;

	vec3 result1 = (1.0/2.0) * (
		texture(u_texture, uv + (dir * vec2(1.0/3.0 - 0.5))).xyz +
		texture(u_texture, uv + (dir * vec2(2.0/3.0 - 0.5))).xyz);

	vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
		texture(u_texture, uv + (dir * vec2(0.0/3.0 - 0.5))).xyz +
		texture(u_texture, uv + (dir * vec2(3.0/3.0 - 0.5))).xyz);

	float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
	float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
	float lumaResult2 = dot(luma, result2);
	
	if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
		FragColor = vec4( result1, 1.0);
	else
		FragColor = vec4( result2, 1.0);
}

\phong_singlepass.fs

#version 330 core

precision highp float;

in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_alpha_cutoff;
uniform sampler2D u_emissive;
uniform vec3 u_emissive_factor;

//light uniforms
const int MAX_LIGHTS = 5;
uniform vec3 u_light_position[MAX_LIGHTS];
uniform vec3 u_light_color[MAX_LIGHTS];
uniform int u_light_types[MAX_LIGHTS];
uniform float u_light_maxdist[MAX_LIGHTS];
uniform float u_light_cutoffCos[MAX_LIGHTS];
uniform float u_light_expos[MAX_LIGHTS];
uniform vec3 u_light_direction[MAX_LIGHTS];
uniform int u_num_lights;

uniform vec3 u_ambient_light;

//GAMMA
uniform float u_correction;

out vec4 FragColor;

#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2

//********************//
//*****INCLUDES******//

#include "Attenuation"

//*******************//

void main()
{
	//**************************************//
	//ALL LIGHTS COLORS ALREADY DEGAMMED!!!!
	//**************************************//
	
	vec2 uv = v_uv;
	vec4 color = u_color;

	color *= texture( u_texture, uv );

	if(color.a < u_alpha_cutoff)
		discard;
		
	//ambient
	vec3 ambient = u_ambient_light;	

	//emissive
	vec3 emissive = texture2D( u_emissive, uv ).xyz;
	emissive *= u_emissive_factor;	

	//Aplying degamma
	//**************************************//
	//ALL LIGHTS COLORS ALREADY DEGAMMED!!!!
	//**************************************//
	color.xyz = pow( color.xyz, vec3(2.2) );
	emissive.xyz = pow( emissive.xyz, vec3(2.2) );
	ambient = pow( u_ambient_light, vec3(2.2) );

	//sum of all diffuses
	vec3 total_diffuse = vec3(0.0);
	
	//iterate through all lights
	for( int i = 0; i < MAX_LIGHTS; ++i )
	{
		if(i < u_num_lights)
		{
			vec3 L;
			vec3 N = normalize( v_normal );
		
			//POINT or SPOT
			if(u_light_types[i] == POINT || u_light_types[i] == SPOT){
				L = u_light_position[i] - v_world_position;
				L = normalize(L);
			}

			//DIRECTIONAL (THE SUN)
			else if(u_light_types[i] == DIRECTIONAL){
				L = u_light_direction[i] * (-1);
			}
			
			//SPOT FACTOR
			float spotFactor = 1.0;
			if(u_light_types[i] == SPOT){
			
				//test if point is inside spot angle
				float spotCos = dot( u_light_direction[i] * (-1), L);
				
				//inside spot cone
				if(spotCos >= u_light_cutoffCos[i])
					spotFactor = pow(spotCos, u_light_expos[i]);
				
				//outside spot cone
				else	
					spotFactor = 0.0;
			}
			
			//compute how much is aligned (cannot be negative)
			float NdotL = max( dot(L,N), 0.0 );
			
			//attenuation
			float att_factor = computeAttenuation(u_light_types[i], u_light_position[i], u_light_maxdist[i], v_world_position);
			
			//add the amount of diffuse light
			//**************************************//
			//ALL LIGHTS COLORS ALREADY DEGAMMED!!!!
			//**************************************//
			total_diffuse += NdotL * u_light_color[i] * att_factor * spotFactor;
		}
	}
	
	color.xyz *= (ambient + emissive + total_diffuse) * u_correction;

	//Aplying Gamma
	//color.xyz = pow( color.xyz, vec3(1.0/2.2) );

	FragColor = color;
}

\phong_multipass.fs

#version 330 core

precision highp float;

in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_alpha_cutoff;
uniform sampler2D u_emissive;
uniform vec3 u_emissive_factor;

//light uniforms
uniform vec3 u_light_position;
uniform vec3 u_light_direction;
uniform vec3 u_light_color;
uniform int u_light_type;
uniform float u_light_maxdist;
uniform float u_light_cutoffCos;
uniform int u_light_exponent;

uniform vec3 u_ambient_light;

//GAMMA
uniform float u_correction;

//PBR
uniform sampler2D u_rough_metall;
uniform float u_metall_factor;
uniform float u_rough_factor;
uniform bool u_pbr;
uniform vec3 u_eye;

out vec4 FragColor;

#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2

//********************//
//*****INCLUDES******//

#include "Attenuation"
#include "PBRFunctions"

//*******************//

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color;

	color *= texture( u_texture, uv );

	if(color.a < u_alpha_cutoff)
		discard;
		
	//ambient
	vec3 ambient = u_ambient_light;	

	//emissive
	vec3 emissive = texture( u_emissive, uv ).xyz;
	emissive *= u_emissive_factor;	

	//diffuse
	vec3 diffuse = vec3(0.0);
	
	//light color
	vec3 light_color = u_light_color;

	//Aplying degamma
	color.xyz = pow( color.xyz, vec3(2.2) );
	emissive.xyz = pow( emissive.xyz, vec3(2.2) );
	light_color = pow( light_color, vec3(2.2) );
	ambient = pow( u_ambient_light, vec3(2.2) );

	vec3 L;
	vec3 N = normalize( v_normal );

	//POINT or SPOT
	if(u_light_type == POINT || u_light_type == SPOT){
		L = u_light_position - v_world_position;
		L = normalize(L);
	}

	//DIRECTIONAL (THE SUN)
	else if(u_light_type == DIRECTIONAL){
		L = u_light_direction * (-1);
	}
	
	//SPOT FACTOR
	float spotFactor = 1.0;
	if(u_light_type == SPOT){
	
		//test if point is inside spot angle
		float spotCos = dot( u_light_direction * (-1), L);
		
		//inside spot cone
		if(spotCos >= u_light_cutoffCos)
			spotFactor = pow(spotCos, u_light_exponent);
		
		//outside spot cone
		else	
			spotFactor = 0.0;
	}
	
	//compute how much is aligned (cannot be negative)
	float NdotL = max( dot(L,N), 0.0 );
	
	//attenuation
	float att_factor = computeAttenuation(u_light_type, u_light_position, u_light_maxdist, v_world_position);
	
	vec3 lightParams = light_color * att_factor  * spotFactor;

	//PBR
	vec3 rougMetall = texture( u_rough_metall, uv).xyz;
	float roughness = rougMetall.y * u_rough_factor;
	float metalness = rougMetall.z * u_metall_factor;

	vec3 light = compute_light_PBR(u_pbr, u_eye, v_world_position, N, L, color.xyz, lightParams, metalness, roughness);
	
	color.xyz *= (ambient + emissive + light) * u_correction;

	FragColor = color;
}

\phong_deferred.fs

#version 430 core

precision highp float;

uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_emissive_texture;
uniform mat4 u_inverse_viewprojection;
uniform vec3 u_eye;

uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform vec3 u_light_direction;
uniform int u_light_type;

//We don't pass intensity because we multiply it by the light color
uniform float u_light_cutoffCos;
uniform int u_light_exponent;
uniform float u_light_maxdist;
uniform int u_use_emissive;

uniform vec3 u_ambient;
uniform vec2 u_iRes;
uniform vec2 u_iRes_Screen;

//shadows
uniform sampler2D u_shadow_map;
uniform sampler2DShadow u_shadow_smooth_map;
uniform float u_bias;
uniform mat4 u_shadow_viewproj[4];
uniform bool u_smooth;
uniform vec2 u_camera_nearfar;
uniform bool u_show_cascades;
uniform bool u_generate_shadows;

//GAMMA
uniform float u_correction;

//Oclusions
uniform sampler2D u_AO_map;
uniform float u_AO_intensity;

//PBR
uniform bool u_pbr;

//IRRADIANCE
uniform sampler2D u_irr_texture;
uniform float u_Irr_intensity;

////////////////////////
//    CONSTANTS       //
////////////////////////
#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2

out vec4 FragColor;

//********************//
//*****INCLUDES******//

#include "Attenuation"
#include "PBRFunctions"
#include "ShadowsFunctions"

//*******************//

void main()
{
	vec2 uv = gl_FragCoord.xy * u_iRes_Screen.xy;

	vec4 color = texture( u_color_texture, uv );
	vec4 normal = texture( u_normal_texture, uv);
	vec4 emission = texture( u_emissive_texture, uv );
	vec3 light_color = u_light_color;

	float oclusions = color.a;
	float roughness = normal.a;
	float metalness = emission.a;

	vec3 N = normal.xyz * 2.0 - 1.0;
	N = normalize(N);
	
	//Get position
	float depth = texture( u_depth_texture, uv ).x;

	if(depth == 1)
		discard;

	vec4 screen_pos = vec4(uv.x * 2.0 - 1.0, uv.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
	vec4 proj_worldpos = u_inverse_viewprojection * screen_pos;
	vec3 worldpos = proj_worldpos.xyz / proj_worldpos.w;

	//Phong Ilumination
	vec3 L;

	//Aplying ambient oclusion
	float oclussion = texture( u_AO_map, uv ).x;
	oclussion = pow(oclussion, u_AO_intensity);

	vec3 irradiance = texture( u_irr_texture, uv ).xyz;
	irradiance = irradiance * u_Irr_intensity;

	vec3 ambient = u_ambient * irradiance * oclussion;

	//Aplying degamma
	color.xyz = pow( color.xyz, vec3(2.2) );
	emission.xyz = pow( emission.xyz, vec3(2.2) );
	light_color = pow( light_color, vec3(2.2) );
	ambient = pow(ambient, vec3(2.2));
	
	//diffuse
	vec3 light = vec3(0.0);

	//shadows
	vec3 shadow_factor = vec3(1.0);

	//emissive
	vec3 emissive = emission.xyz * u_use_emissive;

	if(u_generate_shadows)
		if(u_smooth)
			shadow_factor = computeSmoothShadows(u_light_type, worldpos, u_light_position, u_iRes, 
						  u_camera_nearfar, u_shadow_viewproj, u_shadow_map, u_shadow_smooth_map);
		else
			shadow_factor = computeShadows(u_light_type, worldpos, u_light_position, 
						  u_camera_nearfar, u_shadow_viewproj, u_shadow_map);

	if(shadow_factor.x > 0.001 || shadow_factor.y > 0.001 || shadow_factor.z > 0.001 )
	{
		//POINT or SPOT
		if(u_light_type == POINT || u_light_type == SPOT){
			L = normalize(u_light_position - worldpos);
		}
		//DIRECTIONAL (THE SUN)
		else if(u_light_type == DIRECTIONAL){
			L = u_light_direction * (-1);
		}
	
		//SPOT FACTOR
		float spotFactor = 1.0;
		if(u_light_type == SPOT){
	
			//test if point is inside spot angle
			float spotCos = dot( u_light_direction * (-1), L);
		
			//inside spot cone
			if(spotCos >= u_light_cutoffCos)
				spotFactor = pow(spotCos, u_light_exponent);
		
			//outside spot cone
			else	
				spotFactor = 0.0;
		}

		//attenuation
		float att_factor = computeAttenuation(u_light_type, u_light_position, u_light_maxdist, worldpos);

		//PBR
		vec3 lightParams = light_color * att_factor  * spotFactor * shadow_factor;
		light = compute_light_PBR(u_pbr, u_eye, worldpos, N, L, color.xyz, lightParams, metalness, roughness);

	}

	color.xyz *= (light + emissive + ambient) * u_correction;

	FragColor = vec4(color.xyz, 1.0);
}



/*************************************************************/
/*************************************************************/
/*************************************************************/
/************************PHONG SHADOWS************************/
/*************************************************************/
/*************************************************************/
/*************************************************************/


\phong_shadows.fs

#version 330 core

precision highp float;

in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_alpha_cutoff;
uniform sampler2D u_emissive;
uniform vec3 u_emissive_factor;

//light uniforms
uniform vec3 u_light_position;
uniform vec3 u_light_direction;
uniform vec3 u_light_color;
uniform int u_light_type;
uniform float u_light_maxdist;
uniform float u_light_cutoffCos;
uniform int u_light_exponent;

//shadows
uniform sampler2D u_shadow_map;
uniform sampler2DShadow u_shadow_smooth_map;
uniform mat4 u_shadow_viewproj[4];
uniform bool u_show_cascades;
uniform bool u_smooth;
uniform bool u_generate_shadows;
uniform float u_bias;
uniform vec2 u_iRes;
uniform vec2 u_camera_nearfar;

uniform vec3 u_ambient_light;

//gamma 
uniform float u_correction;

out vec4 FragColor;

#define POINT 0
#define SPOT 1
#define DIRECTIONAL 2

//********************//
//*****INCLUDES******//

#include "Attenuation"
#include "ShadowsFunctions"

//*******************//

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color;
	
	color *= texture( u_texture, uv );

	if(color.a < u_alpha_cutoff)
		discard;
		
	//ambient
	vec3 ambient = u_ambient_light;

	//emissive
	vec3 emissive = texture2D( u_emissive, uv ).xyz * u_emissive_factor;
	
	//light color
	vec3 light_color = u_light_color;

	color.xyz = pow( color.xyz, vec3(2.2) );
	emissive = pow( emissive, vec3(2.2) );
	ambient = pow( ambient, vec3(2.2) );
	light_color = pow( u_light_color, vec3(2.2) );

	//test shadows
	vec3 shadow_factor = vec3(1.0);

	if(u_generate_shadows)
		if(u_smooth)
			shadow_factor = computeSmoothShadows(u_light_type, v_world_position, u_light_position, u_iRes, 
						  u_camera_nearfar, u_shadow_viewproj, u_shadow_map, u_shadow_smooth_map);
		else
			shadow_factor = computeShadows(u_light_type, v_world_position, u_light_position, 
						  u_camera_nearfar, u_shadow_viewproj, u_shadow_map);

	//diffuse
	vec3 diffuse = vec3(0.0);
	
	if(shadow_factor.x > 0.001 || shadow_factor.y > 0.001 || shadow_factor.z > 0.001 )
	{
		vec3 L;
		vec3 N = normalize( v_normal );

		//POINT or SPOT
		if(u_light_type == POINT || u_light_type == SPOT){
			L = u_light_position - v_world_position;
			L = normalize(L);
		}

		//DIRECTIONAL (THE SUN)
		else if(u_light_type == DIRECTIONAL){
			L = u_light_direction * (-1);
		}
		
		//SPOT FACTOR
		float spotFactor = 1.0;
		if(u_light_type == SPOT){
		
			//test if point is inside spot angle
			float spotCos = dot( u_light_direction * (-1), L);
			
			//inside spot cone
			if(spotCos >= u_light_cutoffCos)
				spotFactor = pow(spotCos, u_light_exponent);
			
			//outside spot cone
			else	
				spotFactor = 0.0;
		}
		
		//compute how much is aligned (cannot be negative)
		float NdotL = max( dot(L,N), 0.0 );
		
		//attenuation
		float att_factor = computeAttenuation(u_light_type, u_light_position, u_light_maxdist, v_world_position);
		

		//add the amount of diffuse light
		diffuse = NdotL * light_color *
				  att_factor * spotFactor * shadow_factor;
	}
	
	color.xyz *= (ambient + emissive + diffuse) * u_correction;

	//color.xyz = pow( color.xyz, vec3(1.0/2.2) );

	FragColor = color;
}