
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform int u_alpha;
uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_texture_tiling;

uniform vec3 u_light_ambient;
uniform vec3 u_light_position;
uniform vec3 u_light_direction;
uniform vec3 u_light_color;
uniform int u_light_type;
uniform float u_light_maxdist;
uniform float u_light_cutoffCos;
uniform float u_light_exponent;
uniform float u_light_intensity;
uniform vec3 u_camera_position;
#define SAMPLES 64
uniform sampler2D u_texture_noise;
uniform vec3 u_rand;
vec3 L;
float NdotL;
float transparency=1.0; 
float air_density = 0.001;
vec3 N ;
float spotFactor = 0.0;
vec3 ray_dir; 

vec3 getPointLight(vec3 light) //POINT LIGHT
{
	vec3 N = normalize( v_normal);
	L = normalize( u_light_position - v_world_position);
	float light_distance = length(u_light_position - v_world_position );
	NdotL = clamp( dot(L,N), 0.0,1.0 );
	light += NdotL * u_light_color * u_light_intensity;
	return light; 
}
vec3 getDirectionalLight(vec3 light) //DIRECTIONAL LIGHT
{
	L = normalize(-u_light_direction);
	N = normalize( v_normal);
	NdotL = max( dot(N,L), 0.0 ); 
	light += NdotL * u_light_color * u_light_intensity;

	return light;
}
vec3 getSpotLight(vec3 light, float noise) //SPOT LIGHT
{
	for(int i = 0; i < SAMPLES; ++i)
	{
		vec3 current_pos = u_camera_position + ray_dir * noise;
		L = normalize( u_light_position - current_pos); 
		N = normalize( v_normal);
		NdotL = max( dot(N,L), 0.0); 
		float spotCosine = dot(-u_light_direction,L); 

		if (spotCosine >= u_light_cutoffCos){	
			spotFactor = pow(spotCosine, u_light_exponent);
		}
		else
		{
			current_pos += ray_dir;
			continue;
		}
		light += NdotL * u_light_color * u_light_intensity * spotFactor;	
		transparency -= air_density;
		if(transparency<0.01)
			break; 
	}
	return light;
}
void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color * texture2D( u_texture, uv * u_texture_tiling );

	//PHONG
	vec3 light = u_light_ambient;
	ray_dir = v_world_position - u_camera_position;
	ray_dir /= float( SAMPLES );

	if (u_light_type==0){ //POINT
		light+=getPointLight(light);
	}
	if (u_light_type==1){ //SPOT
		float noise = texture2D( u_texture_noise, uv + u_rand.xy ).x;
		light+=getSpotLight(light, noise);

	}
	if (u_light_type==2){ //DIRECTIONAL
		light+=getDirectionalLight(light);
	}
	color.xyz *= light;
	 
	color = vec4(color.xyz, transparency); 
	if (u_alpha == 1) //Si es 0 el objeto es de la realidad del espejo
		color.a = 0.0f;

	gl_FragColor = color;
}
