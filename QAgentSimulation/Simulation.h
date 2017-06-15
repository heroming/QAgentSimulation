#pragma once

#include <map>
#include <set>
#include <vector>
#include <cmath>
#include <ctime>
#include <string>
#include <algorithm>

#include <QThread>

const float INF = 1e12;
const int INFI = 0x7FFFFFFF;
const int NOTHING = 1e9 + 1;
const int RUNNING = 1e9 + 2;
const int ARRIVED = 1e9 + 3;
const int BLOCKED = 1e9 + 4;
const int DEAD = 1e9 + 5;
const int ARRIVING = 1e9 + 6;

const int BIT = (1 << 16) - 1;
const float SQRT_TWO = 1.4142135623731;
const int DX[9] = {-1, 0, 1, 0, -1, -1, 1, 1, 0};
const int DY[9] = {0, 1, 0, -1, -1, 1, 1, -1, 0};
const float DIS[9] = {1, 1, 1, 1, SQRT_TWO, SQRT_TWO, SQRT_TWO, SQRT_TWO, 0};


class Point
{
public :
    float x, y;
    Point(): x(0), y(0) {}
    Point(float a, float b): x(a), y(b) {}

    Point operator + (const Point & po) const;

    Point operator - (const Point & po) const;

    Point operator * (const float k) const;

    float operator * (const Point & po) const;

    float operator ^ (const Point & po) const;

    float manhattan_distance(const Point & po) const;

    float manhattan_length() const;

    float distance(const int a, const int b) const;

    float distance(const float a, const float b) const;

    float distance(const Point & po) const;

    float length() const;

    Point normalize() const;
};

/********************************************************************
**************************** 道路顶点 ********************************
********************************************************************/
class Vertex
{
public :
    int id;                     // 顶点id
    Point p;                    // 顶点坐标
    
    float manhattan_distance(const Vertex & u) const;

    float manhattan_distance(const Point & u) const;

    float distance(const Vertex & u) const;

    float distance(const Point & u) const;

    static float distance(const Vertex & u, const Vertex & v);
};

/********************************************************************
**************************** 道路边 **********************************
********************************************************************/
class Edge
{
public :
    int id;                     // 边的id
    int u, v;                   // 边上两个节点的id
    float dis;                  // 边的距离权重
    float width;                // 边的宽度权重
    float density;              // 边的密度权重
    float damage;               // 灾害对边的权重影响
    float learn;                // 学习参数
    float weight;               // 道路权重(长距离，全地图)
    float weight_view;          // 道路权重(短距离，包含了agent看到的damage和density)

    bool is_main;               // 是否为主干道路
    bool is_good;               // 道路是否损坏

    static const float DAMAGE_THREHOLD;

    Edge();

    static float get_weight_width(const float x);

    static float get_weight_density(const float x);

    static float get_weight_main_road(const bool b);

    static float get_weight_damage(const float w);

    float get_weight() const;

    float get_weight_view() const;
};

/********************************************************************
**************************** 逃生人员 ********************************
********************************************************************/
class Agent
{
public :

    Point p;                        // 当前位置
    Point np;                       // 下一个位置
    Point cp;                       // 控制点位置
    Point v;                        // 速度方向

    Point op;                       // 初始位置坐标
    float max_velocity;             // 最大速度
    int target_shelter;             // 目标避难所
    int edge_on;                    // 当前所在边
    int status;                     // 逃生中的状态

    int path_index;                 // path_view的指针
    std::vector<int> path_view;     // 记录视线范围之内的将要移动的路径

    Agent();

    bool finish() const;

    float distance(const Agent & a) const;

    float distance(const Point & a) const;

    float distance(const Vertex & a) const;

    float manhattan_distance(const Point & a) const;

    float manhattan_distance(const Agent & a) const;
};

/********************************************************************
**************************** 安全区域 ********************************
********************************************************************/
class Shelter
{
public :
    int area;                       // 安全区面积
    int count;                      // 安全区现在人数
    int capacity;                   // 安全区容量
    float influence;                // 安全区影响范围
    std::vector<int> ids;
    std::vector<int> main_ids;

    static const float CAPACITY_RATE;
    static const float INFLUENCE_RATE;

    void initialize();
};

/********************************************************************
***************************** 采样点 *********************************
********************************************************************/
class Sample
{
public :
    static const int N = 13;

    // 城市规格参数
    static const int MIN_X = 558;
    static const int MAX_X = 10158;
    static const int MIN_Y = 59180;
    static const int MAX_Y = 64580;

    static const int WIDTH = MAX_X - MIN_X;
    static const int HEIGHT = MAX_Y - MIN_Y;

    Point po[N];

    float dis[N];                       // 到控制点或安全区距离
    float weight[N];                    // 只考虑dis、obstacle、damame
    float weight_all[N];                // 采样点整体权重值
    static const Point PO[N];

    Sample();

    // 初始化采样点
    void get_sample(const float w, const Point & ve, const Point & p);
    
    void get_distance(const std::vector<std::vector<float>> & d);

    void get_distance(const float d[WIDTH][HEIGHT]);

    void get_distance(const Point & p);

    // 初始化dis、obstacle以及damage的权重
    void get_weight(const Point & p, const std::vector<std::vector<float>> & cw, const std::vector<std::vector<float>> & cd);

    void clear_weight_agent();

    void add_agent_weight(const Agent & a);

    Point get_min_weight_point() const;

    static float distance_weight(const float d);

    static float agent_weight(const float d);

    static float obstacle_weight(const float d);

    static float damage_weight(const float d);

