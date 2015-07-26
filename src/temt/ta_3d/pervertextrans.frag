#version 150 core

uniform vec4 lightPosition;
uniform vec3 lightIntensity;

// TODO: Replace with a struct
uniform float amb = .2f;      // Ambient scalar to derive reflectivity from diffuse
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor

in vec3 position;
in vec3 normal;
in vec4 color;

out vec4 fragColor;

vec3 adsModel( const vec3 pos, const vec3 n, const in vec3 col  )
{
    // Calculate the vector from the light to the fragment
    vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize( -pos );

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect( -s, n );

    // Calculate the diffuse component
    float diffuse = max( dot( s, n ), 0.0 );

    // Calculate the specular component
    float specular = 0.0;
    if ( dot( s, n ) > 0.0 )
        specular = pow( max( dot( r, v ), 0.0 ), shininess );

    vec3 ka = amb * col;    

    // Combine the ambient, diffuse and specular contributions
    return lightIntensity * ( ka + col * diffuse + ks * specular );
}

void main()
{
      fragColor = vec4( adsModel( position, normalize( normal ), color.rgb ), color.a );
}
