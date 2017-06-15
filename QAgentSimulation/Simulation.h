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
**************************** ��·���� ********************************
********************************************************************/
class Vertex
{
public :
    int id;                     // ����id
    Point p;                    // ��������
    
    float manhattan_distance(const Vertex & u) const;

    float manhattan_distance(const Point & u) const;

    float distance(const Vertex & u) const;

    float distance(const Point & u) const;

    static float distance(const Vertex & u, const Vertex & v);
};

/********************************************************************
**************************** ��·�� **********************************
********************************************************************/
class Edge
{
public :
    int id;                     // �ߵ�id
    int u, v;                   // ���������ڵ��id
    float dis;                  // �ߵľ���Ȩ��
    float width;                // �ߵĿ��Ȩ��
    float density;              // �ߵ��ܶ�Ȩ��
    float damage;               // �ֺ��Աߵ�Ȩ��Ӱ��
    float learn;                // ѧϰ����
    float weight;               // ��·Ȩ��(�����룬ȫ��ͼ)
    float weight_view;          // ��·Ȩ��(�̾��룬������agent������damage��density)

    bool is_main;               // �Ƿ�Ϊ���ɵ�·
    bool is_good;               // ��·�Ƿ���

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
**************************** ������Ա ********************************
********************************************************************/
class Agent
{
public :

    Point p;                        // ��ǰλ��
    Point np;                       // ��һ��λ��
    Point cp;                       // ���Ƶ�λ��
    Point v;                        // �ٶȷ���

    Point op;                       // ��ʼλ������
    float max_velocity;             // ����ٶ�
    int target_shelter;             // Ŀ�������
    int edge_on;                    // ��ǰ���ڱ�
    int status;                     // �����е�״̬

    int path_index;                 // path_view��ָ��
    std::vector<int> path_view;     // ��¼���߷�Χ֮�ڵĽ�Ҫ�ƶ���·��

    Agent();

    bool finish() const;

    float distance(const Agent & a) const;

    float distance(const Point & a) const;

    float distance(const Vertex & a) const;

    float manhattan_distance(const Point & a) const;

    float manhattan_distance(const Agent & a) const;
};

/********************************************************************
**************************** ��ȫ���� ********************************
********************************************************************/
class Shelter
{
public :
    int area;                       // ��ȫ�����
    int count;                      // ��ȫ����������
    int capacity;                   // ��ȫ������
    float influence;                // ��ȫ��Ӱ�췶Χ
    std::vector<int> ids;
    std::vector<int> main_ids;

    static const float CAPACITY_RATE;
    static const float INFLUENCE_RATE;

    void initialize();
};

/********************************************************************
***************************** ������ *********************************
********************************************************************/
class Sample
{
public :
    static const int N = 13;

    // ���й�����
    static const int MIN_X = 558;
    static const int MAX_X = 10158;
    static const int MIN_Y = 59180;
    static const int MAX_Y = 64580;

    static const int WIDTH = MAX_X - MIN_X;
    static const int HEIGHT = MAX_Y - MIN_Y;

    Point po[N];

    float dis[N];                       // �����Ƶ��ȫ������
    float weight[N];                    // ֻ����dis��obstacle��damame
    float weight_all[N];                // ����������Ȩ��ֵ
    static const Point PO[N];

    Sample();

    // ��ʼ��������
    void get_sample(const float w, const Point & ve, const Point & p);
    
    void get_distance(const std::vector<std::vector<float>> & d);

    void get_distance(const float d[WIDTH][HEIGHT]);

    void get_distance(const Point & p);

    // ��ʼ��dis��obstacle�Լ�damage��Ȩ��
    void get_weight(const Point & p, const std::vector<std::vector<float>> & cw, const std::vector<std::vector<float>> & cd);

    void clear_weight_agent();

    void add_agent_weight(const Agent & a);

    Point get_min_weight_point() const;

    static float distance_weight(const float d);

    static float agent_weight(const float d);

    static float obstacle_weight(const float d);

    static float damage_weight(const float d);

    // ����ʵ�ʾ�������ͽ����ľ���
    static float damage_weight(const float d, const float e);
};

/********************************************************************
***************************** ������ *********************************
********************************************************************/
class Result
{
public :
    Point p;            // ��ǰλ��
    int status;         // ״̬

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

