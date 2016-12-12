#pragma once
// local
#include "UniformCameraConfig.h"
/*!
* \brief ǰ����ͨ�������
*/
class UniformCamera
{
public:
    UniformCamera(void);
    ~UniformCamera(void);

    /*!
    * \brief ����viewport
    */
    void set_viewport(int x,int y,int width,int height);

    /*!
    * \brief ���°�Χ��
    */
    void set_bounding_box(float xmin,float xmax,float ymin,float ymax,float zmin,float zmax);

    /*!
    * \brief ���°�Χ��
    */
    void set_bounding_box(const float* new_buffer);

    /*!
    * \breif ��ȡ��Χ��뾶
    */
    float get_bounding_box_radius() const;

    /*!
    * \brief ƽ�ƽ�����Ӧ
    */
    void translate(float startx,float starty,float endx,float endy);

    /*!
    * \brief ���Ž�����Ӧ
    */
    void scale(float delta);

    /*!
    * \brief ��ת������Ӧ
    */
    void rotate(float startx,float starty,float endx,float endy);

    /*!
    * \brief ���������ͶӰ��ʽ
    * \param [in] is_perspective �Ƿ�ʹ�óɽ�͸��ͶӰ
    */
    void set_perspective(bool is_perspective);

	/*!
	* \brief ����ʰȡ����
	*/
	void set_picking(float startx, float starty, float endx, float endy);

    /*!
    * \brief ������ת��ƽ�Ʋ���
    */
    void resetCamera();

    /*!
    * \brief ��õ�ǰ��������õĿ���
    */
    UniformCameraConfig get_config();

    /*!
    * \brief �������������
    */
    void set_config(UniformCameraConfig &new_config);

    /*!
    * \brief ����Ĭ����ת
    */
    void set_default_rotate_status(const float* new_default_rotate_status);

    /*!
    * \brief ����Ĭ��ƽ��
    */
    void set_default_translate_position(const float* new_translate_position);

    /*!
    * \brief ��ȡĬ�ϵ�ƽ������ת״̬
    */
    void load_default_status();

    /*!
    * \brief ��ȡ��ת����
    * \return ��ת���������ָ�룬���ݷֲ�Ϊ������
    */
    const float* get_rotate_matrix();

    /*!
    * \brief ��ȡģ����ͼ����
    * \return ģ����ͼ���������ָ�룬���ݷֲ�Ϊ������
    */
    const float* get_model_view_matrix();

	const glm::mat4& get_model_view_matrix2();

    /*!
    * \brief ��ȡͶӰ����
    * \return ͶӰ���������ָ�룬���ݷֲ�Ϊ������
    */
    const float* get_projection_matrix();

	/*!
	* \brief ��ȡʰȡ����
	* \return ʰȡ���������ָ�룬���ݷֲ�Ϊ������
	*/
	const float* get_picking_matrix();

    /*!
    * \brief ��ȡ�ӿ�
    * \return �ӿڵ�����ָ��
    */
    const int* get_viewport();

    /*!
    * \brief ��ȡ���Ŵ�С
    * \return ���Ŵ�С����
    */
    const float get_scale_parameter();

    void screen_to_world_coordinate(const int x, const int y, float & wx, float & wy, float & wz);
    void world_to_screen_coordinate(const float x, const float y, const float z, int & sx, int & sy);

private:
    /*!
    * \brief ���²ü�ƽ��
    *
    * �ڰ�Χ�С����Ų������º���Ҫ���²ü�ƽ��
    *
    * �ú��������ƽ�Ʋ���
    */
    void update_cull_face();

    /*!
    * \brief �������о���
    *
    * �ڲü�ƽ����º���Ҫ�������о���
    */
    void update_matrix();

    /*!
    * \brief ����ģ�;���
    *
    * ��ƽ����ת�������º���Ҫ����ģ�;���
    */
    void update_model_view_matrix();

    /*!
    * \brief ����ͶӰ����
    * 
    * ���ӿڡ�ͶӰ��ʽ���º���Ҫ����ͶӰ����
    */
    void update_projection_matrix();

    UniformCameraConfig config;
    glm::quat default_rotate_status;
    glm::vec2 default_translate_position;
    glm::mat4 rotate_matrix;
    glm::mat4 mv_matrix;
    glm::mat4 projection_matrix;
	glm::mat4 picking_matrix;
};

