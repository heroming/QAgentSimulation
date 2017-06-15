#include "algorithm"

#include "Simulation.h"

#include "Utilities/IO.h"
#include "Utilities/Tool.h"

// Algorithm文件中的队列
extern int front, tail;
extern int que[];
extern bool inq[];

const int VERTEX_NUMBER = 150000;

int graph_vis[VERTEX_NUMBER];
int graph_inq[VERTEX_NUMBER];
int graph_path[VERTEX_NUMBER];
float graph_dis[VERTEX_NUMBER];

int grid_vis[Simulation::WIDTH][Simulation::HEIGHT];
float grid_dis[Simulation::WIDTH][Simulation::HEIGHT];

const float Edge::DAMAGE_THREHOLD = 1.0;

const float Shelter::CAPACITY_RATE = 0.1;
const float Shelter::INFLUENCE_RATE = 0.1;

const float Simulation::ITER_THREHOLD = 1e-3;
const float Simulation::MIN_CONTROL_DISTANCE = 20.0;
const float Simulation::CHANGE_SHELTER_THREHOLD = 1.5;
const float Simulation::CHANGE_SHELTER_DISTANCE = 300.0;
const float Simulation::SEEING_DISTANCE_DIFFERENCE = 1.8;
const Point Sample::PO[] = {
    Point(0, 0),
    Point(0, 1), Point(0, 0.66), Point(0, 0.33), Point(0, -0.33),
    Point(-0.66, 0), Point(-0.33, 0), Point(0.33, 0), Point(0.66, 0),
    Point(-0.33, 0.8), Point(-0.33, 0.4), Point(0.33, 0.8), Point(0.33, 0.4)
};


#pragma region Points

Point Point::operator + (const Point & po) const
{
    return Point(x + po.x, y + po.y);
}

Point Point::operator - (const Point & po) const
{
    return Point(x - po.x, y - po.y);
}

Point Point::operator * (const float k) const
{
    return Point(x * k, y * k);
}

float Point::operator * (const Point & po) const
{
    return x * po.x + y * po.y;
}

float Point::operator ^ (const Point & po) const
{
    return x * po.y - y * po.x;
}

float Point::manhattan_distance(const Point & po) const
{
    return std::abs(x - po.x) + std::abs(y - po.y);
}

float Point::manhattan_length() const
{
    return std::abs(x) + std::abs(y);
}

float Point::distance(const int a, const int b) const
{
    return std::sqrt((x - a) * (x - a) + (y - b) * (y - b));
}

float Point::distance(const float a, const float b) const
{
    return std::sqrt((x - a) * (x - a) + (y - b) * (y - b));
}

float Point::distance(const Point & po) const
{
    return std::sqrt((x - po.x) * (x - po.x) + (y - po.y) * (y - po.y));
}

float Point::length() const
{
    return std::sqrt(x * x + y * y);
}

Point Point::normalize() const
{
    float l = length();
    if (l < -1e-8 || l > 1e-8) return *this * (1.0 / l); 
    return Point(0, 1.0);
}

#pragma endregion


#pragma region Vertex

float Vertex::manhattan_distance(const Vertex & u) const
{
    return p.manhattan_distance(u.p);
}

float Vertex::manhattan_distance(const Point & u) const
{
    return p.manhattan_distance(u);
}

float Vertex::distance(const Vertex & u) const
{
    return (p - u.p).length();
}

float Vertex::distance(const Point & u) const 
{
    return (p - u).length();
}

float Vertex::distance(const Vertex & u, const Vertex & v)
{
    return (u.p - v.p).length();
}
#pragma endregion


#pragma region Edge

Edge::Edge()
{
    is_main = 0;
    is_good = 1;
    dis = 0.0;
    width = 0.0;
    density = 0.0;
    damage = 0.0;
}

float Edge::get_weight_width(const float x)
{
    const float a = 3.0;
    const float b = 0.25;
    return a * (1.0 - 1.0 / (1.0 + std::exp(-b * x)));
}

float Edge::get_weight_density(const float x)
{
    const float a = 0.1;
    const float b = 0.15;
    const float w = 20;
    if (x <= w) return a * std::exp(b * x) - 1.0;
    return a * std::exp(b * w) - 1.0 + 0.1 * (x - w); 
}

float Edge::get_weight_main_road(const bool b)
{
    return b ? 0.5 : 1.0;
}

float Edge::get_weight_damage(const float w)
{
    const float a = 8.0;
    const float b = 0.15;
    return a * std::exp(- w * w * b * b);
}

float Edge::get_weight() const
{
    return dis * get_weight_main_road(is_main) * (1.0 + get_weight_width(width));
}

float Edge::get_weight_view() const
{
    return dis * get_weight_main_road(is_main) * (1.0 + get_weight_width(width) + get_weight_density(density) + get_weight_damage(damage));
}
#pragma endregion


#pragma region Agent

Agent::Agent()
{
    status = RUNNING;
    edge_on = NOTHING;
    target_shelter = NOTHING;
}

bool Agent::finish() const
{
    return status == DEAD || status == BLOCKED || status == ARRIVED;
}

float Agent::distance(const Agent & a) const
{
    return p.distance(a.p);
}

float Agent::distance(const Point & a) const
{
    return p.distance(a);
}

float Agent::distance(const Vertex & a) const
{
    return p.distance(a.p);
}

float Agent::manhattan_distance(const Point & a) const
{
    return p.manhattan_distance(a);
}

float Agent::manhattan_distance(const Agent & a) const
{
    return p.manhattan_distance(a.p);
}
#pragma endregion


#pragma region Shelter

void Shelter::initialize()
{
    count = 0;
    capacity = area * CAPACITY_RATE;
    influence = area * INFLUENCE_RATE;
}
#pragma endregion


#pragma region Sample

Sample::Sample()
{
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = weight[i] = weight_all[i] = 0.0;
    }
}

void Sample::get_sample(const float w, const Point & ve, const Point & p)
{
    Point a = ve.normalize(), b = Point(0, 1.0);
    float cosx = a * b, sinx = a ^ b;
    Point u(cosx, sinx), v(-sinx, cosx);
    for (int i = 0; i < N; ++ i)
    {
        po[i] = p + Point(PO[i] * u, PO[i] * v) * w;
    }
}

void Sample::get_distance(const std::vector<std::vector<float>> & d)
{
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = INF;
        int x = (int)po[i].x - MIN_X;
        int y = (int)po[i].y - MIN_Y;

        float bd[4] = {INF, INF, INF, INF};
        for (int k = 0; k < 4; ++ k)
        {
            int a = x + DX[k], b = y + DY[k];
            if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT)
            {
                bd[k] = d[a][b];
            }
        }
        float wx = bd[0] + (bd[2] - bd[0]) * (po[i].x - (int)po[i].x + 0.5) * 0.5;
        float wy = bd[1] + (bd[3] - bd[1]) * (po[i].y - (int)po[i].y + 0.5) * 0.5;
        dis[i] = (wx + wy) * 0.5;
    }
    float l_min = INF, l_max = 0;
    for (int i = 0; i < N; ++ i)
    {
        l_min = std::min(l_min, dis[i]);
        l_max = std::max(l_max, dis[i]);
    }
    float l_range = l_max - l_min + 0.001;
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = (dis[i] - l_min) / l_range;
    }
}

