#version 150 core

uniform vec3 ka;            // Ambient reflectivity

out vec4 fragColor;

void main()
{
    fragColor = vec4( ka, 1.0 );
}
