#pragma once

#include <string>
#include <vector>

class CreateRoad
{

public :
    std::string get_init_path() const;
    void set_init_path(const std::string & s);

    std::string get_save_path() const;
    void set_save_path(const std::string & s);

    int get_radius() const;
    void set_radius(const int x);

    int get_length() const;
    void set_length(const int x);

    float get_x();
    float get_y();

    bool is_begin() const;
    void set_begin(const bool flag);

    const std::vector<int> & get_index() const;
    const std::vector<float> & get_point() const;

public :
    // 支持分阶段对道路进行创建，只需要将之前的中间结果导入就可以了
    void load_data(const std::string & l_path);

    // 用户交互选择点(l_x, l_y)
    void point_clicked(const float l_x, const float l_y);

    // 支持删除最近定义的道路，以对错误标记进行修改
    void delete_recent();

    // 初始化
    void clear();

    // 保存最新结果
    void save() const;

private :

    // 验证点(x, y)是否有效，即是否在地图之中
    bool valid(const int x, const int y) const;

    bool valid(const float x, const float y) const;

    // 判断某点是否已经被标记，防止对一个点(或是附近的点)标记多次
    int get_vertex_id(const float x, const float y) const;

    // 对点(x, y)进行标记，其标识符标记为id
    void update_vertex_id(const float x, const float y, const int id);

    // 获取(x, y)附近邻域中空间最大的点
    void get_vertex_position(float & x, float & y) const;

    // 获取格点(x, y)的空间值
    int get_point_space(const int x, const int y) const;

    // 获取格点(x, y)的空间值
    int get_point_space_by_clearance(const int x, const int y) const;

    // 判断半径为r的空间是否是合理的
    bool check_space(const int x, const int y, const int r) const;

private :

    int m_row;
    int m_col;

    float m_x;
    float m_y;
    bool m_begin;

    int m_radius;
    int m_length;

    std::string m_init_path;
    std::string m_save_path;

    int m_point_id;

    std::vector<int> m_index;
    std::vector<float> m_point;

    std::vector<int> m_use;
    std::vector<std::vector<char>> m_grid;
    std::vector<std::vector<int>> m_clearance;
    std::vector<std::vector<int>> m_vert;
};