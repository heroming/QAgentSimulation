#pragma once

#include <map>
#include <set>
#include <queue>
#include <vector>
#include <algorithm>
#include <functional>

class Algorithm
{

public:

    // 根据建筑、河流和安全区的信息构建了城市的地图。
    // 读取city,river,shelter信息，构建出png地图，并分别保存grid的txt与dat数据
    static void build_city_grid_map();

    // 读取10个随时间步递进的vtk格式的Damage数据，构建出城市的可行走区域。
    // 读取MAS DATA，提取0-9共十个damage数据，输出png地图和txt、dat数据。
    static void build_city_damage_map();

    /*=============================================================================
      =============================================================================
      =============================  构建主要道路  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** 1.1 : 通过道路流量信息
    ** 读取MAS DATA第0趟数据，累积所有的道路使用数据(num_agents_passed)，通过阈值挑选标注出城市的主要道路
    *******************************************************************************/
    static void find_the_main_road_by_data_road_information();


    /*******************************************************************************
    ** 1.2 : 通过各个时间步的人员移动信息构建heat map，在计算道路的heat值，用于过滤主道路
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
    ** 1.3 : 道路信息统计，路网构建 
    ** !!! 比较麻烦，主要道路与shelter的连接存在很大困难，不如直接从agent移动入手 !!!
    ** a. 根据agent逃生的热力图信息，统计原始路网中的道路长度和权重信息
    ** b. 根据a中的信息，基于原始的道路网络，构建主道路网络
    ** c. 根据a中的道路长度和权重信息(结合heatmap得到的主道路信息)，重新筛选主要道路，未完成
    *******************************************************************************/
    static void calculate_road_statistic_information();
    static void filter_main_road_by_road_weight_and_heat();
    static void connect_main_road_network();


    /******************************************************************************
    ** 1.4 : 直接计算道路的宽度，用于过滤主道路
    ** 直接从agent的移动出发，映射到道路上，通过道路信息再来挑选主要道路，这样主要道路与
    ** shelter之间直接就成为连接的状态，不用刻意进行connect(结果还是有很多不连续的问题)
    *******************************************************************************/
    static void map_city_grid_and_road();
    static void calculate_road_weight();
    static void filter_main_road_by_road_weight();


    /******************************************************************************
    ** 1.5 : 主要根据地图的网格信息，根据路网的物理形态(据周围建筑、河流距离)来生成主要道路，并通过联通分量和点收缩来优化路网。
    ** a. 统计每个道路端点的可行走半径。
    *******************************************************************************/
    static void map_road_end_point_to_city_map_with_clearance();

    static bool is_valid_position(const std::vector<std::vector<char>> & grid,
        const int x, const int y, const bool actual = false);
    static bool is_valid_edge(const std::vector<std::vector<char>> & grid,
        const int a, const int b, const int c, const int d);
    static int get_point_clearance(const std::vector<std::vector<char>> & grid,
        const int x, const int y, const bool actual, const bool river); 
    static bool check_for_clearance(const std::vector<std::vector<char>> & grid,
        const int x, const int y, const int r, const bool actrual, const bool river);

    static int get_connect_component_dfs(const std::vector<std::vector<int>> & city_graph, std::vector<int> & weight, const int u);
    static void set_connect_component_dfs(const std::vector<std::vector<int>> & city_graph, std::vector<int> & weight, const int u, const int w);

    /******************************************************************************
    ** 1.6 : 通过交互方法指定主要道路，详见CreatRoad类
    *******************************************************************************/


    /*=============================================================================
      =============================================================================
      =============================  灾变仿真模拟  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** 2.1 : 计算每个shelter(安全区域)的容纳人数以及每个agent应该前往哪个安全区域
    *******************************************************************************/
    static void calculate_shelter_influence();
    static void calculate_shelter_influence_bfs(const std::vector<std::vector<char>> & grid,
        std::vector<std::vector<char>> & id_map, const int x, const int y, const int id);

    /******************************************************************************
    ** 2.2 : 计算每个空白区域的clearance(即最近障碍距离)，用以表示区域的宽敞程度
    *******************************************************************************/
    static void calculate_space_clearance();
    static void calculate_damage_clearance();
    static void calculate_shelter_clearance();

    /******************************************************************************
    ** 2.3 : 将主要道路与整体道路相关联
    *******************************************************************************/
    static void connect_main_road_and_detailed_road();

    /******************************************************************************
    ** 2.4 : 一些预处理操作
    ***  a : 计算所有安全区域的信息并存储
    ***  b : 计算道路的长度和宽度
    ***  c : 整理其他趟的agent轨迹数据(为了测试agent初始化位置是否一致：比较随机)
    *******************************************************************************/
    static void pre_calculate_shelter_infomation();
    static void pre_calculate_road_distance_and_width();
    static void pre_calculate_road_distance_and_width(const std::vector<std::vector<char>> & grid,
        const std::vector<std::vector<int>> & clearance, const std::string & path_in, const std::string & path_out);
    static void pre_get_agent_timestep_data();

    // d : 查看原数据中逃生者逃到安全区的容纳范围
    static void pre_calculate_orignal_shelter_influence();
    static void pre_calculate_my_shelter_influence();

    /*=============================================================================
      =============================================================================
      =============================  其他功能函数  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** 添加一条道路
    *******************************************************************************/
    static void add_detailed_road();
    static void get_agent_subset();

    static void get_init_agent();
    static void get_agent_information();


};

