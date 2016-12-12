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

    // 根据agent的移动轨迹，计算移动过程中经过路线的热力图
    static void get_agent_heat_map(const std::vector<std::vector<char>> & city_map,
        std::vector<std::vector<float>> & heat_map);

    // 根据热力图计算主道路中的节点信息
    static void get_main_road_node(const std::vector<std::vector<float>> & heat_map,
        std::vector<std::vector<float>> & road_node);

    // 从原始路网中过滤agent使用频繁的作为主要道路。
    static void filter_main_road_by_heat_map(const std::vector<std::vector<float>> & heat_map);

    // 获取以点(x, y)为中心，以w为半径的agent位移的热力图信息，mode=0表示最大值，mode=1表示累积值
    static float get_neibor_heat(const int x, const int y, const int w, 
        const std::vector<std::vector<float>> & heat_map, const int mode = 0);

    /*******************************************************************************
    ** 道路信息统计，路网构建 
    ** !!! 比较麻烦，主要道路与shelter的连接存在很大困难，不如直接从agent移动入手 !!!
    ** 1. 根据agent逃生的热力图信息，统计原始路网中的道路长度和权重信息
    ** 2. 根据1中的信息，基于原始的道路网络，构建主道路网络
    ** 3. 根据1中的道路长度和权重信息(结合heatmap得到的主道路信息)，重新筛选主要道路
    *******************************************************************************/
    static void calculate_road_statistic_information();
    static void filter_main_road_by_road_weight_and_heat();
    static void connect_main_road_network();


    /******************************************************************************
    ** 直接从agent的移动出发，映射到道路上，通过道路信息再来挑选主要道路，这样主要道路与
    ** shelter之间直接就成为连接的状态，不用刻意进行connect
    *******************************************************************************/
    static void map_city_grid_and_road();
    static void calculate_road_weight();
    static void filter_main_road_by_road_weight();


};

