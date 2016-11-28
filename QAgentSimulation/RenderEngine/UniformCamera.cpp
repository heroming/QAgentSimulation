#include "UniformCamera.h"

// Windows
#include <Windows.h>
// OpenGL
#include <GL/GL.h>
// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

UniformCamera::UniformCamera(void)
{
}

UniformCamera::~UniformCamera(void)
{
}

void UniformCamera::set_viewport(int x,int y,int width,int height)
{
    this->config.viewport.x = x;
    this->config.viewport.y = y;
    this->config.viewport.z = width;
    this->config.viewport.w = height;

    glViewport(x, y,  width, height);
    update_projection_matrix();
}

void UniformCamera::set_bounding_box(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
    // 更新包围球的中心和半径
    glm::vec3 l_dvec_min(xmin, ymin, zmin), l_dvec_max(xmax, ymax, zmax);
    this->config.rotate_center = (l_dvec_min + l_dvec_max) / 2.0f;
    this->config.bounding_sphere_radius = glm::length(l_dvec_max - l_dvec_min) / 2.0f;
    update_cull_face();
}

void UniformCamera::set_bounding_box(const float * new_buffer)
{
    // 更新包围球的中心和半径
    glm::vec3 l_dvec_min(new_buffer[0], new_buffer[2], new_buffer[4]);
    glm::vec3 l_dvec_max(new_buffer[1], new_buffer[3], new_buffer[5]);
    this->config.rotate_center = (l_dvec_min + l_dvec_max) / 2.0f;
    this->config.bounding_sphere_radius = glm::length(l_dvec_max - l_dvec_min) / 2.0f;
    update_cull_face();
}

float UniformCamera::get_bounding_box_radius() const
{
    return config.bounding_sphere_radius;
}

void UniformCamera::translate(float startx, float starty, float endx, float endy)
{
    glm::vec4 view_space_center = this->projection_matrix * glm::vec4(this->config.translate_position, 1.0);
    view_space_center /= view_space_center.w;

    glm::mat4 invert_p_matrix = glm::inverse(this->projection_matrix);
    float float_startx = startx / (this->config.viewport.z - 1) * 2.0 - 1.0;
    float float_starty = starty / (this->config.viewport.w - 1) * -2.0 + 1.0;
    float float_endx = endx / (this->config.viewport.z - 1) * 2.0 - 1.0;
    float float_endy = endy / (this->config.viewport.w - 1) * -2.0 + 1.0;

    glm::vec4 l_start_vec(float_startx, float_starty, view_space_center.z, 1.0);
    l_start_vec = invert_p_matrix * l_start_vec;
    l_start_vec /= l_start_vec.w;
    glm::vec4 l_end_vec(float_endx, float_endy, view_space_center.z, 1.0);
    l_end_vec = invert_p_matrix * l_end_vec;
    l_end_vec /= l_end_vec.w;

    this->config.translate_position.x += l_end_vec.x - l_start_vec.x;
    this->config.translate_position.y += l_end_vec.y - l_start_vec.y;

    update_model_view_matrix();
}

void UniformCamera::scale(float delta)
{
    this->config.scale_parameter *= delta;
    this->update_cull_face();
}

void UniformCamera::rotate(float startx,float starty,float endx,float endy)
{
    glm::mat4 invert_p_matrix = glm::inverse(this->projection_matrix);

    float float_startx = startx / (this->config.viewport.z - 1) * 2.0 - 1.0;
    float float_starty = starty / (this->config.viewport.w - 1) * -2.0 + 1.0;
    float float_endx = endx / (this->config.viewport.z - 1) * 2.0 - 1.0;
    float float_endy = endy / (this->config.viewport.w - 1) * -2.0 + 1.0;

    glm::vec4 l_start_vec(float_startx, float_starty, -1.0, 1.0);
    l_start_vec = invert_p_matrix * l_start_vec;
    l_start_vec /= l_start_vec.w;
    l_start_vec -= glm::vec4(this->config.translate_position, 1.0);
    l_start_vec = glm::normalize(l_start_vec);
    glm::vec4 l_end_vec(float_endx, float_endy, -1.0, 1.0);
    l_end_vec = invert_p_matrix * l_end_vec;
    l_end_vec /= l_end_vec.w;
    l_end_vec -= glm::vec4(this->config.translate_position, 1.0);
    l_end_vec = glm::normalize(l_end_vec);

    glm::vec3 l_axis = glm::cross(glm::vec3(l_start_vec), glm::vec3(l_end_vec));
    float l_dot_product = glm::dot(l_start_vec, l_end_vec);
    if (l_dot_product > 1.0f)
        l_dot_product = 1.0f;
    if (l_dot_product < 0.0f)
        l_dot_product = 0.0f;
    float l_angle = acosf(l_dot_product) / glm::pi<float>() * 180.0f;
    this->config.rotate_status = glm::rotate(glm::quat_cast(glm::mat4(1.0)), l_angle, l_axis) * this->config.rotate_status;

    update_model_view_matrix();
}

