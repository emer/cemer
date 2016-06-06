#define FP highp

uniform FB vec3 ka;            // Ambient reflectivity

void main()
{
    gl_FragColor = vec4( ka, 1.0 );
}
