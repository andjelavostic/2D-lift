#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 uColor;
uniform sampler2D uTexture;
uniform bool useTexture;

void main() {
    if (useTexture)
        FragColor = texture(uTexture, TexCoord);
    else
        FragColor = vec4(uColor, 1.0);
}