    // 考虑实际距离河流和建筑的距离
    static float damage_weight(const float d, const float e);
};

/********************************************************************
***************************** 采样点 *********************************
********************************************************************/
class Result
{
public :
    Point p;            // 当前位置
    int status;         // 状态

    Result();
    Result(const Point & a);
    Result(const Point & a, const int s);
};

class Simulation : public QThread
{
    Q_OBJECT

protected :
    void run();

public :

    // 城市规格参数
    static const int MIN_X = 558;
    static const int MAX_X = 10158;
    static const int MIN_Y = 59180;
    static const int MAX_Y = 64580;

    static const int WIDTH = MAX_X - MIN_X;
    static const int HEIGHT = MAX_Y - MIN_Y;

    // 运行时间规格
    static const int UPDATE_DAMAGE = 1500;
    static const int UPDATE_SHELTER_DISTANCE = 300;
    static const int UPDATE_PATH_PLANNING = 10;

    static const float ITER_THREHOLD;
    static const float CHANGE_SHELTER_THREHOLD;
    static const float CHANGE_SHELTER_DISTANCE;

private :

    bool m_step_debug;
    bool m_step_pause;
    bool m_initialize;
    bool m_finish;

    int m_index_main;
    int m_vertex_main;
    int m_index_connect;
    int m_vertex_connect;


    // city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    std::vector<std::vector<char>> m_grid;
    std::vector<std::vector<float>> m_clearance;

    // city damage数据(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
    std::vector<std::vector<char>> m_grid_damage;
    std::vector<std::vector<float>> m_clearance_damage;

    std::vector<std::vector<float>> m_clearance_shelter;

    // 主要道路和整体道路的点信息
    std::vector<Vertex> m_point;

    // 以Graph表示的主要道路和整体道路信息
    std::vector<Edge> m_edge;
    std::vector<std::vector<int>> m_graph;

    // 以Grid表示道路顶点信息到Vertex信息的映射
    std::vector<std::vector<int>> m_vertex_map;

    // 所有安全区域
    std::vector<Shelter> m_shelter;

    // 所有逃生人员
    std::vector<Agent> m_agent;
    std::vector<int> m_agent_run;

    // 记录初始安全区
    std::vector<int> m_init_shelter;

    static const int GRID_STRIP = 40;
    static const int GRID_WIDTH = (WIDTH + GRID_STRIP - 1) / GRID_STRIP;
    static const int GRID_HEIGHT = (HEIGHT + GRID_STRIP - 1) / GRID_STRIP;

    // 城市人员所在网格，用来找近邻与计算密度
    static const int NEIBOR_DISTANCE = 20;
    std::vector<int> m_agent_grid[GRID_WIDTH][GRID_HEIGHT];

    // 城市人员密度网格化
    std::vector<int> m_density[GRID_WIDTH][GRID_HEIGHT];

    static const int SEEING_DISTANCE = 50;
    static const float SEEING_DISTANCE_DIFFERENCE;
    static const float MIN_CONTROL_DISTANCE;

    static const int SHELTER_NUMBER = 50;

    // 对于每个安全区，记录从influence影响范围之内的每个顶点到shelter的最短路
    std::vector<float> m_dis_to_shelter[SHELTER_NUMBER];
    std::vector<int> m_next_edge_to_shelter[SHELTER_NUMBER];


    // 局部位移数据
    std::vector<Sample> m_sample;
    std::vector<std::vector<int>> m_neibor;

    /**********************  学习数据  ***************************/
    // TODO 学习初始安全区
    std::vector<int> m_learn_damage[GRID_WIDTH][GRID_HEIGHT];
    std::vector<int> m_learn_density[GRID_WIDTH][GRID_HEIGHT];

    /**********************  结果数据  ***************************/
    int m_timestamp;
    std::vector<float> m_one_result;
    std::vector<std::vector<Result>> m_result;


public :
    int get_timestamp() const;
    const std::vector<float> & get_timestamp_data(const int k); 


public :

    Simulation();

    bool is_initialized() const;
    bool is_finished() const;

    void finish();

    void initialize();
    void initialize_road();
    void initialize_main_road();
    void initialize_connect();
    void initialize_shelter();
    void initialize_agent();
    void initialize_road_information();
    void initialize_vertex_map();

    void learning();
    void simulation();
    void global_path_planning();
    void local_movement();

    
    // 按照时间步导入damage，反映到边权重上
    void update_damage(const int k);

    // 由于shelter影响范围以及灾害等因素影响，可能目标shelter不可达，需要更换
    void update_agent_target_shelter();

    // 根据density和damage更新道路权重
    void update_road_weight();

    // 更新m_agent_run列表
    void update_running_agent();

    // 收集结果数据
    void collect_agent_trajectory();

    // 针对初始agent还没有确定沿着哪条道路走时，找一个最近的道路顶点前进
    void find_nearest_road();

    // 对于每个安全区，重新计算其影响范围内所有道路到安全区的距离
    void update_shelter_distance();

    // update_shelter_distance调用，spfa寻找最短路
    void shelter_shortest_path(const int shelter_id);

    static bool valid(const Point & p);
    static bool valid(const int x, const int y);
    static bool valid(const float x, const float y);
    static int compare(const float x);


    /****************** For Debug ********************/
    // 显示某点到达安全区的路径
    void get_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
        std::vector<float> & l_weight, const float l_x, const float l_y);
    
    void get_agent_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
        std::vector<float> & l_weight, const float l_x, const float l_y);

    void get_agent_energy(Agent & l_agent, Sample & l_sample, const float l_x, const float l_y);

    void set_run_step(const bool flag);

    void next_step();
};