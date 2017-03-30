#version 430

layout (std430, binding = 0) buffer buffer_object
{
	uint road_selection[];
};

layout (location = 0) out vec4 color;

void main()
{
    uint l_base = gl_PrimitiveID >> 2;
	uint l_inner = gl_PrimitiveID & 3;
	uint l_status = uint((road_selection[l_base] >> (l_inner * 8)) & 255);
    
    if (l_status == 0) discard;

    color = vec4(0.20, 0.87, 0.97, 0.5);
}
