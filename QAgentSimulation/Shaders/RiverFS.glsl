#version 330


in vec3 m_normal;
in vec3 m_position;

layout (location = 0) out vec4 color;

void main()
{
    vec4 l_color = vec4(0.52, 0.75, 0.91, 1.0);

    vec3 l_camera_direction = normalize(-m_position);
    vec3 l_light_direction = normalize(vec3(2.0, 0.5, -2.0)-m_position);
    vec3 l_half_vector = normalize(l_light_direction + l_camera_direction);

    float l_diffuse_factor = max(0.0, dot(m_normal, l_light_direction));
    float l_specular_factor = max(0.0, dot(m_normal, l_half_vector));

    if (l_diffuse_factor == 0.0)
    {
        l_specular_factor = 0.0;
    }
    else
    {
        l_specular_factor = pow(l_specular_factor, 10);
    }

    vec3 l_ambient_color = vec3(0.5);
    vec3 l_diffuse_color = vec3(l_diffuse_factor * 0.7);
    vec3 l_specular_color = vec3(l_specular_factor * 0.5);

	color = vec4(min(l_color.xyz, l_color.xyz * (l_ambient_color + l_diffuse_color + l_specular_color)), 1.0);
}
