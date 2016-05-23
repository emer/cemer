#version 150 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 color;

out vec4 fragColor;

#pragma include light.inc.frag

void main()
{
    vec3 diffuseColor;
    adsModel(worldPosition, worldNormal, diffuseColor);
    fragColor = vec4( color.rgb + color.rgb * diffuseColor, color.a );
}