void Sample::get_distance(const float d[WIDTH][HEIGHT])
{
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = INF;
        int x = (int)po[i].x - MIN_X;
        int y = (int)po[i].y - MIN_Y;

        float bd[4] = {INF, INF, INF, INF};
        for (int k = 0; k < 4; ++ k)
        {
            int a = x + DX[k], b = y + DY[k];
            if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT)
            {
                bd[k] = d[a][b];
            }
        }
        float wx = bd[0] + (bd[2] - bd[0]) * (po[i].x - (int)po[i].x + 0.5) * 0.5;
        float wy = bd[1] + (bd[3] - bd[1]) * (po[i].y - (int)po[i].y + 0.5) * 0.5;
        dis[i] = (wx + wy) * 0.5;
        if (dis[i] > 1e8) dis[i] = -1.0;
    }
    float l_min = INF, l_max = 0;
    for (int i = 0; i < N; ++ i)
    {
        if (dis[i] < 0) continue;
        l_min = std::min(l_min, dis[i]);
        l_max = std::max(l_max, dis[i]);
    }
    float l_range = l_max - l_min + 0.001;
    for (int i = 0; i < N; ++ i)
    {
        if (dis[i] < 0) dis[i] = l_max;
        dis[i] = (dis[i] - l_min) / l_range;
    }
}

void Sample::get_distance(const Point & p)
{
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = po[i].distance(p);
    }
    float l_min = INF, l_max = 0;
    for (int i = 0; i < N; ++ i)
    {
        l_min = std::min(l_min, dis[i]);
        l_max = std::max(l_max, dis[i]);
    }
    float l_range = l_max - l_min + 0.001;
    for (int i = 0; i < N; ++ i)
    {
        dis[i] = (dis[i] - l_min) / l_range;
    }
}

void Sample::get_weight(const Point & p, const std::vector<std::vector<float>> & cw, const std::vector<std::vector<float>> & cd)
{
    for (int i = 0; i < N; ++ i)
    {
        weight[i] = INF;
        int x = (int)po[i].x - MIN_X;
        int y = (int)po[i].y - MIN_Y;
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        {
            //weight[i] = distance_weight(dis[i]) + obstacle_weight(cw[x][y]) + damage_weight(cw[x][y], cd[x][y]);
            weight[i] = distance_weight(dis[i]) + obstacle_weight(cw[x][y]);
        }
    }
}

void Sample::clear_weight_agent()
{
    for (int i = 0; i < N; ++ i) weight_all[i] = weight[i];
}

void Sample::add_agent_weight(const Agent & a)
{
    for (int i = 0; i < N; ++ i)
    {
        weight_all[i] += agent_weight(po[i].distance(a.np));
    }
}

Point Sample::get_min_weight_point() const
{
    int k = -1;
    float w = INF;
    for (int i = 0; i < N; ++ i)
    {
        if (weight_all[i] < w)
        {
            k = i;
            w = weight_all[i];
        }
    }
    return po[k];
}

float Sample::distance_weight(const float d)
{
    return d * 100.0;
} 

float Sample::agent_weight(const float d)
{
    const float a = 100.0;
    const float b = 12.0;
    return a * std::exp(- d * d / b);
}

float Sample::obstacle_weight(const float d)
{
    const float a = 50.0;
    const float b = 10.0;
    return a * std::exp(- d * d / b);
}

float Sample::damage_weight(const float d)
{
    const float a = 400.0;
    const float b = 10.0;
    return a * std::exp(- d * d / b);
}

float Sample::damage_weight(const float d, const float e)
{
    const float a0 = 350.0;
    const float b0 = 10.0;
    const float a1 = 50.0;
    const float b1 = 10.0;
    if (e < 1.0)
    {
        return a1 + damage_weight(d);
    }
    else
    {
        return a0 * std::exp(- d * d / b0) + a1 * std::exp(- e * e / b1);
    }
}

#pragma endregion


#pragma region Result

Result::Result(): status(RUNNING) {}

Result::Result(const Point & a): p(a), status(RUNNING) {}

Result::Result(const Point & a, const int s): p(a), status(s) {}
#pragma endregion


void Simulation::run()
{
    initialize();
    learning();
}

Simulation::Simulation()
{
    m_finish = false;
    m_initialize = false;
    m_step_debug = false;
    m_step_pause = false;
    m_timestamp = 0;
    m_index_main = 0;
    m_vertex_main = 0;
    m_index_connect = 0;
    m_vertex_connect = 0;
}

bool Simulation::is_initialized() const
{
    return m_initialize;
}

bool Simulation::is_finished() const
{
    return m_finish;
}

void Simulation::finish()
{
    m_finish = true;
}

void Simulation::initialize()
{
    /**********************  DEBUG **********************
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;
    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    std::string idx = "#";
    for (char c = '0'; c <= '9'; ++ c)
    {
        idx[0] = c;
        const std::string city_damage_path = "./Data/damage/city_damage_map_" + idx + ".dat";
        //const std::string city_damage_path = "E:/MAS Data/Damages/damage/K11_V19Dam4_9.dat";
        std::vector<std::vector<char>> city_damage;
        // 导入city map数据(-1 for shelter, 0 for road, 1 for damage)
        IO::load_city_grid_data(city_damage_path, city_damage, true);

        int l_row = (int)city_grid.size();
        int l_col = (int)city_grid[0].size();

        for (int i = 0; i < l_row; ++ i)
            for (int j = 0; j < l_col; ++ j)
            {
                if (city_grid[i][j] == 0)
                {
                    city_damage[i][j] = (city_damage[i][j] == 1 ? 3 : 0);
                }
                else
                {
                    city_damage[i][j] = city_grid[i][j];
                }
            }

       const std::string city_damage_path_out = "./Data/damage/city_damage_grid_" + idx + ".dat"; 
       IO::save_city_grid_data(city_damage_path_out, city_damage, true);

        //const std::string city_image_path = "E:/MAS Data/Damages/damage/K11_V19Dam4_9";
        const std::string city_image_path = "./Data/damage/city_damage_map_" + idx;
        IO::save_damage_image_data(city_image_path, city_grid, city_damage);
    }
    
    return;
    /**********************  DEBUG **********************/

    m_finish = false;
    if (! m_initialize)
    {
        // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
        const std::string city_grid_path = "./Data/city_grid.dat";
        IO::load_city_map(city_grid_path, m_grid);
        const std::string clearance_path = "./Data/grid/space_clearance.dat";
        IO::load_city_grid_data(clearance_path, m_clearance, true);

        // 导入道路数据
        initialize_road();
        initialize_main_road();
        initialize_connect();
        initialize_road_information();
        initialize_shelter();
        initialize_agent();
        initialize_vertex_map();

        m_initialize = true;
    }
    else
    {
        // init edge
        for (int k = 0; k < (int)m_edge.size(); ++ k)
        {
            m_edge[k].is_good = 1;
        }

        // init agent
        for (int k = 0; k < (int)m_agent.size(); ++ k)
        {
            m_agent[k].status = NOTHING;
            m_agent[k].edge_on = NOTHING;
            m_agent[k].p = m_agent[k].op;
            m_agent[k].target_shelter = m_init_shelter[k];
        }
    }
}

