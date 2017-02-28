#version 330

uniform float exposure = 0.0024;
uniform float decay = 1.0;
uniform float density = 0.84;
uniform float weight = 5.65;

uniform vec2 uLightPositionOnScreen;
uniform sampler2D uOccludedTexture;

in vec2 vTextureCoordinates;

const int NUM_SAMPLES = 100;

out vec4 oColor;

void main(){
    
    oColor = texture(uOccludedTexture, vTextureCoordinates);

    vec2 texCoord = vTextureCoordinates;
    vec2 deltaTextCoord = vec2(texCoord - uLightPositionOnScreen.xy );
    deltaTextCoord *= 1.0 / float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        texCoord -= deltaTextCoord;
        vec4 sample = texture(uOccludedTexture, texCoord);
        sample *= illuminationDecay * weight;
        oColor += sample;
        illuminationDecay *= decay;
    }
    
    oColor *= exposure;
}
