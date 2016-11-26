#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class UniformCameraConfig
{
public:
    UniformCameraConfig(void);
    ~UniformCameraConfig(void);
    void load_default_config();

    // rotate
    glm::vec3 rotate_center; //! 旋转中心，始终为包围球中心
    glm::quat rotate_status; //! 当前旋转参数
    // translate
    glm::vec3 translate_position; //! 当前平移参数
    // scale
    float scale_parameter; //! 当前缩放参数

    // projection
    bool is_perspective; //! 当前投影状态
    float face_far, face_near; //! 最远裁剪面

    // viewport
    glm::ivec4 viewport; //! 视口

    // bounding sphere
    float bounding_sphere_radius; //! 包围球半径
};

