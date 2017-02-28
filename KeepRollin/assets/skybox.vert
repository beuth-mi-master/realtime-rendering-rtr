uniform mat4 ciModelViewProjection;

uniform mat4 uRotation;

in vec4 ciPosition;

out vec3 vPos;

void main(void) {
    vPos = ciPosition.xyz;
    gl_Position = ciModelViewProjection * (uRotation * ciPosition);
}
