#version 150

//------------------------------------------------
// Material Textures

uniform sampler2D DiffuseTexture;			// The sampler uniform for the texture.

//------------------------------------------------
// Lights

uniform vec3 AmbientLightColor;             // Minimum light

uniform vec3 DirectionalLightDirection;     // Vector indicating direction of directional light. Should be normalized.
uniform vec3 DirectionalLightDiffuseColor;  // The color of the directional light.

//------------------------------------------------
// Interpolated from vertex shader. (varyings)

in vec4 v_position;							// World coordinate position of this vertex.
in vec3 v_normal;							// World Inverse Transpose of the normal.
in vec3 v_view;								// Vector from the position of this fragment to the viewer.
in vec2 v_uv1;								// Texture coordinates.

//------------------------------------------------
// Color Output

out vec4 FragmentColor;

//----------------------------------
// Program

void main()
{
    vec3 normal = normalize(v_normal);
	vec3 light = DirectionalLightDiffuseColor * max(0.0, dot(normal, DirectionalLightDirection));
    light = clamp(light + AmbientLightColor, 0.0, 1.0);
    FragmentColor = vec4(light, 1.0) * texture(DiffuseTexture, v_uv1);
}