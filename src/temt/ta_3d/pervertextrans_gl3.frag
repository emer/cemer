#version 150 core

uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

uniform vec3 eyePosition;

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 color;

out vec4 fragColor;

#pragma include light.inc_gl3.frag

void main()
{
    vec3 diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, eyePosition, shininess, diffuseColor, specularColor);
    fragColor = vec4( color.rgb + color.rgb * diffuseColor + ks * specularColor, color.a );
}
