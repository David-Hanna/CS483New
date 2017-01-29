#version 150

//------------------------------------------------
// Material Textures

uniform sampler2D Texture;			// The sampler uniform for the texture.

//------------------------------------------------
// Interpolated from vertex shader. (varyings)

in vec4 v_color;
in vec2 v_uv1;								// Texture coordinates.

//------------------------------------------------
// Color Output

out vec4 FragmentColor;
out vec4 PixelColor;

//----------------------------------
// Program

void main()
{
    FragmentColor = v_color * texture(Texture, v_uv1);
}