/*
 * Simple Phong Shader
 * (C)opyright Hartmut Schirmacher, http://schirmacher.beuth-hochschule.de
 *
 * This fragment shader calculates some direction vectors in eye space
 * and then uses a Phong illum model to calculate output color.
 *
 */

#version 150

// Phong coefficients and exponent
uniform vec3  k_ambient;
uniform vec3  k_diffuse;
uniform vec3  k_specular;
uniform float shininess;

// ambient light and point light
uniform vec3 ambientLightColor;
uniform vec3 lightColor;
uniform vec4 lightPositionEC; // in eye coordinates!

// Cinder-provided unoformas
uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

uniform int depth;

uniform float time;

in vec2 texCoords; // receive coord from vertex shader

// vertex position from vertex shader, in eye coordinates
in vec4 vertexPositionEC;

// normal vector from vertex shader, in eye coordinates
in vec3 normalDirEC;

// output: color
out vec4 outColor;

// filters for chessboard, check only fractional part of x and y
bool checkPositionOnBoard(vec2 pos) {
    return (fract(pos.x) < 0.5 && fract(pos.y) < 0.5) || (fract(pos.x) > 0.5 && fract(pos.y) > 0.5);
}

void main(void) {
    // normalize normal after projection
    vec3 normalEC = normalize(normalDirEC);
    
    // calculate light direction (for point light)
    vec3 lightDirEC = normalize(vertexPositionEC - lightPositionEC).xyz;
    
    // do we use a perspective or an orthogonal projection matrix?
    bool usePerspective = ciProjectionMatrix[2][3] != 0.0;
    
    // for perspective mode, the viewing direction (in eye coords) points
    // from the vertex to the origin (0,0,0) --> use -ecPosition as direction.
    // for orthogonal mode, the viewing direction is simply (0,0,1)
    vec3 viewdirEC = usePerspective? normalize(-vertexPositionEC.xyz) : vec3(0,0,1);
    
    // calculate parts
    float parts = (1.0/(depth/2.0));
    vec2 pos = texCoords.xy / parts;
    
    vec3 color = (checkPositionOnBoard(pos)) ? vec3(0.8,0.8,0.8) : vec3(0.2,0.2,0.2);
    
    color *= vec3(time);
        
    // out to frame buffer
    outColor = vec4(fract(color.xyz), 1);

}


