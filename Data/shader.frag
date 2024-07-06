#version 430

noperspective in vec3 distanceEdge;
in vec3 fragPos;
in vec3 normal;
in vec3 lightPosMvp;

out vec4 o_color;

uniform vec4 u_solidColor;
uniform vec4 u_wireColor;
uniform vec4 u_lightColor;

const float ambientStrength = 0.2;
const float specularStrength = 0.5;

void main() {
    // couleur ambient
    vec3 ambient = ambientStrength * u_lightColor.xyz;
    
    // couleur diffuse
    vec3 lightDir = normalize(lightPosMvp - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor.xyz;

    vec3 solidColorLight = (ambient + diffuse) * u_solidColor.xyz;

    if (u_wireColor.a < 0.5) {
        // on affiche pas les wire
        o_color = vec4(solidColorLight, u_solidColor.a);
    } else {
        // calcul des wire
        float d = min(distanceEdge.x, min(distanceEdge.y, distanceEdge.z));
        float I = exp2(-0.5*d*d);
        // on affiche les wire
        o_color = mix(vec4(solidColorLight, u_solidColor.a), vec4(u_wireColor.xyz, 1.0f), I);
    }
}