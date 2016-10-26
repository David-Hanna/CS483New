#version 150

//----------------------------------
// Transformation uniforms.

uniform mat4 WorldTransform;
uniform mat4 WorldViewTransform;
uniform mat4 WorldViewProjectionTransform;
uniform mat3 WorldInverseTransposeTransform;

//----------------------------------
// Per vertex attributes.

in vec4 a_position;
in vec3 a_normal;
in vec2 a_uv1;

//----------------------------------
// Pass on to the fragment shader. (varyings)

out vec4 v_position;							// World coordinate position of this vertex.
out vec3 v_normal;								// World Inverse Transpose of the normal.
out vec3 v_view;								// Vector from the position of this vertex to the viewer.
out vec2 v_uv1;									// Pass along the texture coordinates to the fragment shader.

//----------------------------------
// Program

void main()
{
    v_position = WorldTransform * a_position;
	v_normal = WorldInverseTransposeTransform * a_normal;
	v_view = -(WorldViewTransform * a_position).xyz;
	v_uv1 = a_uv1;
	gl_Position = WorldViewProjectionTransform * a_position;
}