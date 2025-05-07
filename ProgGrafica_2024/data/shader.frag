#version 330

#define MAX_LIGHTS 8

struct Light {
    int type;         // 0 = directional, 1 = point
    vec4 position;
    vec4 color;
    float intensity;
    vec4 direction;
};

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;

uniform sampler2D uTexture;
uniform vec4 uViewPos;
uniform float uKd = 0.8;  // Coef. difuso
uniform float uKs = 0.5;  // Coef. especular
uniform int uShininess = 32;

in vec4 fColor;
in vec4 fNormal;
in vec4 fFragPos;
in vec4 fTextureCoord;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTexture, fTextureCoord.xy);
    if (texColor.a < 0.1) discard;

    vec3 result = vec3(0.0);
    vec3 norm = normalize(fNormal.xyz);
    vec3 viewDir = normalize(uViewPos.xyz - fFragPos.xyz);

    for (int i = 0; i < uNumLights; ++i) { // Iterar sobre las luces y calcular la iluminación
        vec3 lightDir;

        if (uLights[i].type == 0) {
            lightDir = normalize(-uLights[i].direction.xyz);  // Direccional
        } else {
            lightDir = normalize(uLights[i].position.xyz - fFragPos.xyz);  // Puntual
        }

        // Ambiental
        vec3 ambient = 0.1 * uLights[i].color.rgb;

        // Difusa
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = uKd * diff * uLights[i].color.rgb;

        // Epecular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
        vec3 specular = uKs * spec * uLights[i].color.rgb;

        // Resultado parcial
        result += (ambient + diffuse + specular) * uLights[i].intensity;
    }

    result *= texColor.rgb * fColor.rgb;
    FragColor = vec4(result, texColor.a);
}