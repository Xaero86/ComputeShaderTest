#version 430

in vec4 vertex;

uniform mat4 u_mvpMatrix;

void main() {
	gl_Position = u_mvpMatrix * vertex;
}
