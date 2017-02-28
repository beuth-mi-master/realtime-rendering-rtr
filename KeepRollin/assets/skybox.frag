uniform samplerCube uCubeTexture;

in vec3 vPos;

out vec4 outColor;

void main(void) {
    outColor = texture(uCubeTexture, vPos) * vec4(0.8);
}
