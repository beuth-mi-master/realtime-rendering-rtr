#version 330

uniform mat4 ciProjectionMatrix;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;

uniform sampler2DShadow uShadowTextureLeft;
uniform sampler2DShadow uShadowTextureFront;
uniform sampler2DShadow uShadowTextureRight;
uniform sampler2DShadow uShadowTextureTop;
uniform vec3 uShadowLightPosition;

uniform mat4 uShadowMatrixLeft;
uniform mat4 uShadowMatrixFront;
uniform mat4 uShadowMatrixRight;

uniform vec3 uLightColor;

uniform vec3 uWcLightPosition;
uniform vec3 uWcCameraPosition;

uniform vec3 uAmbient;

uniform vec3 uSpecularColor = vec3(1.0);
uniform int uShininess = 256;

uniform float uEmissiveThreshold = 0.8f;

uniform float uFogStart = 8.0f;
uniform float uFogEnd = 10.0f;

uniform float uAttentuationConstant = 0.8f;
uniform float uAttentuationLinear = 0.5f;
uniform float uAttentuationQuadratic = 0.4f;

const float near = 0.01f;
const float far  = 10.0f;

const vec3 luminanceFactor = vec3(0.2126, 0.7152, 0.0722);

in mat3 TBN;
in vec4 vVertexPosition;
in vec3 vNormalDirection;
in vec2 vTextureCoordinates;
in vec4 vViewPos;

in vec4 vShadowCoordLeft;
in vec4 vShadowCoordFront;
in vec4 vShadowCoordRight;

layout (location = 0) out vec4 oColor0;
layout (location = 1) out vec4 oColor1;

float getShadow(vec4 shadowCoord, sampler2DShadow uShadowTexture) {
    if (shadowCoord.z > -1 && shadowCoord.z < 1) {
        return textureProj(uShadowTexture, shadowCoord, -0.00005f);
    }
    return 1.0f;
}

float linearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0  * near * far) / (far + near - z * (far - near));
}

vec3 phong(vec3 n, vec3 v, vec3 l, float dist) {
    
    vec3 ambientColor = uAmbient;
    
    if (dot(n, v) < 0)
        return vec3(0);
    
    float ndotl = dot(n, -l);
    if (ndotl < 0)
        return vec3(0);
    
    vec3 diffuseColor = texture(uTexDiffuse, vTextureCoordinates).rgb * uLightColor * ndotl;
    
    vec3 r = reflect(l, n);
    float rdotv = max(dot(r, v), 0.0f);
    vec3 specularColor = uSpecularColor * uLightColor * pow(rdotv, uShininess);
    
    float attentuation = 2 / (uAttentuationConstant + uAttentuationLinear * dist + uAttentuationQuadratic * pow(dist, 2));
    
    vec4 shadowCoordLeft = vShadowCoordLeft / vShadowCoordLeft.w;
    vec4 shadowCoordFront = vShadowCoordFront / vShadowCoordFront.w;
    vec4 shadowCoordRight = vShadowCoordRight / vShadowCoordRight.w;

    float shadowLeft = getShadow(shadowCoordLeft, uShadowTextureLeft);
    float shadowFront = getShadow(shadowCoordFront, uShadowTextureFront);
    float shadowRight = getShadow(shadowCoordRight, uShadowTextureRight);
    
    float shadow = shadowLeft * shadowFront * shadowRight;
    
    shadow = shadow - linearizeDepth(gl_FragCoord.z) / 10.0f;
    
    return ambientColor + diffuseColor * shadow * attentuation + specularColor * attentuation * shadow;
}

vec3 decode(vec3 normal) {
    return normalize((normal * vec3(2, 2, 1) - vec3(1, 1, 0)) * vec3(-1, -1, 1));
}

void main(void) {

    vec3 wcLightDirection = uWcLightPosition - vVertexPosition.xyz;
    vec3 wcViewDirection = uWcCameraPosition - vVertexPosition.xyz;

    vec3 tsLightDirection = TBN * -wcLightDirection;
    vec3 tsViewDirection = TBN * wcViewDirection;
    
    float dist = length(wcLightDirection);

    vec3 n = normalize(decode(texture(uTexNormal, vTextureCoordinates).rgb));
    vec3 v = normalize(tsViewDirection);
    vec3 l = normalize(tsLightDirection);
    
    vec3 color = phong(n, v, l, dist);

    // fog
    float distToEnd = abs(vViewPos.z);
    float fogFactor = clamp((uFogEnd - distToEnd) / (uFogEnd - uFogStart), 0, 1);

    oColor0 = vec4(color, fogFactor);
    
    float luminance = dot(color, luminanceFactor);
    if (luminance >= uEmissiveThreshold) {
        oColor1 = vec4(color, fogFactor);
    } else {
        oColor1 = vec4(vec3(0), fogFactor);
    }

}
