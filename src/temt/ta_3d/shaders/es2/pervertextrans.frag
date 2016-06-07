#define FP highp

varying FP vec3 worldPosition;
varying FP vec3 worldNormal;
varying FP vec4 color;

#pragma include light.inc.frag

void main()
{
    FP vec3 diffuseColor;
    adModel(worldPosition, worldNormal, diffuseColor);
    gl_FragColor = vec4( color.rgb + color.rgb * diffuseColor, color.a );
}