void Simulation::initialize_road()
{
    printf("Initialize road ...\n");
    std::vector<int> l_index;
    std::vector<float> l_point;
    const std::string road_path = "./Data/road/road.dat";
    IO::load_line_data(road_path, l_point, l_index);

    const int number_point = (int)l_point.size() / 3;
    const int number_index = (int)l_index.size() / 2;

    m_vertex_main = number_point;
    m_index_main = number_index;

    m_point.clear();
    for (int k = 0; k < number_point; ++ k)
    {
        Vertex it;
        it.id = k;
        it.p = Point(l_point[k * 3], l_point[k * 3 + 1]);
        m_point.push_back(it);
    }

    m_edge.clear();
    m_graph.clear();
    m_graph.resize(number_point);
    for (int k = 0; k < number_index; ++ k)
    {
        Edge it;

        it.id = (int)m_edge.size();
        it.u = l_index[k << 1];
        it.v = l_index[k << 1 | 1];

        m_edge.push_back(it);
        m_graph[it.u].push_back(it.id);

        it.id = (int)m_edge.size();
        it.v = l_index[k << 1];
        it.u = l_index[k << 1 | 1];

        m_edge.push_back(it);
        m_graph[it.v].push_back(it.id);
    }
}

void Simulation::initialize_main_road()
{
    printf("Initialize main road ...\n");
    std::vector<int> l_index;
    std::vector<float> l_point;
    const std::string main_road_path = "./Data/road/create_main_road.dat";
    IO::load_line_data(main_road_path, l_point, l_index);

    const int number_point = (int)l_point.size() / 3;
    const int number_index = (int)l_index.size() / 2;

    m_index_connect = m_index_main + number_index;
    m_vertex_connect = m_vertex_main + number_point;

    for (int k = 0; k < number_point; ++ k)
    {
        Vertex it;
        it.id = k + m_vertex_main;
        it.p = Point(l_point[k * 3], l_point[k * 3 + 1]);
        m_point.push_back(it);
        m_graph.push_back(std::vector<int>());
    }

    for (int k = 0; k < number_index; ++ k)
    {
        Edge it;
        it.is_main = true;

        it.id = (int)m_edge.size();
        it.u = l_index[k << 1] + m_vertex_main;
        it.v = l_index[k << 1 | 1] + m_vertex_main;

        m_edge.push_back(it);
        m_graph[it.u].push_back(it.id);

        it.id = (int)m_edge.size();
        it.v = l_index[k << 1] + m_vertex_main;
        it.u = l_index[k << 1 | 1] + m_vertex_main;

        m_edge.push_back(it);
        m_graph[it.v].push_back(it.id);
    }
}

void Simulation::initialize_connect()
{
    printf("Initialize connection ...\n");
    std::vector<int> l_index;
    const std::string connect_path = "./Data/road/connect.dat";
    IO::load_index_data(connect_path, l_index);

    const int number_index = (int)l_index.size() / 2;

    for (int k = 0; k < number_index; ++ k)
    {
        int l_u = l_index[k << 1];
        int l_v = l_index[k << 1 | 1] + m_vertex_main;

        Edge it;

        it.u = l_u;
        it.v = l_v;
        it.id = (int)m_edge.size();

        m_edge.push_back(it);
        m_graph[it.u].push_back(it.id);

        it.v = l_u;
        it.u = l_v;
        it.id = (int)m_edge.size();

        m_edge.push_back(it);
        m_graph[it.v].push_back(it.id);
    }
}

void Simulation::initialize_shelter()
{
    const std::string shelter_path = "./Data/shelter/shelter.txt";
    IO::load_shelters(shelter_path, m_shelter);
}

void Simulation::initialize_agent()
{
    printf("Initialize agent ...\n");
    //const std::string agent_path = "./Data/agent/run9/timestep9_0000000.dat";
    //const std::string agent_path = "./Data/agent/run0/timestep0_0000000.dat";
    const std::string agent_path = "./Data/agent/agent_1000_rand.dat";
    //const std::string agent_path = "./Data/agent/agent_10_rand.dat";
    //const std::string agent_path = "./Data/agent/agent_1000_rand.dat";
    std::vector<float> l_agent;
    IO::load_point_data(agent_path, l_agent);

    std::vector<std::vector<char>> shelter_id;
    const std::string shelter_id_path = "./Data/shelter/shelter_influence_id.dat";
    IO::load_city_grid_data(shelter_id_path, shelter_id, true);

    const int number_agent = (int)l_agent.size() / 3;

    const int l_row = (int)shelter_id.size();
    const int l_col = (int)shelter_id[0].size();

    m_init_shelter.resize(number_agent, -1);

    m_agent.clear();
    m_agent.resize(number_agent);
    for (int k = 0; k < number_agent; ++ k)
    {
        Agent & it = m_agent[k];
        it.p = it.op = Point(l_agent[k * 3], l_agent[k * 3 + 1]);
        it.max_velocity = 2.5;

        // 初始化目标避难所
        int a = (int)it.p.x - MIN_X;
        int b = (int)it.p.y - MIN_Y;
        if (a >= 0 && a < l_row && b >= 0 && b < l_col && shelter_id[a][b] != -1)
        {
            it.target_shelter = shelter_id[a][b];
            m_init_shelter[k] = it.target_shelter;
        }
    }
}

void Simulation::initialize_road_information()
{
    printf("Initialize road information ...\n");
    const float inf = 1e9;
    for (int k = 0; k < (int)m_edge.size(); ++ k)
    {
        Vertex u = m_point[m_edge[k].u];
        Vertex v = m_point[m_edge[k].v];

        // get road dis
        m_edge[k].dis = u.distance(v);
        
        // get road width
        float x0 = u.p.x - MIN_X, y0 = u.p.y - MIN_Y;
        float x1 = v.p.x - MIN_X, y1 = v.p.y - MIN_Y;

        int part = 0;
        float l_width = inf;
        float dx = std::abs(x0 - x1), dy = std::abs(y0 - y1);
        if (dy <= dx)
        {
            part = std::abs(x1 - x0);
        }
        else
        {
            part = std::abs(y1 - y0);
        }
        if (part == 0)
        {
            l_width = m_clearance[(int)x0][(int)y0];
        }
        else
        {
            float l_count = 0;
            float x = x0, y = y0, px = (x1 - x0) / part, py = (y1 - y0) / part;
            for (int i = 0; i <= part; ++ i)
            {
                l_count = m_clearance[(int)x][(int)y];
                x += px, y += py;
            }
            l_width = l_count / part;
        }
        m_edge[k].width = l_width;
    }
    /*
    std::vector<int> cnt(300, 0);
    for (int i = 0; i < (int)m_edge.size(); ++ i)
    {
        float l_width = m_edge[i].width;
        if (l_width >= 0 && l_width < 300)
        {
            ++ cnt[(int)l_width];
        }
    }
    for (int i = 0; i < 300; i += 3) if (cnt[i])
    {
        printf("width[%d] = %d, width[%d] = %d, width[%d] = %d\n", i, cnt[i], i + 1, cnt[i + 1], i + 2, cnt[i + 2]);
    }
    */
}

