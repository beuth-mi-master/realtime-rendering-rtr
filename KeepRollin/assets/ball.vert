#version 330

uniform mat4 ciModelViewProjection;

uniform vec2 uResolution = vec2(1.0f);
uniform mat4 uRotation;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 vTextureCoordinates;

void main(void) {
    vTextureCoordinates = ciTexCoord0 * uResolution;
    gl_Position = ciModelViewProjection * (uRotation * ciPosition);
}
