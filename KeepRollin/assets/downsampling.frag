#version 330

uniform sampler2D uHighresTexture;

uniform vec4 uBias;
uniform vec4 uScale;

in vec2	vTextureCoordinates;

out vec4 oColor;

void main() {
	oColor = max(vec4(0.0), texture(uHighresTexture, vTextureCoordinates) + uBias) * uScale;
}