void Simulation::initialize_vertex_map()
{
    printf("Initialize vertex map ...\n");
    m_vertex_map.clear();
    m_vertex_map.resize(WIDTH, std::vector<int>(HEIGHT, -1));
    for (int i = 0; i < (int)m_point.size(); ++ i)
    {
        const Point & p = m_point[i].p;
        int x = (int)p.x - MIN_X;
        int y = (int)p.y - MIN_Y;
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        {
            m_vertex_map[x][y] = m_point[i].id;
        }
    }
}

void Simulation::learning()
{
    simulation();
    m_finish = true;
}

void Simulation::simulation()
{
    const int number_agent = (int)m_agent.size();

    m_agent_run.clear();
    for (int i = 0; i < number_agent; ++ i)
    {
        m_agent_run.push_back(i);
    }

    m_result.clear();
    m_result.resize(number_agent);

    update_road_weight();
    update_shelter_distance();
    find_nearest_road();

    collect_agent_trajectory();
    update_running_agent();

    const int MAX_TIMESTEMP = 15000;
    const int PRINT_PROCESS_TIMESTEMP = 50;

    int timestamp = 0;
    while (! m_finish && ! m_agent_run.empty() && timestamp < MAX_TIMESTEMP)
    {
        if (timestamp && m_step_debug)
        {
            if (m_step_pause) continue;
            m_step_pause = true;
        }

        if (timestamp && timestamp % PRINT_PROCESS_TIMESTEMP == 0) printf("\n");

        if (timestamp % UPDATE_DAMAGE == 0)
        {
            update_damage(timestamp / UPDATE_DAMAGE);
        }
        if (timestamp % UPDATE_PATH_PLANNING == 0)
        {
            update_road_weight();
            global_path_planning();

            /*
            std::vector<int> l_ids;
            std::vector<float> l_points;
            for (int i = 0; i < m_agent.size(); ++ i)
            {
                const Agent & man = m_agent[i];
                l_points.push_back(man.p.x);
                l_points.push_back(man.p.y);
                l_points.push_back(0.0);
                l_ids.push_back(man.target_shelter);
            }
            IO::save_agent_data("./Data/agent/agent_to_shelter.txt", l_points, l_ids, false);
            return;
            */
        }
        if (timestamp && timestamp % UPDATE_SHELTER_DISTANCE == 0)
        {
            update_shelter_distance();
            update_agent_target_shelter();
        }

        //printf(">");
        printf("%d >>> \n", timestamp);

        local_movement();
        collect_agent_trajectory();
        update_running_agent();

        // TODO 在更新灾害时考虑遇害agent
        // TODO 到达安全区时shelter的count+1

        ++ timestamp;
    }

    printf("\nSimulation finish.\n");
}

void Simulation::global_path_planning()
{
    printf("+");
    int l_count_block = 0;
    const int number_agent_left = (int)m_agent_run.size();
    memset(graph_inq, -1, sizeof(graph_inq));
    memset(graph_vis, -1, sizeof(graph_vis));
    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        if (man.finish()) continue;
        if (man.target_shelter == NOTHING)
        {
            ++ l_count_block;
            man.status = BLOCKED;
            continue;
        }

        // 经过城市图模型的顶点和边bfs，找到最小的通往target_shelter的顶点
        int target_vertex_id = NOTHING;
        float target_shelter_dis = INF;

        // 考虑有没有在边上，一共三种可能
        // -- NOTHING表示初始化时未更新，该agent被困或遇害（否则在 find_nearest_road中会更新为负值）
        // -- < 0，取负号得到的正整数表示控制点id
        // -- > 0，已经在边上
        int start_vertex = NOTHING;
        if (man.edge_on == NOTHING)
        {
            ++ l_count_block;
            man.status = BLOCKED;
            continue;
        }
        else if (man.edge_on < 0)
        {
            start_vertex = -man.edge_on;
        }
        else
        {
            start_vertex = m_edge[man.edge_on].v;
        }

        // spfa 求 start_vertex出发到达目标shelter的最短路
        // 视线范围内道路权重加入灾害与密度，视线范围外道路权重不变
        front = tail = 0;
        que[front ++] = start_vertex;
        graph_inq[start_vertex] = id;
        graph_dis[start_vertex] = 0.0;
        graph_vis[start_vertex] = id;
        graph_path[start_vertex] = -1;
        while (tail < front)
        {
            const int u = que[tail ++];
            graph_inq[u] = -1;

            // 能见范围之内继续计算，之外检测是否是最小距离的顶点
            if (man.distance(m_point[u]) < SEEING_DISTANCE)
            {
                for (int i = 0; i < (int)m_graph[u].size(); ++ i)
                {
                    const Edge & e = m_edge[m_graph[u][i]];
                    if (! e.is_good) continue;

                    float l_dis = graph_dis[u] + e.weight_view;
                    if (graph_vis[e.v] != id || l_dis < graph_dis[e.v])
                    {
                        graph_vis[e.v] = id;
                        graph_path[e.v] = e.id;
                        graph_dis[e.v] = l_dis;
                        if (graph_inq[e.v] != id)
                        {
                            graph_inq[e.v] = id;
                            que[front ++] = e.v;
                        }
                    }
                }
            }
            else
            {
                // 加上u到shelter的距离
                float l_dis_to_shelter = graph_dis[u] + m_dis_to_shelter[man.target_shelter][u];
                if (l_dis_to_shelter < target_shelter_dis)
                {
                    target_vertex_id = u;
                    target_shelter_dis = l_dis_to_shelter;
                }
            }
        }

        if (target_vertex_id == NOTHING)
        {
            ++ l_count_block;
            man.status = BLOCKED;
            continue;
        }

        // 将走出视线范围的路径保存到path_viwe中
        int e_id = graph_path[target_vertex_id];
        man.path_index = 0;
        man.path_view.clear();
        man.path_view.push_back(m_next_edge_to_shelter[man.target_shelter][target_vertex_id]);
        if (man.path_view.back() == -1) man.path_view.pop_back();
        while (e_id != -1)
        {
            man.path_view.push_back(e_id);
            const Edge & e = m_edge[e_id];
            e_id = graph_path[e.u];
        }


        // reverse path_view顺序
        int l = 0, r = (int)man.path_view.size() - 1;
        while (l < r) std::swap(man.path_view[l ++], man.path_view[r --]);
    }
    printf("path planning block agent : %d\n", l_count_block);
}

