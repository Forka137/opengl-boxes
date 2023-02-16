//FRAGMENT SHADER
#version 440 core
in vec2 texCoord;
out vec4 FragColor;

uniform float mixValue;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
	// 0.2 es 80% textura 1 y 20% textura 2
	FragColor = mix(texture(texture1, texCoord),texture(texture2, texCoord), mixValue);
}