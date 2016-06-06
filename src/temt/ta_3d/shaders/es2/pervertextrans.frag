#define FP highp

uniform FP vec3 ks;            // Specular reflectivity
uniform FP float shininess;    // Specular shininess factor

uniform FP vec3 eyePosition;

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec4 color;

#pragma include light.inc.frag

void main()
{
    FP vec3 diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, eyePosition, shininess, diffuseColor, specularColor);
    gl_FragColor = vec4( color.rgb + color.rgb * diffuseColor + ks * specularColor, color.a );
}
