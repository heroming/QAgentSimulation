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

    // ���ݽ����������Ͱ�ȫ������Ϣ�����˳��еĵ�ͼ��
    // ��ȡcity,river,shelter��Ϣ��������png��ͼ�����ֱ𱣴�grid��txt��dat����
    static void build_city_grid_map();

    // ��ȡ10����ʱ�䲽�ݽ���vtk��ʽ��Damage���ݣ����������еĿ���������
    // ��ȡMAS DATA����ȡ0-9��ʮ��damage���ݣ����png��ͼ��txt��dat���ݡ�
    static void build_city_damage_map();

    /*=============================================================================
      =============================================================================
      =============================  ������Ҫ��·  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** 1.1 : ͨ����·������Ϣ
    ** ��ȡMAS DATA��0�����ݣ��ۻ����еĵ�·ʹ������(num_agents_passed)��ͨ����ֵ��ѡ��ע�����е���Ҫ��·
    *******************************************************************************/
    static void find_the_main_road_by_data_road_information();


    /*******************************************************************************
    ** 1.2 : ͨ������ʱ�䲽����Ա�ƶ���Ϣ����heat map���ڼ����·��heatֵ�����ڹ�������·
    ** ��������agent�������ƶ��������ע���е���Ҫ��·
    *******************************************************************************/
    static void find_the_main_road_by_agent_movement();
     
    // ��ȡ����agent���ƶ��켣�����ݣ�����Ԥ����
    static void get_agent_movement(std::vector<std::vector<float>> & agent);

    // ����agent���ƶ��켣�������ƶ������о���·�ߵ�����ͼ
    static void get_agent_heat_map(const std::vector<std::vector<char>> & city_map,
        std::vector<std::vector<float>> & heat_map);

    // ��������ͼ��������·�еĽڵ���Ϣ
    static void get_main_road_node(const std::vector<std::vector<float>> & heat_map,
        std::vector<std::vector<float>> & road_node);

    // ��ԭʼ·���й���agentʹ��Ƶ������Ϊ��Ҫ��·��
    static void filter_main_road_by_heat_map(const std::vector<std::vector<float>> & heat_map);

    // ��ȡ�Ե�(x, y)Ϊ���ģ���wΪ�뾶��agentλ�Ƶ�����ͼ��Ϣ��mode=0��ʾ���ֵ��mode=1��ʾ�ۻ�ֵ
    static float get_neibor_heat(const int x, const int y, const int w, 
        const std::vector<std::vector<float>> & heat_map, const int mode = 0);

    /*******************************************************************************
    ** 1.3 : ��·��Ϣͳ�ƣ�·������ 
    ** !!! �Ƚ��鷳����Ҫ��·��shelter�����Ӵ��ںܴ����ѣ�����ֱ�Ӵ�agent�ƶ����� !!!
    ** a. ����agent����������ͼ��Ϣ��ͳ��ԭʼ·���еĵ�·���Ⱥ�Ȩ����Ϣ
    ** b. ����a�е���Ϣ������ԭʼ�ĵ�·���磬��������·����
    ** c. ����a�еĵ�·���Ⱥ�Ȩ����Ϣ(���heatmap�õ�������·��Ϣ)������ɸѡ��Ҫ��·��δ���
    *******************************************************************************/
    static void calculate_road_statistic_information();
    static void filter_main_road_by_road_weight_and_heat();
    static void connect_main_road_network();


    /******************************************************************************
    ** 1.4 : ֱ�Ӽ����·�Ŀ�ȣ����ڹ�������·
    ** ֱ�Ӵ�agent���ƶ�������ӳ�䵽��·�ϣ�ͨ����·��Ϣ������ѡ��Ҫ��·��������Ҫ��·��
    ** shelter֮��ֱ�Ӿͳ�Ϊ���ӵ�״̬�����ÿ������connect(��������кܶ಻����������)
    *******************************************************************************/
    static void map_city_grid_and_road();
    static void calculate_road_weight();
    static void filter_main_road_by_road_weight();


    /******************************************************************************
    ** 1.5 : ��Ҫ���ݵ�ͼ��������Ϣ������·����������̬(����Χ��������������)��������Ҫ��·����ͨ����ͨ�����͵��������Ż�·����
    ** a. ͳ��ÿ����·�˵�Ŀ����߰뾶��
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
    ** 1.6 : ͨ����������ָ����Ҫ��·�����CreatRoad��
    *******************************************************************************/


    /*=============================================================================
      =============================================================================
      =============================  �ֱ����ģ��  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** 2.1 : ����ÿ��shelter(��ȫ����)�����������Լ�ÿ��agentӦ��ǰ���ĸ���ȫ����
    *******************************************************************************/
    static void calculate_shelter_influence();
    static void calculate_shelter_influence_bfs(const std::vector<std::vector<char>> & grid,
        std::vector<std::vector<char>> & id_map, const int x, const int y, const int id);

    /******************************************************************************
    ** 2.2 : ����ÿ���հ������clearance(������ϰ�����)�����Ա�ʾ����Ŀ��̶�
    *******************************************************************************/
    static void calculate_space_clearance();
    static void calculate_damage_clearance();
    static void calculate_shelter_clearance();

    /******************************************************************************
    ** 2.3 : ����Ҫ��·�������·�����
    *******************************************************************************/
    static void connect_main_road_and_detailed_road();

    /******************************************************************************
    ** 2.4 : һЩԤ�������
    ***  a : �������а�ȫ�������Ϣ���洢
    ***  b : �����·�ĳ��ȺͿ��
    ***  c : ���������˵�agent�켣����(Ϊ�˲���agent��ʼ��λ���Ƿ�һ�£��Ƚ����)
    *******************************************************************************/
    static void pre_calculate_shelter_infomation();
    static void pre_calculate_road_distance_and_width();
    static void pre_calculate_road_distance_and_width(const std::vector<std::vector<char>> & grid,
        const std::vector<std::vector<int>> & clearance, const std::string & path_in, const std::string & path_out);
    static void pre_get_agent_timestep_data();

    // d : �鿴ԭ�������������ӵ���ȫ�������ɷ�Χ
    static void pre_calculate_orignal_shelter_influence();
    static void pre_calculate_my_shelter_influence();

    /*=============================================================================
      =============================================================================
      =============================  �������ܺ���  ==================================
      =============================================================================
      ============================================================================*/

    /******************************************************************************
    ** ���һ����·
    *******************************************************************************/
    static void add_detailed_road();
    static void get_agent_subset();

    static void get_init_agent();
    static void get_agent_information();


};