    // ���й�����
    static const int MIN_X = 558;
    static const int MAX_X = 10158;
    static const int MIN_Y = 59180;
    static const int MAX_Y = 64580;

    static const int WIDTH = MAX_X - MIN_X;
    static const int HEIGHT = MAX_Y - MIN_Y;

    // ����ʱ����
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


    // city map����(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    std::vector<std::vector<char>> m_grid;
    std::vector<std::vector<float>> m_clearance;

    // city damage����(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
    std::vector<std::vector<char>> m_grid_damage;
    std::vector<std::vector<float>> m_clearance_damage;

    std::vector<std::vector<float>> m_clearance_shelter;

    // ��Ҫ��·�������·�ĵ���Ϣ
    std::vector<Vertex> m_point;

    // ��Graph��ʾ����Ҫ��·�������·��Ϣ
    std::vector<Edge> m_edge;
    std::vector<std::vector<int>> m_graph;

    // ��Grid��ʾ��·������Ϣ��Vertex��Ϣ��ӳ��
    std::vector<std::vector<int>> m_vertex_map;

    // ���а�ȫ����
    std::vector<Shelter> m_shelter;

    // ����������Ա
    std::vector<Agent> m_agent;
    std::vector<int> m_agent_run;

    // ��¼��ʼ��ȫ��
    std::vector<int> m_init_shelter;

    static const int GRID_STRIP = 40;
    static const int GRID_WIDTH = (WIDTH + GRID_STRIP - 1) / GRID_STRIP;
    static const int GRID_HEIGHT = (HEIGHT + GRID_STRIP - 1) / GRID_STRIP;

    // ������Ա�������������ҽ���������ܶ�
    static const int NEIBOR_DISTANCE = 20;
    std::vector<int> m_agent_grid[GRID_WIDTH][GRID_HEIGHT];

    // ������Ա�ܶ�����
    std::vector<int> m_density[GRID_WIDTH][GRID_HEIGHT];

    static const int SEEING_DISTANCE = 50;
    static const float SEEING_DISTANCE_DIFFERENCE;
    static const float MIN_CONTROL_DISTANCE;

    static const int SHELTER_NUMBER = 50;

    // ����ÿ����ȫ������¼��influenceӰ�췶Χ֮�ڵ�ÿ�����㵽shelter�����·
    std::vector<float> m_dis_to_shelter[SHELTER_NUMBER];
    std::vector<int> m_next_edge_to_shelter[SHELTER_NUMBER];


    // �ֲ�λ������
    std::vector<Sample> m_sample;
    std::vector<std::vector<int>> m_neibor;

    /**********************  ѧϰ����  ***************************/
    // TODO ѧϰ��ʼ��ȫ��
    std::vector<int> m_learn_damage[GRID_WIDTH][GRID_HEIGHT];
    std::vector<int> m_learn_density[GRID_WIDTH][GRID_HEIGHT];

    /**********************  �������  ***************************/
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

    
    // ����ʱ�䲽����damage����ӳ����Ȩ����
    void update_damage(const int k);

    // ����shelterӰ�췶Χ�Լ��ֺ�������Ӱ�죬����Ŀ��shelter���ɴ��Ҫ����
    void update_agent_target_shelter();

    // ����density��damage���µ�·Ȩ��
    void update_road_weight();

    // ����m_agent_run�б�
    void update_running_agent();

    // �ռ��������
    void collect_agent_trajectory();

    // ��Գ�ʼagent��û��ȷ������������·��ʱ����һ������ĵ�·����ǰ��
    void find_nearest_road();

    // ����ÿ����ȫ�������¼�����Ӱ�췶Χ�����е�·����ȫ���ľ���
    void update_shelter_distance();

    // update_shelter_distance���ã�spfaѰ�����·
    void shelter_shortest_path(const int shelter_id);

    static bool valid(const Point & p);
    static bool valid(const int x, const int y);
    static bool valid(const float x, const float y);
    static int compare(const float x);


    /****************** For Debug ********************/
    // ��ʾĳ�㵽�ﰲȫ����·��
    void get_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
        std::vector<float> & l_weight, const float l_x, const float l_y);
    
    void get_agent_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
        std::vector<float> & l_weight, const float l_x, const float l_y);

    void get_agent_energy(Agent & l_agent, Sample & l_sample, const float l_x, const float l_y);

    void set_run_step(const bool flag);

    void next_step();
};