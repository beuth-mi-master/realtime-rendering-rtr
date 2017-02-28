#version 330

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

uniform sampler2D uTexDiffuse;
uniform float uEmissiveThreshold = 0.8f;

in vec2 vTextureCoordinates;

layout (location = 0) out vec4 oColor0;
layout (location = 1) out vec4 oColor1;

void main(void) {

    vec3 color = texture(uTexDiffuse, vTextureCoordinates).rgb;

    oColor0 = vec4(color, 1.0);
    
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (luminance >= uEmissiveThreshold) {
        oColor1 = vec4(color, 1.0);
    } else {
        oColor1 = vec4(vec3(0), 1);
    }
}
