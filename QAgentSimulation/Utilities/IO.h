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
    ** 通用函数，用于导入GLSL Shader以及需要渲染的城市数据
    *******************************************************************************/
    static bool load_shader(const std::string & path, std::vector<char> & data);
    static bool load_point_data(const std::string & path, std::vector<float> & point);
    static bool load_line_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_triangle_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);


    /*******************************************************************************
    ** 将5400*9600的地图保存到文件中，其中-1是避难所，0是可通行区域，1是障碍物
    *******************************************************************************/
    static bool load_damage(const std::string & path, std::vector<int> & x_coordinate,
        std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data);
    static bool save_damage_data(const std::string & path,
        const std::vector<std::vector<char>> & data, bool binary = false);


    /********************************************************************************
    ** 道路信息处理，分别获取道路的节点和连接信息，以及各条道路的属性信息。
    *********************************************************************************/
    static bool load_graph_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_graph_attribute(const std::string & path,
        const std::string & name, std::vector<float> & data);


    /********************************************************************************
    ** 对agent数据进行了预处理，只保留了agent的位置和id信息
    *********************************************************************************/
    static bool load_agent_data(const std::string & path,
        std::vector<float> & points, std::vector<int> & ids, bool binary = false);
    static bool save_agent_data(const std::string & path,
        const std::vector<float> & points, const std::vector<int> & ids, bool binary = false);

    /********************************************************************************
    ** 进一步对agent数据进行了预处理，保留所有agent的轨迹信息
    *********************************************************************************/
    static bool load_agent_path(const std::string & path,
        std::vector<std::vector<float>> & agents, bool binary = false);
    static bool save_agent_path(const std::string & path,
        const std::vector<std::vector<float>> & agents, bool binary = false);


    /*******************************************************************************
    ** 通用的渲染数据的导入，分为点线面三种
    *******************************************************************************/
    static bool save_point_data(const std::string & path,
        const std::vector<float> & point, bool binary = false);
    static bool save_line_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);
    static bool save_triangle_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);

    
    /*******************************************************************************
    ** 导入2维地图信息
    *******************************************************************************/
    static bool load_city_map(const std::string & path, std::vector<std::vector<char>> & data);


    /*******************************************************************************
    ** 将计算的2维地图信息绘制到图片中，便于观察
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
    ** 将计算的2维地图信息保存起来，下次使用时直接读取，不用再进行计算
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
    ** 保存关于道路的属性数据，包括节点和单元数据
    *******************************************************************************/
    static bool load_road_attribute_data(const std::string & path,
        std::vector<float> & data, bool binary = false);
    static bool save_road_attribute_data(const std::string & path,
        const std::vector<float> & data, bool binary = false);




};