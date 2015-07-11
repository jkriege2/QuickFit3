//see http://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL#GLSLLinear
#version 110
uniform sampler2D texture0;
uniform int texture0_width;
uniform int texture0_height;

uniform sampler2D texture1;
uniform int texture1_width;
uniform int texture1_height;

float Triangular( float f )
{
    f = f / 2.0;
    if( f < 0.0 )
    {
        return ( f + 1.0 );
    }
    else
    {
        return ( 1.0 - f );
    }
    return 0.0;
}

// Function to get interpolated texel data from a texture with GL_NEAREST property.
// Bi-Linear interpolation is implemented in this function with the
// help of nearest four data.
vec4 tex2DBiLinear( sampler2D textureSampler_i, vec2 texCoord_i )
{
    float texelSizeX = 1.0 / float(texture0_width); //size of one texel
    float texelSizeY = 1.0 / float(texture0_height); //size of one texel
    vec4 p0q0 = texture2D(textureSampler_i, texCoord_i);
    vec4 p1q0 = texture2D(textureSampler_i, texCoord_i + vec2(texelSizeX, 0));

    vec4 p0q1 = texture2D(textureSampler_i, texCoord_i + vec2(0, texelSizeY));
    vec4 p1q1 = texture2D(textureSampler_i, texCoord_i + vec2(texelSizeX , texelSizeY));

    float a = fract( texCoord_i.x * float(texture0_width) ); // Get Interpolation factor for X direction.
                    // Fraction near to valid data.

    vec4 pInterp_q0 = mix( p0q0, p1q0, a ); // Interpolates top row in X direction.
    vec4 pInterp_q1 = mix( p0q1, p1q1, a ); // Interpolates bottom row in X direction.

    float b = fract( texCoord_i.y * float(texture0_height) );// Get Interpolation factor for Y direction.
    return mix( pInterp_q0, pInterp_q1, b ); // Interpolate in Y direction.
}


vec4 BiCubic( sampler2D textureSampler, vec2 TexCoord )
{
    float texelSizeX = 1.0 /  float(texture0_width); //size of one texel
    float texelSizeY = 1.0 / float(texture0_height); //size of one texel
    vec4 nSum = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 nDenom = vec4( 0.0, 0.0, 0.0, 0.0 );
    float a = fract( TexCoord.x *  float(texture0_width) ); // get the decimal part
    float b = fract( TexCoord.y * float(texture0_height) ); // get the decimal part
    for( int m = -1; m <=2; m++ )
    {
        for( int n =-1; n<= 2; n++)
        {
            vec4 vecData = texture2D(textureSampler,
                               TexCoord + vec2(texelSizeX * float( m ),
                    texelSizeY * float( n )));
            float f  = Triangular( float( m ) - a );
            vec4 vecCooef1 = vec4( f,f,f,f );
            float f1 = Triangular ( -( float( n ) - b ) );
            vec4 vecCoeef2 = vec4( f1, f1, f1, f1 );
            nSum = nSum + ( vecData * vecCoeef2 * vecCooef1  );
            nDenom = nDenom + (( vecCoeef2 * vecCooef1 ));
        }
    }
    return nSum / nDenom;
}

void main()
{
    vec2 pos=gl_TexCoord[0].st;
    vec4 color = tex2DBiLinear(texture0, pos);
    gl_FragColor = color;


}
