#version 150

uniform sampler2D DiffuseTexture;			// The sampler uniform for the texture.

in vec4 v_position;							// World coordinate position of this vertex.
in vec3 v_normal;							// World Inverse Transpose of the normal.
in vec3 v_view;								// Vector from the position of this fragment to the viewer.
in vec2 v_uv1;								// Pass along the texture coordinates to the fragment shader.

out vec4 FragmentColor;						// Final color for this fragment.

void main()
{
    FragmentColor = texture(DiffuseTexture, v_uv1);
}