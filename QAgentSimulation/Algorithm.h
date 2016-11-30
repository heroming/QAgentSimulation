#pragma once

class Algorithm
{

public:

    // 根据建筑、河流和安全区的信息构建了城市的地图。
    static void build_city_grid_map();

    // 读取10个随时间步递进的vtk格式的Damage数据，构建出城市的可行走区域。
    static void build_city_damage_map();

};