void Simulation::local_movement()
{
    int l_count_block = 0;

    if (m_neibor.empty())
    {
        m_neibor.resize(m_agent.size());
        m_sample.resize(m_agent.size());
    }

    const int number_agent_left = (int)m_agent_run.size();

    memset(grid_vis, INFI, sizeof(grid_vis));

    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        if (man.finish()) continue;

        const int px = (int)man.p.x - MIN_X;
        const int py = (int)man.p.y - MIN_Y;

        // bfs计算可视范围的距离
        grid_vis[px][py] = id;
        grid_dis[px][py] = 0.0;

        front = tail = 0;
        que[front ++] = px << 16 | py;

        // bfs找man的近邻
        while (tail < front)
        {
            const int s = que[tail ++];
            const int x = s >> 16;
            const int y = s & BIT;
            for (int i = 0; i < 8; ++ i)
            {
                const int a = x + DX[i];
                const int b = y + DY[i];
                if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT && grid_vis[a][b] != id)
                {
                    grid_vis[a][b] = id;
                    grid_dis[a][b] = grid_dis[x][y] + DIS[i];
                    if (grid_dis[a][b] < SEEING_DISTANCE)
                    {
                        que[front ++] = a << 16 | b;
                    }
                }
            }
        }

        // 计算可视范围内的可见的agent集合
        m_neibor[id].clear();
        int gx = px / GRID_STRIP, gy = py / GRID_STRIP; 
        for (int d = 0; d < 9; ++ d)
        {
            int a = gx + DX[d], b = gy + DY[d];
            if (a >= 0 && a < GRID_WIDTH && b >= 0 && b < GRID_HEIGHT)
            {
                const std::vector<int> & v = m_agent_grid[0][0];
                for (int i = 0; i < (int)v.size(); ++ i)
                {
                    const Agent & pe = m_agent[v[i]];
                    const int tx = (int)pe.p.x - MIN_X;
                    const int ty = (int)pe.p.y - MIN_Y;

                    if (id != v[i] && grid_vis[tx][ty] == id && grid_dis[tx][ty] < SEEING_DISTANCE_DIFFERENCE)
                    {
                        m_neibor[id].push_back(v[i]);
                    }
                }
            }
        }

        // 计算控制点，需要考虑两种情况
        // 1. 考虑已经能看到安全区的情况，直接根据shelter_clearance网格进行移动，此时不需要计算
        // 2. 根据agent的path_view来进行，需要考虑还未到边上edge_on < 0的情况
        if (m_clearance_shelter[px][py] < SEEING_DISTANCE) man.status = ARRIVING;

        if (man.status != ARRIVING)
        {
            if (man.path_view.empty())
            {
                man.status = BLOCKED;
                ++ l_count_block;
            }
            else
            {
                bool has_control_point = 0;
                for (int i = man.path_index; i < (int)man.path_view.size(); ++ i)
                {
                    const Edge & e = m_edge[man.path_view[i]];
                    if (man.distance(m_point[e.v]) > MIN_CONTROL_DISTANCE)
                    {
                        man.cp = m_point[e.v].p;
                        has_control_point = 1;
                        break;
                    }
                }
                if (! has_control_point)
                {
                    //has_control_point = 1;
                    //for (int i = man.path_index; i < (int)man.path_view.size(); ++ i)
                    //{
                        //const Edge & e = m_edge[man.path_view[i]];
                        //printf("[%d / %d] ==== %.2lf\n", i, (int)man.path_view.size(), man.distance(m_point[e.v]));
                    //}
                    ++ l_count_block;
                    man.status = BLOCKED;
                }
            }
        }

        if (man.finish()) continue;

        // 计算采样点集合，计算距离、障碍以及灾害因素
        // 区分agent的状态：ARRIVING 与 RUNNING
        m_sample[id].get_sample(m_agent[id].max_velocity, m_agent[id].v, m_agent[id].p);
        if (man.status == ARRIVING)
        {
            m_sample[id].get_distance(m_clearance_shelter);
        }
        else
        {
            // 用A*算法计算控制点到sample区域的距离
            const float MAX_DIS = man.distance(man.cp) * 3.0;

            const int cx = (int)man.cp.x - MIN_X;
            const int cy = (int)man.cp.y - MIN_Y;

            grid_vis[cx][cy] = -id - 1;
            grid_dis[cx][cy] = 0.0;

            front = tail = 0;
            que[front ++] = cx << 16 | cy;

            while (tail < front)
            {
                const int s = que[tail ++];
                const int x = s >> 16;
                const int y = s & BIT;
                for (int i = 0; i < 8; ++ i)
                {
                    const int a = x + DX[i];
                    const int b = y + DY[i];
                    if (grid_dis[x][y] + man.cp.distance(a + MIN_X, b + MIN_Y) > MAX_DIS) continue;

                    if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT && grid_vis[a][b] != -id - 1)
                    {
                        if (m_grid[a][b] <= 0)
                        {
                            grid_vis[a][b] = -id - 1;
                            grid_dis[a][b] = grid_dis[x][y] + DIS[i];
                            que[front ++] = a << 16 | b;
                        }
                        else
                        {
                            grid_dis[a][b] = INF;
                        }
                    }
                }
            }

            /*
            int mx = (int)man.p.x - MIN_X;
            int my = (int)man.p.y - MIN_Y;
            for (int i = 0; i < 9; ++ i)
            {
                int a = mx + DX[i];
                int b = my + DY[i];
                printf("%d : dis[%d][%d] = %.2f\n",  i, a, b, grid_dis[a][b]);
            }
            */

            m_sample[id].get_distance(grid_dis);
            //m_sample[id].get_distance(man.cp);
        }

        m_sample[id].get_weight(m_agent[id].cp, m_clearance, m_clearance_damage);

        // 计算下一步位置np，初始为惯性下一步位置
        if (valid(m_sample[id].po[0]))
        {
            man.np = m_sample[id].po[0];
        }
        else
        {
            ++ l_count_block;
            man.status = BLOCKED;
        }
    }

    // 计算最优的局部移动的解
    int iter = 10;
    float dif = INF;
    while (dif > ITER_THREHOLD && iter --)
    {
        dif = 0;
        for (int k = 0; k < number_agent_left; ++ k)
        {
            const int id = m_agent_run[k];
            m_sample[id].clear_weight_agent();
            for (int i = 0; i < (int)m_neibor[id].size(); ++ i)
            {
                int u = m_neibor[id][i];
                m_sample[id].add_agent_weight(m_agent[u]);
            }

            Point p = m_sample[id].get_min_weight_point();
            dif += p.manhattan_distance(m_agent[id].np);
            m_agent[id].np = p;
        }
        dif /= number_agent_left;
    }

    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        if (man.finish()) continue;

        man.v = man.np - man.p;
        man.p = man.np;

        const int px = (int)man.p.x - MIN_X;
        const int py = (int)man.p.y - MIN_Y;

        // 记录到达安全区情况，并更新其状态
        if (m_clearance_shelter[px][py] <= 1.2)
        {
            man.status = ARRIVED;
            continue;
        }

        // 更新edge_on或将其值从负值到道路id
        for (int i = man.path_index; i < (int)man.path_view.size(); ++ i)
        {
            const Edge & e = m_edge[man.path_view[i]];
            if (m_point[e.v].manhattan_distance(man.cp) < man.manhattan_distance(man.cp))
            {
                man.edge_on = e.id;
                man.path_index = i;
                break;
            }
        }
    }
    printf("local move agent block : %d\n", l_count_block);
}

