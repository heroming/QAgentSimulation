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

};

