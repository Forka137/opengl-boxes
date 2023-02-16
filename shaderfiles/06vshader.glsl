//VERTEX SHADER
#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// v_clip = m_projection * m_view * m_model * v_local
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	texCoord = aTexCoord;
}