void Simulation::update_damage(const int k)
{
    std::string idx = "#";
    idx[0] = '0' + k;

    // 导入damage数据
    const std::string city_damage_path = "./Data/damage/city_damage_grid_" + idx + ".dat";
    const std::string city_damage_clearance_path = "./Data/damage/city_damage_clearance_" + idx + ".dat";
    const std::string city_shelter_clearance_path = "./Data/shelter/shelter_clearance_" + idx + ".dat";

    // 导入city damage数据(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
    IO::load_city_grid_data(city_damage_path, m_grid_damage, true);
    // 导入city damage clearance数据
    IO::load_city_grid_data(city_damage_clearance_path, m_clearance_damage, true);
    // 导入city shelter clearance数据
    IO::load_city_grid_data(city_shelter_clearance_path, m_clearance_shelter, true);

    // 将damage数据反映到道路上
    int l_damage_count = 0;
    for (int k = 0; k < (int)m_edge.size(); ++ k)
    {
        Edge & e = m_edge[k];
        
        // 损坏道路不考虑
        if (! e.is_good) continue;

        Vertex u = m_point[e.u];
        Vertex v = m_point[e.v];

        // get road damage
        float x0 = u.p.x - MIN_X, y0 = u.p.y - MIN_Y;
        float x1 = v.p.x - MIN_X, y1 = v.p.y - MIN_Y;

        int part = 0;
        float l_damage = INF;
        float dx = std::abs(x0 - x1), dy = std::abs(y0 - y1);
        if (dy <= dx)
        {
            part = std::abs(x1 - x0);
        }
        else
        {
            part = std::abs(y1 - y0);
        }
        if (part == 0)
        {
            l_damage = m_clearance[(int)x0][(int)y0];
        }
        else
        {
            float l_count = 0;
            float x = x0, y = y0, px = (x1 - x0) / part, py = (y1 - y0) / part;
            for (int i = 0; i <= part; ++ i)
            {
                l_count = m_clearance[(int)x][(int)y];
                x += px, y += py;
            }
            l_damage = l_count / part;
        }
        e.damage = l_damage;

        // 道路损坏
        if (l_damage < Edge::DAMAGE_THREHOLD) 
        {
            e.is_good = 1;
            ++ l_damage_count;
        }
    }
    //printf("Edge damaged : %d\n", l_damage_count);
}

void Simulation::update_agent_target_shelter()
{
    const int number_agent_left = (int)m_agent_run.size();
    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        if (man.finish()) continue;

        // 经过城市图模型的顶点和边bfs，找到最小的通往target_shelter的顶点
        int target_vertex_id = NOTHING;
        float target_shelter_dis = INF;

        // 是否要更换shelter
        int l_shelter = NOTHING;
        int l_shelter_veretx_id = NOTHING;
        float l_shelter_dis = INF;

        // 考虑有没有在边上，一共三种可能
        // -- NOTHING表示初始化时未更新，该agent被困或遇害（否则在 find_nearest_road中会更新为负值）
        // -- < 0，取负号得到的正整数表示控制点id
        // -- > 0，已经在边上
        int start_vertex = NOTHING;
        if (man.edge_on < 0)
        {
            start_vertex = -man.edge_on;
        }
        else
        {
            start_vertex = m_edge[man.edge_on].v;
        }

        // spfa 求 start_vertex出发到达目标shelter的最短路
        // 视线范围内道路权重加入灾害与密度，视线范围外道路权重不变
        front = tail = 0;
        que[front ++] = start_vertex;
        graph_inq[start_vertex] = id;
        graph_dis[start_vertex] = 0.0;
        graph_vis[start_vertex] = id;
        graph_path[start_vertex] = -1;
        while (tail < front)
        {
            const int u = que[tail ++];
            graph_inq[u] = -1;

            // 能见范围之内继续计算，之外检测是否是最小距离的顶点
            if (man.distance(m_point[u]) < SEEING_DISTANCE)
            {
                for (int i = 0; i < (int)m_graph[u].size(); ++ i)
                {
                    const Edge & e = m_edge[m_graph[u][i]];
                    if (! e.is_good) continue;

                    float l_dis = graph_dis[u] + e.weight_view;
                    if (graph_vis[e.v] != id || l_dis < graph_dis[e.v])
                    {
                        graph_vis[e.v] = id;
                        graph_path[e.v] = e.id;
                        graph_dis[e.v] = l_dis;
                        if (graph_inq[e.v] != id)
                        {
                            graph_inq[e.v] = id;
                            que[front ++] = e.v;
                        }
                    }
                }
            }
            else
            {
                // 加上u到shelter的距离
                for (int i = 0; i < SHELTER_NUMBER; ++ i)
                {
                    float l_dis_to_shelter = graph_dis[u] + m_dis_to_shelter[i][u];
                    if (i == man.target_shelter)
                    {
                        if (l_dis_to_shelter < target_shelter_dis)
                        {
                            target_vertex_id = u;
                            target_shelter_dis = l_dis_to_shelter;
                        }
                    }
                    else
                    {
                        if (l_dis_to_shelter < l_shelter_dis)
                        {
                            l_shelter = i;
                            l_shelter_veretx_id = u;
                            l_shelter_dis = l_dis_to_shelter;
                        }
                    }
                }
            }
        }

        // 根据距离以及shelter参数判断是否更换shelter
        if (target_shelter_dis > CHANGE_SHELTER_DISTANCE && target_shelter_dis > l_shelter_dis * CHANGE_SHELTER_THREHOLD)
        {
            // 将走出视线范围的路径保存到path_viwe中
            man.target_shelter = l_shelter;

            man.path_index = 0;
            man.path_view.clear();
            int e_id = graph_path[l_shelter_veretx_id];
            while (e_id != -1)
            {
                man.path_view.push_back(e_id);
                const Edge & e = m_edge[e_id];
                e_id = graph_path[e.u];
            }

            // reverse path_view顺序
            int l = 0, r = (int)man.path_view.size() - 1;
            while (l < r) std::swap(man.path_view[l ++], man.path_view[r --]);
        }
    }
}

