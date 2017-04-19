#version 150

//------------------------------------------------
// Material Textures

uniform sampler2D DiffuseTexture;			// The sampler uniform for the texture.

//------------------------------------------------
// Lights

uniform vec3 AmbientLightColor;             // Minimum light

uniform vec3 DirectionalLightDirection;     // Vector indicating direction of directional light. Should be normalized.
uniform vec3 DirectionalLightDiffuseColor;  // The color of the directional light.

uniform vec3 PointLightPosition;            // Position of the point light.
uniform vec3 PointLightDiffuseColor;        // Color of the point light.
uniform vec3 PointLightAttenuation;         // How the light decreases with distance.
uniform float PointLightRange;              // Maximum range the point light lights.

//------------------------------------------------
// Interpolated from vertex shader. (varyings)

in vec4 v_position;							// World coordinate position of this vertex.
in vec3 v_normal;							// World Inverse Transpose of the normal.
in vec2 v_uv1;								// Texture coordinates.

//------------------------------------------------
// Color Output

out vec4 FragmentColor;

//----------------------------------
// Program

void main()
{
    vec3 normal = normalize(v_normal);

    //-----------------------------------------
    // Directional light computation.

	vec3 DirectionalLight = DirectionalLightDiffuseColor * max(0.0, dot(normal, DirectionalLightDirection));

    //-----------------------------------------
    // Point light computation.

    vec3 PointLightDirectionVector = PointLightPosition - v_position.xyz;
    float distance = length(PointLightDirectionVector);

    vec3 PointLight = vec3(0.0, 0.0, 0.0);

    //if (distance <= PointLightRange)
    //{
        PointLightDirectionVector /= distance;
        PointLight = clamp(PointLightDiffuseColor * max(0.0, dot(normal, PointLightDirectionVector)), 0.0, 1.0);
        PointLight *= (1.0 / dot(PointLightAttenuation, vec3(1.0, distance, distance * distance)));
    //}

    //----------------------------------------
    // Total light computation

    vec3 TotalLight = clamp((AmbientLightColor + DirectionalLight + PointLight) * 1.5, 0.0, 1.0);
    FragmentColor = vec4(TotalLight, 1.0) * texture(DiffuseTexture, v_uv1);
}