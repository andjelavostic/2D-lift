#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 uColor;          
uniform sampler2D uTexture;   
uniform bool useTexture;      
uniform bool uHighlight;      
uniform float highlightWidth; // širina okvira (0-0.5)
uniform float uAlpha = 1.0;

void main() {
    vec4 baseColor = useTexture ? texture(uTexture, TexCoord) : vec4(uColor, 1.0);

    if(uHighlight){
        // centar dugmeta u UV koordinatama
        vec2 center = vec2(0.5, 0.5);
        float dist = distance(TexCoord, center); // udaljenost od centra

        // okvir je kada je dist blizu 0.5 (rub dugmeta)
        if(dist > 0.5 - highlightWidth){
            baseColor.rgb = vec3(1.0); // bela boja
        }
    }
    baseColor.a *= uAlpha; // primeni providnost
    FragColor = baseColor;
}

