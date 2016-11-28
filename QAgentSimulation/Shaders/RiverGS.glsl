#version 330

uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 m_normal;
out vec3 m_position;

void main()
{
    vec3 l_vec_1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 l_vec_2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 l_normal = normalize(mat3(modelview_matrix) * cross(l_vec_1, l_vec_2));

    m_normal = l_normal;
    m_position = (modelview_matrix * gl_in[0].gl_Position).xyz;
    gl_Position = projection_matrix * vec4(m_position, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();

    m_normal = l_normal;
    m_position = (modelview_matrix * gl_in[1].gl_Position).xyz;
    gl_Position = projection_matrix * vec4(m_position, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();

    m_normal = l_normal;
    m_position = (modelview_matrix * gl_in[2].gl_Position).xyz;
    gl_Position = projection_matrix * vec4(m_position, 1.0);
    gl_PrimitiveID = gl_PrimitiveIDIn;
    EmitVertex();
}