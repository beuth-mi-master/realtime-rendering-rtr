#version 330

uniform mat4 ciModelViewProjection;
uniform mat4 ciViewMatrixInverse;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelMatrix;

uniform vec2 uResolution = vec2(10.0f);

uniform mat4 uShadowMatrixLeft;
uniform mat4 uShadowMatrixFront;
uniform mat4 uShadowMatrixRight;

uniform vec3 uShadowLightPosition;

in vec4 ciPosition;
in vec3 ciNormal;
in vec3 ciTangent;
in vec2 ciTexCoord0;

out vec4 vVertexPosition;
out vec2 vTextureCoordinates;
out mat3 TBN;
out vec4 vViewPos;
out vec4 vShadowCoordLeft;
out vec4 vShadowCoordFront;
out vec4 vShadowCoordRight;

const mat4 biasMatrix = mat4( 0.5, 0.0, 0.0, 0.0,
							  0.0, 0.5, 0.0, 0.0,
							  0.0, 0.0, 0.5, 0.0,
							  0.5, 0.5, 0.5, 1.0 );

void main(void) {
    
    vec3 wcNormal = (ciModelMatrix * vec4(ciNormal, 0)).xyz;
    vec3 wcTangent = (ciModelMatrix * vec4(ciTangent, 0)).xyz;
    vec3 wcBitangent = cross(wcNormal, wcTangent);

    TBN = inverse(mat3(wcTangent, wcBitangent, wcNormal));
    
    vVertexPosition = ciModelMatrix * ciPosition;
    vTextureCoordinates = ciTexCoord0 * uResolution;
    
    vShadowCoordLeft = biasMatrix * uShadowMatrixLeft * ciModelMatrix * ciPosition;
    vShadowCoordFront = biasMatrix * uShadowMatrixFront * ciModelMatrix * ciPosition;
    vShadowCoordRight = biasMatrix * uShadowMatrixRight * ciModelMatrix * ciPosition;

    vViewPos = ciModelViewProjection * ciPosition;
    
    gl_Position = vViewPos;
    
}
