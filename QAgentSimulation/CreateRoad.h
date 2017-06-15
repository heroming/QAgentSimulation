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
    // ֧�ַֽ׶ζԵ�·���д�����ֻ��Ҫ��֮ǰ���м�������Ϳ�����
    void load_data(const std::string & l_path);

    // �û�����ѡ���(l_x, l_y)
    void point_clicked(const float l_x, const float l_y);

    // ֧��ɾ���������ĵ�·���ԶԴ����ǽ����޸�
    void delete_recent();

    // ��ʼ��
    void clear();

    // �������½��
    void save() const;

private :

    // ��֤��(x, y)�Ƿ���Ч�����Ƿ��ڵ�ͼ֮��
    bool valid(const int x, const int y) const;

    bool valid(const float x, const float y) const;

    // �ж�ĳ���Ƿ��Ѿ�����ǣ���ֹ��һ����(���Ǹ����ĵ�)��Ƕ��
    int get_vertex_id(const float x, const float y) const;

    // �Ե�(x, y)���б�ǣ����ʶ�����Ϊid
    void update_vertex_id(const float x, const float y, const int id);

    // ��ȡ(x, y)���������пռ����ĵ�
    void get_vertex_position(float & x, float & y) const;

    // ��ȡ���(x, y)�Ŀռ�ֵ
    int get_point_space(const int x, const int y) const;

    // ��ȡ���(x, y)�Ŀռ�ֵ
    int get_point_space_by_clearance(const int x, const int y) const;

    // �жϰ뾶Ϊr�Ŀռ��Ƿ��Ǻ����
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