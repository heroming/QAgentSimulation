#pragma once
// local
#include "UniformCameraConfig.h"
/*!
* \brief 前后处理通用摄像机
*/
class UniformCamera
{
public:
    UniformCamera(void);
    ~UniformCamera(void);

    /*!
    * \brief 更新viewport
    */
    void set_viewport(int x,int y,int width,int height);

    /*!
    * \brief 更新包围球
    */
    void set_bounding_box(float xmin,float xmax,float ymin,float ymax,float zmin,float zmax);

    /*!
    * \brief 更新包围球
    */
    void set_bounding_box(const float* new_buffer);

    /*!
    * \breif 获取包围球半径
    */
    float get_bounding_box_radius() const;

    /*!
    * \brief 平移交互响应
    */
    void translate(float startx,float starty,float endx,float endy);

    /*!
    * \brief 缩放交互响应
    */
    void scale(float delta);

    /*!
    * \brief 旋转交互响应
    */
    void rotate(float startx,float starty,float endx,float endy);

    /*!
    * \brief 更新摄像机投影方式
    * \param [in] is_perspective 是否使用成角透视投影
    */
    void set_perspective(bool is_perspective);

	/*!
	* \brief 设置拾取矩阵
	*/
	void set_picking(float startx, float starty, float endx, float endy);

    /*!
    * \brief 重置旋转与平移参数
    */
    void resetCamera();

    /*!
    * \brief 获得当前摄像机配置的拷贝
    */
    UniformCameraConfig get_config();

    /*!
    * \brief 设置摄像机配置
    */
    void set_config(UniformCameraConfig &new_config);

    /*!
    * \brief 设置默认旋转
    */
    void set_default_rotate_status(const float* new_default_rotate_status);

    /*!
    * \brief 设置默认平移
    */
    void set_default_translate_position(const float* new_translate_position);

    /*!
    * \brief 读取默认的平移与旋转状态
    */
    void load_default_status();

    /*!
    * \brief 获取旋转矩阵
    * \return 旋转矩阵的数据指针，数据分布为列主序
    */
    const float* get_rotate_matrix();

    /*!
    * \brief 获取模型视图矩阵
    * \return 模型视图矩阵的数据指针，数据分布为列主序
    */
    const float* get_model_view_matrix();

	const glm::mat4& get_model_view_matrix2();

    /*!
    * \brief 获取投影矩阵
    * \return 投影矩阵的数据指针，数据分布为列主序
    */
    const float* get_projection_matrix();

	/*!
	* \brief 获取拾取矩阵
	* \return 拾取矩阵的数据指针，数据分布为列主序
	*/
	const float* get_picking_matrix();

    /*!
    * \brief 获取视口
    * \return 视口的数据指针
    */
    const int* get_viewport();

    /*!
    * \brief 获取缩放大小
    * \return 缩放大小参数
    */
    const float get_scale_parameter();

    void screen_to_world_coordinate(const int x, const int y, float & wx, float & wy, float & wz);
    void world_to_screen_coordinate(const float x, const float y, const float z, int & sx, int & sy);

private:
    /*!
    * \brief 更新裁剪平面
    *
    * 在包围盒、缩放参数更新后，需要更新裁剪平面
    *
    * 该函数会更新平移参数
    */
    void update_cull_face();

    /*!
    * \brief 更新所有矩阵
    *
    * 在裁剪平面更新后，需要更新所有矩阵
    */
    void update_matrix();

    /*!
    * \brief 更新模型矩阵
    *
    * 在平移旋转参数更新后，需要更新模型矩阵
    */
    void update_model_view_matrix();

    /*!
    * \brief 更新投影矩阵
    * 
    * 在视口、投影方式更新后，需要更新投影矩阵
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

