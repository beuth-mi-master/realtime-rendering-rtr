uniform sampler2D	uTex0;
uniform vec2		sample_offset;
uniform float		attenuation;
uniform float       radius;

const float gauss[21] = float[21](
                            0.009167927656011385,
                            0.014053461291849008,
                            0.020595286319257878,
                            0.028855245532226279,
                            0.038650411513543079,
                            0.049494378859311142,
                            0.060594058578763078,
                            0.070921288047096992,
                            0.079358891804948081,
                            0.084895951965930902,
                            0.086826196862124602,
                            0.084895951965930902,
                            0.079358891804948081,
                            0.070921288047096992,
                            0.060594058578763078,
                            0.049494378859311142,
                            0.038650411513543079,
                            0.028855245532226279,
                            0.020595286319257878,
                            0.014053461291849008,
                            0.009167927656011385
                                  
);

in vec2 vTexCoord0;

out vec4 oColor;

void main() {
    vec3 sum = vec3(0.0, 0.0, 0.0);
    
    for (int i = -10; i <= 10; i++) {
        sum += texture(uTex0, vTexCoord0 + i * sample_offset * radius).rgb * gauss[i+10];
    }
    
    oColor.rgb = attenuation * sum;
    oColor.a = 1.0;
}
