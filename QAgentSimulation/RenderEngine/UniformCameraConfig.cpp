#include "UniformCameraConfig.h"


UniformCameraConfig::UniformCameraConfig(void)
{
    this->load_default_config();
}

UniformCameraConfig::~UniformCameraConfig(void)
{
}

void UniformCameraConfig::load_default_config()
{
    // rotate
    this->rotate_center = glm::vec3(0.0f);
    this->rotate_status = glm::quat_cast(glm::mat4(1.0f));
    // translate
    this->translate_position = glm::vec3(0.0f, 0.0f, -3.0f);
    // scale
    this->scale_parameter = 1.0f;
    // projection
    this->is_perspective = false;
    this->face_far = 5.0f;
    this->face_near = 1.0f;
    // viewport
    this->viewport = glm::ivec4(0, 0, 1, 1);
    // bounding box
    this->bounding_sphere_radius = 1.0f;
}