void UniformCamera::set_perspective(bool is_perspective)
{
    this->config.is_perspective = is_perspective;
    update_projection_matrix();
}

void UniformCamera::set_picking(float startx, float starty, float endx, float endy)
{
	glm::vec2 l_center((startx + endx) * 0.5f, this->config.viewport.w - (starty + endy) * 0.5f);
	glm::vec2 l_delta(abs(startx - endx) + 1.0f, abs(starty - endy) + 1.0f);
	picking_matrix = glm::pickMatrix(l_center, l_delta, this->config.viewport) * projection_matrix;
}

void UniformCamera::resetCamera()
{
    this->config.rotate_status = glm::quat_cast(glm::mat4(1.0));
    this->config.translate_position.x = 0.0;
    this->config.translate_position.y = 0.0;
    update_model_view_matrix();
}

UniformCameraConfig UniformCamera::get_config()
{
    return this->config;
}

void UniformCamera::set_config(UniformCameraConfig &new_config)
{
    this->config = new_config;
    update_cull_face();
}

void UniformCamera::set_default_rotate_status(const float* new_default_rotate_status)
{
    memcpy_s(glm::value_ptr(default_rotate_status), 4 * sizeof(float), new_default_rotate_status, 4 * sizeof(float));
}

void UniformCamera::set_default_translate_position(const float* new_translate_position)
{
    memcpy_s(glm::value_ptr(default_translate_position), 2 * sizeof(float), new_translate_position, 2 * sizeof(float));
}

void UniformCamera::load_default_status()
{
    config.rotate_status = default_rotate_status;
    config.translate_position.x = default_translate_position.x;
    config.translate_position.y = default_translate_position.y;
    update_model_view_matrix();
}

const float* UniformCamera::get_rotate_matrix()
{
    return glm::value_ptr(this->rotate_matrix);
}

const float* UniformCamera::get_model_view_matrix()
{
    return glm::value_ptr(this->mv_matrix);
}

const glm::mat4& UniformCamera::get_model_view_matrix2()
{
	return mv_matrix;
}


const float* UniformCamera::get_projection_matrix()
{
    return glm::value_ptr(projection_matrix);
}

const float* UniformCamera::get_picking_matrix()
{
	return glm::value_ptr(picking_matrix);
}

const int* UniformCamera::get_viewport()
{
    return glm::value_ptr(this->config.viewport);
}

const float UniformCamera::get_scale_parameter() {
    return config.scale_parameter;
}

// private
void UniformCamera::update_cull_face()
{
    double scaled_bounding_sphere_radius = this->config.bounding_sphere_radius * this->config.scale_parameter;
    this->config.translate_position.z = -3 * scaled_bounding_sphere_radius;
    // TODO: to conform a proper shift value
    this->config.face_far = 4.1 * scaled_bounding_sphere_radius;
    this->config.face_near = 1.9 * scaled_bounding_sphere_radius;
    
    update_matrix();
}

void UniformCamera::update_matrix()
{
    update_model_view_matrix();
    update_projection_matrix();
}

void UniformCamera::update_model_view_matrix()
{
    this->mv_matrix = glm::translate(glm::mat4(1.0), this->config.translate_position);
    this->rotate_matrix = glm::mat4_cast(this->config.rotate_status);
    this->mv_matrix = this->mv_matrix * this->rotate_matrix;
    this->mv_matrix = glm::scale(this->mv_matrix, glm::vec3(this->config.scale_parameter));
    this->mv_matrix = glm::translate(this->mv_matrix, -this->config.rotate_center);
}

void UniformCamera::update_projection_matrix()
{
    float view_half_width = this->config.bounding_sphere_radius * 1.2;
    float view_half_height = view_half_width;
    if (this->config.viewport.z > this->config.viewport.w)
    {
        view_half_width *= (float)this->config.viewport.z / this->config.viewport.w;
    }
    else
    {
        view_half_height *= (float)this->config.viewport.w / this->config.viewport.z;
    }

    if (this->config.is_perspective)
    {
        this->projection_matrix = glm::frustum(-view_half_width, view_half_width, -view_half_height, view_half_height, this->config.face_near, this->config.face_far);
    }
    else
    {
        this->projection_matrix = glm::ortho(-view_half_width, view_half_width, -view_half_height, view_half_height, this->config.face_near, this->config.face_far);
    }
}
