#version 150
uniform mat4 ProjViewWorld;
in vec4 a_position;
in vec2 a_uv1;
out vec2 v_uv1;

void main()
{
    gl_Position = ProjViewWorld * a_position;
	v_uv1 = a_uv1;
}
