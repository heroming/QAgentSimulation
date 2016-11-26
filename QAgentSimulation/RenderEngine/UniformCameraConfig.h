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
    glm::vec3 rotate_center; //! ��ת���ģ�ʼ��Ϊ��Χ������
    glm::quat rotate_status; //! ��ǰ��ת����
    // translate
    glm::vec3 translate_position; //! ��ǰƽ�Ʋ���
    // scale
    float scale_parameter; //! ��ǰ���Ų���

    // projection
    bool is_perspective; //! ��ǰͶӰ״̬
    float face_far, face_near; //! ��Զ�ü���

    // viewport
    glm::ivec4 viewport; //! �ӿ�

    // bounding sphere
    float bounding_sphere_radius; //! ��Χ��뾶
};

