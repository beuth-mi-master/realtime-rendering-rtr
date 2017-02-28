#version 330

uniform sampler2D uBlurTexture;
uniform bool uHorizontal;

const int kernelSize = 11;
uniform float weight[kernelSize] = float[] (
	0.18,
	0.12396,
	0.11443,
	0.09001,
	0.06034,
	0.03446,
	0.01678,
	0.00696,
	0.00246,
	0.00074,
	0.00019
);

in vec2	vTextureCoordinates;

out vec4 oColor;

void main() {

	vec2 tex_offset = 1.0f / textureSize(uBlurTexture, 0);

    vec2 offset;
    if (uHorizontal) {
        offset = vec2(tex_offset.x, 0.0f);
    } else {
        offset = vec2(0.0f, tex_offset.y);
    }
    
    vec3 result = texture(uBlurTexture, vTextureCoordinates).rgb * weight[0];
    for (int i = 1; i < kernelSize; ++i) {
        result += texture(uBlurTexture, vTextureCoordinates + offset * i).rgb * weight[i];
        result += texture(uBlurTexture, vTextureCoordinates - offset * i).rgb * weight[i];
    }
    
	oColor = vec4(result, 1.0f);
}
