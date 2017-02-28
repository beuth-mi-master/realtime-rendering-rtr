uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec4 vertexPosition;
out vec3 normalDirection;
out vec2 textureCoordinates;

void main(void) {
    gl_Position = ciModelViewProjection * ciPosition;
    vertexPosition = ciModelView * ciPosition;
    normalDirection = ciNormalMatrix * ciNormal;
    textureCoordinates = ciTexCoord0;
}
