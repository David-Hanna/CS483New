#version 150

//------------------------------------------------
// Material Textures

uniform sampler2D DiffuseTexture;			// The sampler uniform for the texture.

//------------------------------------------------
// Interpolated from vertex shader. (varyings)

in vec2 v_uv1;								// Texture coordinates.

//------------------------------------------------
// Color Output

out vec4 FragmentColor;

//----------------------------------
// Program

void main()
{
    FragmentColor = texture(DiffuseTexture, v_uv1);
	if (FragmentColor.a == 0)
		discard;
}