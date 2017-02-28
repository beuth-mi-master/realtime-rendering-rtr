uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

uniform vec4 light1;
uniform vec4 light2;

uniform vec3 ambientLightColor;
uniform vec3 light1Color;
uniform vec3 light2Color;
uniform vec3 k_ambient;
uniform vec3 k_diffuse;
uniform vec3 k_specular;
uniform float shininess;
uniform vec2 resolution;
uniform float time;
uniform bool shouldColorChange;
uniform vec4 color1;
uniform vec4 color2;

in vec4 vertexPosition;
in vec3 normalDirection;
in vec2 textureCoordinates;

out vec4 outColor;

vec3 phong(vec3 n, vec3 v, vec3 l, vec3 lColor) {
    vec3 ambient = k_ambient * ambientLightColor;
    
    float ndotl = dot(n, -l);
    if(ndotl < 0.0f)
        return vec3(0.0f, 0.0f, 0.0f);
    
    vec3 diffuse;
    if (shouldColorChange) {
        vec2 uv = vec2(textureCoordinates.x - 0.25 * resolution.x, textureCoordinates.y) / resolution.y;
        uv = 2.0 * uv - 1.0;
        float wave = sin(time * 2.0);
        float circle = (uv.x * uv.x + uv.y * uv.y) * 0.2;
        diffuse =  vec3(mix(color1, color2, circle + wave)) * lColor * ndotl;
    } else if (!shouldColorChange){
        diffuse =  k_diffuse * lColor * ndotl;
    }
    
    vec3 r = reflect(l, n);
    float rdotv = max(dot(r, v), 0.0f);
    vec3 specular = k_specular * lColor * pow(rdotv, shininess);
    
    return ambient + diffuse + specular;
}

void main(void) {
    vec3 n = normalize(normalDirection);
    vec3 l1 = normalize(vertexPosition - light1).xyz;
    vec3 l2 = normalize(vertexPosition - light2).xyz;
    vec3 v = (ciProjectionMatrix[2][3] != 0.0) ? normalize(-vertexPosition.xyz) : vec3(0.0f, 0.0f, 1.0f);
    vec3 color = phong(n, v, l1, light1Color) + phong(n, v, l2, light2Color);
    outColor = vec4(color, 1.0f);
}
