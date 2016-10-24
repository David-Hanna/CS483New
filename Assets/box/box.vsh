#version 150

uniform mat4 WorldTransform;					// World Transform.
uniform mat4 WorldViewTransform;				// World View Transform.
uniform mat4 WorldViewProjectionTransform;		// World View Projection Transform.
uniform mat3 WorldInverseTransposeTransform;	// The Transpose of the Inverse of the World Transform.

in vec4 a_position;								// This vertex's position.
in vec3 a_normal;								// This vertex's normal vector.
in vec2 a_uv1;									// This vertex's texture coordinates.

out vec4 v_position;							// World coordinate position of this vertex.
out vec3 v_normal;								// World Inverse Transpose of the normal.
out vec3 v_view;								// Vector from the position of this vertex to the viewer.
out vec2 v_uv1;									// Pass along the texture coordinates to the fragment shader.

void main()
{
    gl_Position = WorldViewProjectionTransform * a_position;
	v_uv1 = a_uv1;
}