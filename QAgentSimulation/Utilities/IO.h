#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>

#include <QtGui>

class IO
{
public:
    static const int PROCESS_STEP = 50;

    static bool file_exist(const std::string & path);

    /*******************************************************************************
    ** ͨ�ú��������ڵ���GLSL Shader�Լ���Ҫ��Ⱦ�ĳ�������
    *******************************************************************************/
    static bool load_shader(const std::string & path, std::vector<char> & data);
    static bool load_point_data(const std::string & path, std::vector<float> & point);
    static bool load_line_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_triangle_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);


    /*******************************************************************************
    ** ��5400*9600�ĵ�ͼ���浽�ļ��У�����-1�Ǳ�������0�ǿ�ͨ������1���ϰ���
    *******************************************************************************/
    static bool load_damage(const std::string & path, std::vector<int> & x_coordinate,
        std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data);
    static bool save_damage_data(const std::string & path,
        const std::vector<std::vector<char>> & data, bool binary = false);


    /********************************************************************************
    ** ��·��Ϣ�����ֱ��ȡ��·�Ľڵ��������Ϣ���Լ�������·��������Ϣ��
    *********************************************************************************/
    static bool load_graph_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_graph_attribute(const std::string & path,
        const std::string & name, std::vector<float> & data);


    /********************************************************************************
    ** ��agent���ݽ�����Ԥ����ֻ������agent��λ�ú�id��Ϣ
    *********************************************************************************/
    static bool load_agent_data(const std::string & path,
        std::vector<float> & points, std::vector<int> & ids, bool binary = false);
    static bool save_agent_data(const std::string & path,
        const std::vector<float> & points, const std::vector<int> & ids, bool binary = false);

    /********************************************************************************
    ** ��һ����agent���ݽ�����Ԥ������������agent�Ĺ켣��Ϣ
    *********************************************************************************/
    static bool load_agent_path(const std::string & path,
        std::vector<std::vector<float>> & agents, bool binary = false);
    static bool save_agent_path(const std::string & path,
        const std::vector<std::vector<float>> & agents, bool binary = false);


    /*******************************************************************************
    ** ͨ�õ���Ⱦ���ݵĵ��룬��Ϊ����������
    *******************************************************************************/
    static bool save_point_data(const std::string & path,
        const std::vector<float> & point, bool binary = false);
    static bool save_line_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);
    static bool save_triangle_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);

    
    /*******************************************************************************
    ** ����2ά��ͼ��Ϣ
    *******************************************************************************/
    static bool load_city_map(const std::string & path, std::vector<std::vector<char>> & data);


    /*******************************************************************************
    ** �������2ά��ͼ��Ϣ���Ƶ�ͼƬ�У����ڹ۲�
    *******************************************************************************/
    static bool save_city_map_data(const std::string & path,
        const std::vector<std::vector<char>> & city_map, std::vector<std::vector<float>> & data);
    static bool save_city_map_data(const std::string & path, const std::vector<std::vector<char>> & city_map,
        std::vector<std::vector<float>> & data, const float min_value, const float max_value);

    static bool save_city_map_shelter_influence(const std::string & path, const int shelter_number,
        const std::vector<std::vector<char>> & city_map, const std::vector<std::vector<char>> & data);

    static bool save_city_space_clearance(const std::string & path,
        const std::vector<std::vector<char>> & city_map,const std::vector<std::vector<int>> & data);


    /*******************************************************************************
    ** �������2ά��ͼ��Ϣ�����������´�ʹ��ʱֱ�Ӷ�ȡ�������ٽ��м���
    *******************************************************************************/
    static bool load_city_grid_data(const std::string & path,
        std::vector<std::vector<char>> & city, bool binary = false);
    static bool load_city_grid_data(const std::string & path,
        std::vector<std::vector<int>> & city, bool binary = false);
    static bool load_city_grid_data(const std::string & path,
        std::vector<std::vector<float>> & city, bool binary = false);
    static bool save_city_grid_data(const std::string & path,
        const std::vector<std::vector<char>> & city, bool binary = false);
    static bool save_city_grid_data(const std::string & path,
        const std::vector<std::vector<int>> & city, bool binary = false);
    static bool save_city_grid_data(const std::string & path,
        const std::vector<std::vector<float>> & city, bool binary = false);


    /*******************************************************************************
    ** ������ڵ�·���������ݣ������ڵ�͵�Ԫ����
    *******************************************************************************/
    static bool load_road_attribute_data(const std::string & path,
        std::vector<float> & data, bool binary = false);
    static bool save_road_attribute_data(const std::string & path,
        const std::vector<float> & data, bool binary = false);




};