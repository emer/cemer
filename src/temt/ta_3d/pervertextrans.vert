#version 150 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexColor;

out vec3 position;
out vec3 normal;
out vec4 color;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    normal = normalize( modelViewNormal * vertexNormal );
    position = vec3( modelView * vec4( vertexPosition, 1.0 ) );
    color = vertexColor;

    gl_Position = mvp * vec4( vertexPosition, 1.0 );
}
