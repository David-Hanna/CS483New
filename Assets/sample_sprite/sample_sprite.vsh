#version 150

//----------------------------------
// Transformation uniforms.

uniform mat4 WorldViewProjectionTransform;

//----------------------------------
// Per vertex attributes.

in vec4 a_position;
in vec2 a_uv1;

//----------------------------------
// Pass on to the fragment shader. (varyings)

out vec2 v_uv1;									// Pass along the texture coordinates to the fragment shader.

//----------------------------------
// Program

void main()
{
	v_uv1 = a_uv1;
	gl_Position = WorldViewProjectionTransform * a_position;
}