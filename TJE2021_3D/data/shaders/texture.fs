
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

void main()
{
	vec2 uv = v_uv;
	vec4 color = u_color * texture2D( u_texture, uv * u_texture_tiling );
	
	if (u_alpha == 1)
		color.a = 0.0f;

	gl_FragColor = color;
}
