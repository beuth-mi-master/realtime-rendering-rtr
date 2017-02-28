#version 330

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

uniform vec4 uColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
uniform float uEmissiveThreshold = 0.8f;

layout (location = 0) out vec4 oColor0;
layout (location = 1) out vec4 oColor1;

void main(void) {
    oColor0 = uColor;
    
    float luminance = dot(uColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (luminance >= uEmissiveThreshold) {
        oColor1 = uColor;
    } else {
        oColor1 = vec4(0.0f);
    }
}
