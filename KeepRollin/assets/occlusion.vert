#version 330

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelMatrix;

uniform float uPositionOfBall;

in vec4 ciPosition;

out float gl_ClipDistance[1];

void main(void) {
    gl_Position = ciModelViewProjection * ciPosition;
    gl_ClipDistance[0] = dot(ciModelMatrix * ciPosition, vec4(0, 0, -1, uPositionOfBall));
}
