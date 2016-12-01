#version 330

uniform mat4 modelview_matrix;
uniform mat4 projection_matrix;

layout (location = 0) in vec3 position;

void main()
{
    gl_PointSize = 3.0;
    gl_Position = projection_matrix * (modelview_matrix * vec4(position, 1.0));
}