void Simulation::update_road_weight()
{
    // 根据agent移动后位置更新人员GRID数据
    for (int i = 0; i < GRID_WIDTH; ++ i)
        for (int j = 0; j < GRID_HEIGHT; ++ j)
            m_agent_grid[i][j].clear();

    const int number_agent_left = (int)m_agent_run.size();
    for (int i = 0; i < number_agent_left; ++ i)
    {
        const int id = m_agent_run[i];
        const Agent & man = m_agent[id];
        const int x = (int)(man.p.x - MIN_X) / GRID_STRIP;
        const int y = (int)(man.p.y - MIN_Y) / GRID_STRIP;
        m_agent_grid[x][y].push_back(id);
    }

    /*
    std::vector<int> cnt(1000, 0);
    for (int i = 0; i < GRID_WIDTH; ++ i)
        for (int j = 0; j < GRID_HEIGHT; ++ j)
        {
            int k = (int)m_agent_grid[i][j].size();
            if (k < 1000) ++ cnt[k];
        }
    for (int i = 0; i < 1000; ++ i) if (cnt[i]) printf("dens[%d] = %d\n", i, cnt[i]);
    */

    // 更新道路密度
    const int number_edge = (int)m_edge.size();
    for (int i = 0; i < number_edge; ++ i)
    {
        Edge & e = m_edge[i];

        // 损坏道路不考虑
        if (! e.is_good) continue;

        const Point & u = m_point[e.u].p;
        const Point & v = m_point[e.v].p;
        const int x0 = (int)(u.x - MIN_X) / GRID_STRIP;
        const int y0 = (int)(u.y - MIN_Y) / GRID_STRIP;
        const int x1 = (int)(v.x - MIN_X) / GRID_STRIP;
        const int y1 = (int)(v.y - MIN_Y) / GRID_STRIP;
        int l_count = (int)m_agent_grid[x0][y0].size() + (int)m_agent_grid[x1][y1].size();
        e.density = l_count / 2.0;

        e.weight = e.get_weight();
        e.weight_view = e.get_weight_view();
    }
}

void Simulation::update_running_agent()
{
     for (int k = 0; k < (int)m_agent_run.size(); ++ k)
     {
         const int id = m_agent_run[k];
         Agent & man = m_agent[id];

         // 去除不用继续仿真的agent
         if (man.finish())
         {
             // 先与最后一个元素交换，再去掉最后元素
             const int last = (int)m_agent_run.size() - 1;
             if (k < last)
             {
                 std::swap(m_agent_run[k --], m_agent_run[last]);
                 m_agent_run.pop_back();
             }
         }
     }
     printf("Running agent: %d\n", m_agent_run.size());
}

void Simulation::collect_agent_trajectory()
{
    const int number_agent_left = (int)m_agent_run.size();
    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        // 如果已经标记完成，直接
        if (man.finish())
        {
            m_result[id].push_back(Result(man.p, man.target_shelter));
        }
        else
        {
            m_result[id].push_back(Result(man.p));
        }
    }
    ++ m_timestamp;
}

void Simulation::find_nearest_road()
{
    front = tail = 0;
    memset(grid_vis, -1, sizeof(grid_vis));

    const int number_agent_left = (int)m_agent_run.size();

    // 改为从所有的vertex出发，减少计算量
    for (int i = 0; i < (int)m_point.size(); ++ i)
    {
        const Point & p = m_point[i].p;
        int x = (int)p.x - MIN_X;
        int y = (int)p.y - MIN_Y;
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
        {
            grid_vis[x][y] = i;
            grid_dis[x][y] = 0.0;
            que[front ++] = x << 16 | y;
        }
    }

    while (tail < front)
    {
        const int s = que[tail ++];
        int x = s >> 16;
        int y = s & BIT;
        for (int k = 0; k < 8; ++ k)
        {
            int a = x + DX[k];
            int b = y + DY[k];
            if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT && grid_vis[a][b] == -1 && m_grid[a][b] <= 0)
            {
                grid_vis[a][b] = grid_vis[x][y];
                grid_dis[a][b] = grid_dis[x][y] + DIS[k];
                que[front ++] = a << 16 | b;
            }
        }
    }

    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & man = m_agent[id];

        int x = (int)man.p.x - MIN_X;
        int y = (int)man.p.y - MIN_Y;

        const int v_id = grid_vis[x][y];
        if (v_id != -1)
        {
            man.cp = m_point[grid_vis[x][y]].p;
            man.v = man.cp - man.p;
            man.edge_on = -grid_vis[x][y];
        }
        else
        {
            man.status = BLOCKED;
        }
    }

    /*
    // 此处为从agent出发，找到最近的vertex
    // 改为先对所有vertex进行bfs处理，在遍历agent找最近点
    for (int k = 0; k < number_agent_left; ++ k)
    {
        const int id = m_agent_run[k];
        Agent & ag = m_agent[id];

        // bfs
        int x = (int)ag.p.x - MIN_X;
        int y = (int)ag.p.y - MIN_Y;

        front = tail = 0;
        grid_dis[x][y] = 0;
        grid_vis[x][y] = id;
        que[front ++] = x << 16 | y;

        while (tail < front)
        {
            const int s = que[tail ++];
            x = s >> 16;
            y = s & BIT;
            if (m_vertex_map[x][y] != -1) break;
            for (int k = 0; k < 8; ++ k)
            {
                int a = x + DX[k];
                int b = y + DY[k];
                if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT && grid_vis[a][b] != id)
                {
                    grid_vis[a][b] = id;
                    grid_dis[a][b] = grid_dis[x][y] + DIS[k];
                    que[front ++] = a << 16 | b;
                }
            }
        }

        const int v_id = m_vertex_map[x][y];
        ag.cp = m_point[m_vertex_map[x][y]].p;
        ag.v = ag.cp - ag.p;
        ag.edge_on = -1;
    }
    */
}

void Simulation::update_shelter_distance()
{
    printf("update shelter distance ...\n");
    if (m_dis_to_shelter[0].empty())
    {
        const int number_vertex = (int)m_point.size();
        for (int k = 0; k < SHELTER_NUMBER; ++ k)
        {
            m_dis_to_shelter[k].resize(number_vertex, INF);
            m_next_edge_to_shelter[k].resize(number_vertex, NOTHING);
        }
    }
    else
    {
        for (int k = 0; k < SHELTER_NUMBER; ++ k)
        {
            std::fill(m_dis_to_shelter[k].begin(), m_dis_to_shelter[k].end(), INF);
            std::fill(m_next_edge_to_shelter[k].begin(), m_next_edge_to_shelter[k].end(), NOTHING);
        }
    }

    for (int k = 0; k < (int)m_shelter.size(); ++ k)
    {
        front = tail = 0;

        std::fill(inq, inq + m_vertex_connect, false);
        std::vector<int> & ids = m_shelter[k].ids;
        for (int i = 0; i < (int)ids.size(); ++ i)
        {
            m_dis_to_shelter[k][ids[i]] = 0.0;
            m_next_edge_to_shelter[k][ids[i]] = -1;
            inq[ids[i]] = true;
            que[front ++] = ids[i];
        }
        ids = m_shelter[k].main_ids;
        for (int i = 0; i < (int)ids.size(); ++ i)
        {
            const int id = ids[i] + m_vertex_main;
            m_dis_to_shelter[k][id] = 0.0;
            m_next_edge_to_shelter[k][id] = -1;
            inq[id] = true;
            que[front ++] = id;
        }

        shelter_shortest_path(k);
    }
    printf("\n");
}

