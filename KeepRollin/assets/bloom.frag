#version 330

uniform sampler2D uTex0;
uniform sampler2D uTex1;

in vec2 vTextureCoordinates;

out vec4 oColor;

void main() {
    oColor = vec4(texture(uTex0, vTextureCoordinates).rgb + texture(uTex1, vTextureCoordinates).rgb, 1.0);
}
