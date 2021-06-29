
varying vec2 v_uv;

uniform sampler2D u_texture;
uniform sampler2D u_texture_1;
uniform int u_alpha;
void main(){
    vec2 uv = v_uv;
  	//vec2 uv = fragCoord.xy / iResolution.xy;
       
	vec3 obump =  texture2D(u_texture,uv).rgb;
	float displace = dot(obump, vec3(0.3, 0.6, 0.1));
	displace = (displace - 0.5)*0.3;
	vec3 color = texture2D(u_texture_1,uv + (displace)).rgb;
	vec4 alpha = texture2D(u_texture_1,uv);
	if(alpha.a != 1.0)
		discard;

	vec4 final_Color = vec4(color,0.4);
	
	if (u_alpha == 1)
		final_Color = vec4(0.0f,0.0f,0.0f,0.0f);

		//final_Color.a = 0.0f;

    
    gl_FragColor = final_Color;
}