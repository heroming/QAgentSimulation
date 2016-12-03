#pragma once

class Algorithm
{

public:

    // 根据建筑、河流和安全区的信息构建了城市的地图。
    static void build_city_grid_map();

    // 读取10个随时间步递进的vtk格式的Damage数据，构建出城市的可行走区域。
    static void build_city_damage_map();

    // 读取所有的道路使用数据，标注出城市的主要道路
    static void find_the_main_road_by_data_road_information();


    /*******************************************************************************
    ** 根据所有agent逃生的移动情况，标注城市的主要道路
    *******************************************************************************/
    static void find_the_main_road_by_agent_movement();
     
    // 获取所有agent的移动轨迹点数据，用于预处理
    static void get_agent_movement(std::vector<std::vector<float>> & agent);

};

