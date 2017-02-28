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

uniform int levels;
uniform vec3 circleColor;
uniform int circleDepth;
uniform float circleSize;

in vec2 texCoords; // receive coord from vertex shader

// vertex position from vertex shader, in eye coordinates
in vec4 vertexPositionEC;

// normal vector from vertex shader, in eye coordinates
in vec3 normalDirEC;

// output: color
out vec4 outColor;

// calculate Phong-style local illumination
vec3 phongIllum(vec3 normalDir, vec3 viewDir, vec3 lightDir) {
    
    // ambient part
    vec3 ambient = k_ambient * ambientLightColor;
    
    // back face towards viewer?
    float ndotv = dot(normalDir,viewDir);
    if(ndotv<0.15)
        return vec3(0,0,0);
    
    // cos of angle between light and surface.
    float ndotl = dot(normalDir,-lightDir);
    if(ndotl<0.0)
        return vec3(0,0,0); // shadow / facing away from the light source

    // diffuse contribution
    
    vec3 diffuse = k_diffuse * lightColor * floor(ndotl * levels) * (1.0 / levels);
    
    vec2 parts = vec2(circleDepth); // how many parts per fragment
    vec2 nParts = vec2(1.0) / parts; // normalize parts
    vec2 mid = nParts / vec2(2); // get center of parts
    vec2 t = mod(texCoords.xy, nParts) - mid; // calculate center point of current coord
    float distance = dot(t, t) * 100; // dot product and scale by 100
    
    if (distance < circleSize) {
        diffuse = circleColor;
    }

    // reflected light direction = perfect reflection direction
    vec3 r = reflect(lightDir,normalDir);
    
    // angle between reflection dir and viewing dir
    float rdotv = max( dot(r,viewDir), 0.0);
    
    // specular contribution
    vec3 specular = k_specular * lightColor * pow(rdotv, shininess);
    
    // return sum of all contributions
    return ambient + diffuse + specular;

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
    
    // calculate color using phong illumination
    vec3 color = phongIllum(normalEC, viewdirEC, lightDirEC);
    
    // out to frame buffer
    outColor = vec4(color, 1);

}
