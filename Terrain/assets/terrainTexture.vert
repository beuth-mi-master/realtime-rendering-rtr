uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;
uniform mat3 ciNormalMatrix;

// position of light
uniform vec4 wcLightPosition;

// texture position - gets translated for endless flyover effect
uniform vec3 texturePosition = vec3(0.0f, 0.0f, 0.0f);

// displacement effect
uniform float displacementEffectScale = 0.5f;

// delta timing for fluent animations
uniform float deltaTiming = 1.0f;

// en- / disable displacement map
uniform bool isDisplacementEnabled = true;

uniform sampler2D displacementTexture;

in vec2 ciTexCoord0;
in vec3 ciTangent;
in vec3 ciNormal;
in vec4 ciPosition;

out vec4 viewPos;
out vec3 tsViewDir;
out vec3 tsLightDir;
out vec2 texCoords;

void main(void) {
        
    // calculating TBN matrix
    vec3 wcNormal = (ciModelMatrix * vec4(ciNormal, 0)).xyz;
    vec3 wcTangent = (ciModelMatrix * vec4(ciTangent, 0)).xyz;
    vec3 wcBitangent = cross(wcNormal, wcTangent);
    
    mat3 TBN = mat3(wcTangent, wcBitangent, wcNormal);

    // calculating light direction and view direction in world coordinates
    vec3 wcLightDir = (wcLightPosition - ciPosition).xyz;
    vec3 wcViewDir = (ciPosition * ciModelMatrix).xyz;
    
    // conversion to tangent space for normal map
    tsLightDir = -wcLightDir * TBN;
    tsViewDir = wcViewDir * TBN;

    // export texCoords - delta timing for smooth animation implemented
    texCoords = ciTexCoord0 + (texturePosition.xz * deltaTiming);
    
    // calculate displacement from red channel and multiply with effect scale
    float displacement = texture(displacementTexture, texCoords).r * displacementEffectScale;
    
    // calculate vertex position
    vec4 pos = isDisplacementEnabled ? ciPosition + vec4(ciNormal, 0) * displacement : ciPosition;
    
    // position to clip coordinates
    gl_Position = ciModelViewProjection * pos;
    
    // export glPos for use in frag shader
    viewPos = gl_Position;
}
