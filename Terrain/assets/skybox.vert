uniform mat4 ciProjectionMatrix;
uniform mat4 ciViewMatrix;
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;

out vec3 pos;

void main(void) {
    // export pos to frag shader
    pos = vec3(ciPosition);
    gl_Position = ciModelViewProjection * ciPosition;
}
