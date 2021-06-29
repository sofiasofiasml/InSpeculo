float SCALE = 1.;

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform sampler2D u_texture2;
uniform float u_time;

uniform float u_texture_tiling;
mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p ); 
    return f;
}
// --- End of: Created by inigo quilez --------------------
float mynoise ( vec3 p)
{
     return noise(p);
     //return .5+.5*sin(50.*noise(p));
}
float myfbm( vec3 p )
{
    float f;
    f  = 0.5000*mynoise( p ); p = m*p*2.02;
    f += 0.2500*mynoise( p ); p = m*p*2.03;
    f += 0.1250*mynoise( p ); p = m*p*2.01;
    f += 0.0625*mynoise( p ); p = m*p*2.05;
    f += 0.0625/2.*mynoise( p ); p = m*p*2.02;
    f += 0.0625/4.*mynoise( p );
    return f;
}
float myfbm2( vec3 p )
{
    float f;
    f  = 1. - 0.5000*mynoise( p ); p = m*p*2.02;
    f *= 1. - 0.2500*mynoise( p ); p = m*p*2.03;
    f *= 1. - 0.1250*mynoise( p ); p = m*p*2.01;
    f *= 1. - 0.0625*mynoise( p ); p = m*p*2.05;
    f *= 1. - 0.0625/2.*mynoise( p ); p = m*p*2.02;
    f *= 1. - 0.0625/4.*mynoise( p );
    return f;
}
void main()
{
	vec2 uv = v_uv;
	  vec3 v;
    vec3 p = 4.*vec3(uv,0.)+u_time*0.2*(.1,.7,1.2);
    float x = myfbm(p);
    //v = vec3(x);
    v = (.5+.5*sin(x*vec3(30.,20.,10.)*SCALE))/SCALE;
    float g = 1.;
    g = pow(length(v),1.);
    g =  .5*noise(8.*m*m*m*p)+.5; g = 2.*pow(g,3.);
    v *= g;
    vec4 Ti = texture2D(u_texture2,.02*v.xy+uv * u_texture_tiling)*1.-.2;
    vec4 Tf = texture2D(u_texture,.02*v.xy+uv * u_texture_tiling); 
    vec3 T=Ti.rgb;
    //T = Ti+(1.-Ti)*(Tf*0.5); 
    vec3 T1,T2;
    T1 = vec3(0.,0.,1.); T1 *= .5*(T+1.);
    T2 = vec3(1.,1.,1.); //T2 = 1.2*Ti*vec3(0.,.0,.0)-.2;
    v = mix(mix(T1+(T2*0.2),1.*Tf.rgb,.5),T2,T*0.9);   
	vec4 color = vec4(v,Ti.a);

    if(color.a < 0.1)
		discard;
    
    gl_FragColor = color;
	//gl_FragColor = u_color * texture2D( u_texture, uv * u_texture_tiling );
}