void Simulation::shelter_shortest_path(const int shelter_id)
{
    Shelter & l_shelter = m_shelter[shelter_id];

    // 根据容量计算安全区影响范围，大于50%容量影响不变，小于50%线性递减
    const float l_cap_rate = (l_shelter.capacity - l_shelter.count + 0.0) / l_shelter.capacity;
    const int l_inf = l_shelter.area * Shelter::INFLUENCE_RATE;
    l_shelter.influence = (l_cap_rate < 0.5) ? l_inf * l_cap_rate * 2 : l_inf;

    std::vector<float> & l_dis = m_dis_to_shelter[shelter_id];
    std::vector<int> & l_next = m_next_edge_to_shelter[shelter_id];
    while (tail < front)
    {
        int u = que[tail ++];
        inq[u] = false;
        for (int i = 0; i < (int)m_graph[u].size(); ++ i)
        {
            int k = m_graph[u][i];
            const Edge & e = m_edge[k];
            float dis = l_dis[e.u] + e.weight;
            if (e.is_good && dis < l_dis[e.v] && dis < l_shelter.influence)
            {
                l_dis[e.v] = dis;
                l_next[e.v] = e.id;
                if (! inq[e.v])
                {
                    inq[e.v] = true;
                    que[front ++] = e.v;
                }
            }
        }
    }
}

bool Simulation::valid(const Point & p)
{
    return valid(p.x, p.y);
}

bool Simulation::valid(const int x, const int y)
{
    int a = x - MIN_X, b = y - MIN_Y;
    return a >= 0 && a < WIDTH && b >= 0 && b < WIDTH;
}

bool Simulation::valid(const float x, const float y)
{
    int a = (int)x - MIN_X, b = (int)y - MIN_Y;
    return a >= 0 && a < WIDTH && b >= 0 && b < WIDTH;
}

int Simulation::compare(const float x)
{
    static const float eps = 1e-6;
    return (int)(x > eps) - (x < -eps);
}

int Simulation::get_timestamp() const
{
    return m_timestamp;
}

const std::vector<float> & Simulation::get_timestamp_data(const int t)
{
    //std::vector<int> l_ids;
    m_one_result.clear();
    for (int k = 0; k < (int)m_result.size(); ++ k)
    {
        if (t < (int)m_result[k].size())
        {
            m_one_result.push_back(m_result[k][t].p.x);
            m_one_result.push_back(m_result[k][t].p.y);
            m_one_result.push_back(0.0);
            //l_ids.push_back(k);
        }
    }
    //char l_path[64];
    //sprintf_s(l_path, "./Data/agent/result/timestep_%07d.txt", t);
    //IO::save_agent_data(l_path, m_one_result, l_ids, false);
    return m_one_result;
}


/****************** For Debug ********************/
void Simulation::get_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
    std::vector<float> & l_weight, const float l_x, const float l_y)
{
    const int x = (int)l_x - MIN_X;
    const int y = (int)l_y - MIN_Y;

    // dfs寻找最近的顶点
    front = tail = 0;
    memset(grid_vis, 0, sizeof(grid_vis));

    int px, py;
    grid_dis[x][y] = 0;
    grid_vis[x][y] = 1;
    que[front ++] = x << 16 | y;
    while (tail < front)
    {
        const int s = que[tail ++];
        px = s >> 16;
        py = s & BIT;
        if (m_vertex_map[px][py] != -1) break;
        for (int k = 0; k < 8; ++ k)
        {
            int a = px + DX[k];
            int b = py + DY[k];
            if (a >= 0 && a < WIDTH && b >= 0 && b < HEIGHT && grid_vis[a][b] == 0)
            {
                grid_vis[a][b] = 1;
                grid_dis[a][b] = grid_dis[px][py] + DIS[k];
                que[front ++] = a << 16 | b;
            }
        }
    }

    l_point.clear();
    l_main.clear();
    l_weight.clear();

    l_point.push_back(Point(l_x, l_y));
    l_main.push_back(false);
    l_weight.push_back(5.0);

    const int vid = m_vertex_map[px][py];
    int l_target_shelter = 0;
    for (int i = 0; i < SHELTER_NUMBER; ++ i)
    {
        if (m_dis_to_shelter[i][vid] < m_dis_to_shelter[l_target_shelter][vid])
        {
            l_target_shelter = i;
        }
    }

    int eid = m_next_edge_to_shelter[l_target_shelter][vid];
    l_point.push_back(m_point[m_edge[eid].v].p);
    while (eid != -1)
    {
        Edge & e = m_edge[eid];
        l_point.push_back(m_point[e.u].p);
        l_main.push_back(e.is_main);
        l_weight.push_back(e.weight);
        eid = m_next_edge_to_shelter[l_target_shelter][e.u];
    }
}

void Simulation::get_agent_road_to_shelter(std::vector<Point> & l_point, std::vector<bool> & l_main,
    std::vector<float> & l_weight, const float l_x, const float l_y)
{
    const Point po = Point(l_x, l_y);

    // 寻找最近的agent
    int aid = -1;
    float dis = INF;
    for (int k = 0; k < (int)m_agent_run.size(); ++ k)
    {
        const int id = m_agent_run[k];
        float l_dis = m_agent[id].distance(po);
        if (l_dis < dis)
        {
            aid = id;
            dis = l_dis;
        }
    }

    l_point.clear();
    l_main.clear();
    l_weight.clear();

    const Agent & man = m_agent[aid];
    l_point.push_back(man.p);

    // 获取agent的视野内路径
    for (int i = man.path_index; i < (int)man.path_view.size(); ++ i)
    {
        int eid = man.path_view[i];
        Edge & e = m_edge[eid];
        l_point.push_back(m_point[e.v].p);
        l_main.push_back(true);
        l_weight.push_back(e.weight);
    }

    // 获取agent的全局规划路径
    if (man.path_view.empty()) return;
    int vid = m_edge[man.path_view.back()].v;

    int l_target_shelter = man.target_shelter;
    int eid = m_next_edge_to_shelter[l_target_shelter][vid];
    while (eid != -1 && eid != NOTHING)
    {
        Edge & e = m_edge[eid];
        l_point.push_back(m_point[e.u].p);
        l_main.push_back(false);
        l_weight.push_back(e.weight);
        eid = m_next_edge_to_shelter[l_target_shelter][e.u];
    }
}

void Simulation::get_agent_energy(Agent & l_agent, Sample & l_sample, const float l_x, const float l_y)
{
    const Point po = Point(l_x, l_y);

    // 寻找最近的agent
    int aid = -1;
    float dis = INF;
    for (int k = 0; k < (int)m_agent_run.size(); ++ k)
    {
        const int id = m_agent_run[k];
        float l_dis = m_agent[id].distance(po);
        if (l_dis < dis)
        {
            aid = id;
            dis = l_dis;
        }
    }

    l_agent = m_agent[aid];
    l_sample = m_sample[aid];
}

void Simulation::set_run_step(const bool flag)
{
    m_step_debug = flag;
    m_step_pause = true;
}

void Simulation::next_step()
{
    m_step_pause = false;
}
