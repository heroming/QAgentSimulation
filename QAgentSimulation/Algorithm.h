#pragma once

class Algorithm
{

public:

    // ���ݽ����������Ͱ�ȫ������Ϣ�����˳��еĵ�ͼ��
    static void build_city_grid_map();

    // ��ȡ10����ʱ�䲽�ݽ���vtk��ʽ��Damage���ݣ����������еĿ���������
    static void build_city_damage_map();

    // ��ȡ���еĵ�·ʹ�����ݣ���ע�����е���Ҫ��·
    static void find_the_main_road_by_data_road_information();


    /*******************************************************************************
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
    ** ��·��Ϣͳ�ƣ�·������ 
    ** !!! �Ƚ��鷳����Ҫ��·��shelter�����Ӵ��ںܴ����ѣ�����ֱ�Ӵ�agent�ƶ����� !!!
    ** 1. ����agent����������ͼ��Ϣ��ͳ��ԭʼ·���еĵ�·���Ⱥ�Ȩ����Ϣ
    ** 2. ����1�е���Ϣ������ԭʼ�ĵ�·���磬��������·����
    ** 3. ����1�еĵ�·���Ⱥ�Ȩ����Ϣ(���heatmap�õ�������·��Ϣ)������ɸѡ��Ҫ��·
    *******************************************************************************/
    static void calculate_road_statistic_information();
    static void filter_main_road_by_road_weight_and_heat();
    static void connect_main_road_network();


    /******************************************************************************
    ** ֱ�Ӵ�agent���ƶ�������ӳ�䵽��·�ϣ�ͨ����·��Ϣ������ѡ��Ҫ��·��������Ҫ��·��
    ** shelter֮��ֱ�Ӿͳ�Ϊ���ӵ�״̬�����ÿ������connect
    *******************************************************************************/
    static void map_city_grid_and_road();
    static void calculate_road_weight();
    static void filter_main_road_by_road_weight();


};

