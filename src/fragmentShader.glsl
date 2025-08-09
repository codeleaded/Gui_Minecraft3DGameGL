#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec2 shade;

void main() {
    vec4 texColor = texture(texture1, fragTexCoord);
    float lighting = max(dot(normalize(fragNormal), normalize(vec3(-0.5, 0.6, -0.4))), 0.1);
    FragColor = texColor * max(shade.x * shade.y,0.02);
}
