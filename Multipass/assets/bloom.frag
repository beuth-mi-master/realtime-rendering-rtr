uniform sampler2D	uTex0;
uniform sampler2D   uTex1;

in vec2 vTexCoord0;

out vec4 oColor;

void main() {
    oColor = vec4(texture(uTex0, vTexCoord0).rgb + texture(uTex1, vTexCoord0).rgb , 1.0);
}
