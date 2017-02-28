// textures
uniform sampler2D displacementTexture;
uniform sampler2D normalTexture;
uniform sampler2D grassTexture;
uniform sampler2D stoneTexture;
uniform sampler2D snowTexture;


// light position
uniform vec4 wcLightPosition;

// heights of textures
// x => min position (from)
// y => max position (to)
// z => how long should mix be
uniform vec3 grassHeight;
uniform vec3 stoneHeight;
uniform vec3 snowHeight;

// slope variables
uniform float slopeToleranceFrom = 0.9f;
uniform float slopeToleranceTo = 0.94f;

// fog variables
uniform float fogStart = 0.5f;
uniform float fogEnd = 0.8f;
uniform vec3 fogColor = vec3(0.4, 0.4, 0.4);

in vec4 viewPos;
in vec3 tsLightDir;
in vec3 tsViewDir;
in vec2 texCoords;

out vec4 outColor;

// interpolates between given values
float interpolate(float from, float to, float v) {
    float v1 = to - from;
    float v2 = v - from;
    return clamp(v2 / v1, 0.0, 1.0);
}

// change contrast range
vec3 decode(vec3 normal) {
    return normalize(normal * vec3(2, 2, 2) - vec3(1, 1, 1)) * vec3(1, -1, 1);
}

// mix Textures where height is not normalized
// sizes.x => min position (from)
// sizes.y => max position (to)
// sizes.z => how long should mix be
// height => current position
vec3 mixTextures(vec3 tex1, vec3 tex2, vec3 sizes, float height) {
    float diffLength = sizes.z;
    float maxLength = (sizes.x + diffLength);
    float currentPosition = (maxLength - height) / diffLength;
    return (height <= maxLength) ? tex2 * (1 - currentPosition) + tex1 * currentPosition : tex2;
}

vec3 terrainCalculation(vec3 normalDir, vec3 viewDir, vec3 lightDir) {
    
    // cos of angle between light and surface.
    float ndotl = dot(normalDir, -lightDir);
    
    // shadow / facing away from the light source
    if(ndotl < 0.0) {
        return vec3(0,0,0);
    }
    
    // loadTexture colors
    vec3 grassT = texture(grassTexture, texCoords * 64.0).rgb;
    vec3 stoneT = texture(stoneTexture, texCoords * 16.0).rgb;
    vec3 snowT = texture(snowTexture, texCoords * 16.0).rgb;
    
    // current height of displaced map
    float height = texture(displacementTexture, texCoords).x;
    
    // slope
    float slope = normalDir.z;
    
    vec3 textureColor;
    
    if (height >= grassHeight.x && height < grassHeight.y) {
        textureColor = ndotl * grassT;
    }
    if (height >= stoneHeight.x && height < stoneHeight.y) {
        textureColor = ndotl * mixTextures(grassT, stoneT, stoneHeight, height);
    }
    if (height >= snowHeight.x && height <= snowHeight.y) {
        // calculate slope additionally
        float relevantValue = interpolate(slopeToleranceFrom, slopeToleranceTo, slope);
        vec3 slopeColor = mix(stoneT, snowT, relevantValue);
        textureColor = ndotl * mixTextures(stoneT, slopeColor, snowHeight, height);
    }
        
    return textureColor;

}

void main(void) {
    
    // normal map calculation
    vec3 N = normalize(decode(texture(normalTexture, texCoords).xyz));
    vec3 V = normalize(tsViewDir);
    vec3 L = normalize(tsLightDir);
    
    // calculate color using phong illumination
    vec3 color = terrainCalculation(N, V, L);
    
    /*
     * FOG http://in2gpu.com/2014/07/22/create-fog-shader/
     */
    // get absolut distance value from viewPosition (z axis)
    float dist = abs(viewPos.z);
    
    // calculate 'to be fogged' factor
    float fogFactor = (fogEnd - dist) / (fogEnd - fogStart);
    
    // clamp values lower than 0.0 to 0.0 and higher than 1.0 to 1.0 to avoid wrong values
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);

    // mix actual color with fog
    vec3 finalColor = mix(fogColor, color, fogFactor);
    
    // out to frame buffer
    outColor = vec4(finalColor, 1.0f);

}
