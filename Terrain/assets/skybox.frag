uniform samplerCube cubeMapTex;

in vec3 pos;

out vec4 outColor;

void main(void) {
    // grab color at current position
    outColor = texture(cubeMapTex, pos);
}
