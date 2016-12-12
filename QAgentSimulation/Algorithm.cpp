#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>

#include <QtGui>

#include "Algorithm.h"
#include "Utilities/IO.h"
#include "Utilities/Tool.h"

const int STEP = 50;
const int AGENT = 75000;
const int TIMESTAMP = 15000;

const int ROAD_POINT = 137434;
const int ROAD_INDEX = 311115;

const int DX[] = {-1, 0, 1, 0, -1, -1, 1, 1, 0};
const int DY[] = {0, 1, 0, -1, -1, 1, 1, -1, 0};

const int MIN_X = 558;
const int MAX_X = 10158;
const int MIN_Y = 59180;
const int MAX_Y = 64580;

const int WIDTH = MAX_X - MIN_X;
const int HEIGHT = MAX_Y - MIN_Y;

void Algorithm::build_city_grid_map()
{
    std::vector<int> l_index;
    std::vector<float> l_point;

    const int MIN_X = 500;
    const int MAX_X = 9500;
    const int MIN_Y = 59150;
    const int MAX_Y = 64600;
    const int WIDTH = MAX_X - MIN_X;
    const int HEIGHT = MAX_Y - MIN_Y;
    const float EPS = 1e-6;

    QImage l_city(WIDTH, HEIGHT, QImage::Format_RGB32);
    l_city.fill(QColor("white"));

    QPainter l_painter(&l_city);
    l_painter.setPen(Qt::NoPen);

    IO::load_triangle_data("./Data/river.dat", l_point, l_index);
    for (int k = 0; k < (int)l_index.size(); k += 3)
    {
        int l_a = l_index[k] * 3;
        int l_b = l_index[k + 1] * 3;
        int l_c = l_index[k + 2] * 3;
        float l_x0 = l_point[l_a] - MIN_X, l_y0 = l_point[l_a + 1] - MIN_Y;
        float l_x1 = l_point[l_b] - MIN_X, l_y1 = l_point[l_b + 1] - MIN_Y;
        float l_x2 = l_point[l_c] - MIN_X, l_y2 = l_point[l_c + 1] - MIN_Y;
        QPainterPath l_path;

        l_path.moveTo(l_x0, l_y0);
        l_path.lineTo(l_x1, l_y1);
        l_path.lineTo(l_x2, l_y2);
        l_path.lineTo(l_x0, l_y0);

        l_painter.fillPath(l_path, QBrush(QColor(52, 152, 219)));
    }

    l_point.clear();
    l_index.clear();
    IO::load_triangle_data("./Data/shelter.dat", l_point, l_index);
    for (int k = 0; k < (int)l_index.size(); k += 3)
    {
        int l_a = l_index[k] * 3;
        int l_b = l_index[k + 1] * 3;
        int l_c = l_index[k + 2] * 3;
        float l_x0 = l_point[l_a] - MIN_X, l_y0 = l_point[l_a + 1] - MIN_Y;
        float l_x1 = l_point[l_b] - MIN_X, l_y1 = l_point[l_b + 1] - MIN_Y;
        float l_x2 = l_point[l_c] - MIN_X, l_y2 = l_point[l_c + 1] - MIN_Y;
        QPainterPath l_path;

        l_path.moveTo(l_x0, l_y0);
        l_path.lineTo(l_x1, l_y1);
        l_path.lineTo(l_x2, l_y2);
        l_path.lineTo(l_x0, l_y0);

        l_painter.fillPath(l_path, QBrush(QColor(35, 155, 86)));
    }

    l_point.clear();
    l_index.clear();
    IO::load_triangle_data("./Data/city.dat", l_point, l_index);
    for (int k = 0; k < (int)l_index.size(); k += 3)
    {
        int l_a = l_index[k] * 3;
        int l_b = l_index[k + 1] * 3;
        int l_c = l_index[k + 2] * 3;
        float l_x0 = l_point[l_a] - MIN_X, l_y0 = l_point[l_a + 1] - MIN_Y, l_z0 = l_point[l_a + 2];
        float l_x1 = l_point[l_b] - MIN_X, l_y1 = l_point[l_b + 1] - MIN_Y, l_z1 = l_point[l_b + 2];
        float l_x2 = l_point[l_c] - MIN_X, l_y2 = l_point[l_c + 1] - MIN_Y, l_z2 = l_point[l_c + 2];
        if (EPS < l_z0 && EPS < l_z1 && EPS < l_z2)
        {
            QPainterPath l_path;

            l_path.moveTo(l_x0, l_y0);
            l_path.lineTo(l_x1, l_y1);
            l_path.lineTo(l_x2, l_y2);
            l_path.lineTo(l_x0, l_y0);

            l_painter.fillPath(l_path, QBrush(QColor(46, 64, 83)));
        }
    }
    l_city.save("./Data/city_grid_map.png", "PNG", 0);

}

void Algorithm::build_city_damage_map()
{
    std::vector<int> x_coord, y_coord;
    std::vector<std::vector<char>> l_data;

    QImage l_damage(WIDTH, HEIGHT, QImage::Format_RGB32);
    l_damage.fill(QColor("white"));

    QPainter l_painter(&l_damage);

    std::string timestamp = "0";

    for (char c = '0'; c <= '9'; ++ c)
    {
        timestamp[0] = c;
        std::string l_name = "K11_V19Dam4_" + timestamp;

        l_data.clear();
        x_coord.clear();
        y_coord.clear();
        IO::load_damage("E:/MAS Data/Damages/" + l_name + ".vtk", x_coord, y_coord, l_data);

        std::string txt_path = std::string("./Data/city_damage_map_") + c + ".txt";
        IO::save_damage_data(txt_path, l_data);

        std::string dat_path = std::string("./Data/city_damage_map_") + c + ".dat";
        IO::save_damage_data(dat_path, l_data, true);

        printf("Painting image %s ... \n", l_name.c_str());
        for (int i = 0; i < WIDTH; ++ i)
        {
            if (i > 0 && i % 500 == 0) printf("%.2lf%% ...\n", i * 100.0 / WIDTH);
            for (int j = 0; j < HEIGHT; ++ j)
            {
                if (l_data[i][j] == -1)
                {
                    l_painter.setPen(QColor(29, 131, 72));
                    l_painter.drawPoint(i, j);
                }
                else if (l_data[i][j] == 1)
                {
                    l_painter.setPen(QColor(86, 101, 115));
                    l_painter.drawPoint(i, j);
                }
            }
        }
        QString l_save_path = QString("./Data/city_damage_map_%1.png").arg(timestamp[0]);
        l_damage.save(l_save_path, "PNG", 0);
        printf("Saving image %s ...\n", l_name.c_str());
    }
}

void Algorithm::find_the_main_road_by_data_road_information()
{
    // 根据时间步拼接文件名字
    std::string path = "E:/MAS Data/Graph/0.Graph/Graph/road_usage_0_";
    char timestamp[16];
    int first_file_timestamp = 0;
    sprintf(timestamp, "%07d", first_file_timestamp);

    std::string name = path + std::string(timestamp) + ".vtk";

    // 获取节点以及相应的道路端点(所有时间步的数据都一样，获取一次)
    std::vector<int> index;
    std::vector<float> point;
    IO::load_graph_data(name, point, index);

    IO::save_line_data("./Data/road/road.txt", point, index);
    IO::save_line_data("./Data/road/road.dat", point, index, true);

    int point_size = (int)point.size() / 3;
    int index_size = (int)index.size() >> 1;

    // 获取不随时间步变化的节点和单元属相
    std::vector<float> exits;
    IO::load_graph_attribute(name, "exits", exits);
    std::vector<float> weights;
    IO::load_graph_attribute(name, "weights", weights);
    std::vector<float> num_agents_passed;
    IO::load_graph_attribute(name, "num_agents_passed", num_agents_passed);

    // 获取每个时间步的道路人员经过数量
    std::vector<float> stamp_num_agents_passed;
    std::vector<float> max_agent_passed(index_size, 0.0);
    for (int idx = 0; idx < TIMESTAMP; idx += 50)
    {
        sprintf(timestamp, "%07d", idx);
        std::string l_name = path + std::string(timestamp) + ".vtk";
        IO::load_graph_attribute(l_name, "num_agents_passed", stamp_num_agents_passed);
        for (int i = 0; i < index_size; ++ i) max_agent_passed[i] = std::max(max_agent_passed[i], stamp_num_agents_passed[i]);
    }

    // 获取道路权重的阈值
    const float weight_threhold = 5.0;
    const float num_agents_passed_threhold = 2.0;
    const float EPS = 0.5;

    // 根据阈值进行挑选从所有道路中
    std::vector<int> main_road_index;
    for (int i = 0; i < index_size; ++ i)
    {
        // 过滤出口的道路
        int l_a = index[i << 1];
        int l_b = index[i << 1 | 1];

        if (exits[l_a] > EPS || exits[l_b] > EPS) continue;

        // 过滤合适的道路
        if (weights[i] >= weight_threhold && num_agents_passed[i] >= max_agent_passed[i])
        {
            main_road_index.push_back(l_a);
            main_road_index.push_back(l_b);
        }
    }
    IO::save_line_data("./Data/road/main_road.txt", point, main_road_index);
    IO::save_line_data("./Data/road/main_road.dat", point, main_road_index, true);
}

void Algorithm::find_the_main_road_by_agent_movement()
{
    std::string city_map_path = "./Data/damage/city_damage_map_0.dat";
    std::vector<std::vector<char>> city_map;

    // load city map data (-1 for shelter, 0 for road and 1 for obstacle
    bool io_status = IO::load_city_map(city_map_path, city_map);

    if (! io_status)
    {
        printf("Load %s failed !\n", city_map_path.c_str());
        return;
    }

    // calulate agent movement heat map data
    std::vector<std::vector<float>> heat_map(WIDTH, std::vector<float>(HEIGHT, 0));
    get_agent_heat_map(city_map, heat_map);

    /*
    // get cross road position by the heat map, it is very difficult
    std::vector<std::vector<float>> road_node(WIDTH, std::vector<float>(HEIGHT, 0));
    get_main_road_node(heat_map, road_node);
    IO::save_city_map_data("./Data/main_road_node.png", city_map, road_node);
    */

    // filter the original road network by the heat map
    filter_main_road_by_heat_map(heat_map);
}

void Algorithm::get_agent_movement(std::vector<std::vector<float>> & agent)
{
    // 根据时间步拼接文件名字
    std::string path = "./Data/agent/timestep0_";

    char timestamp[16];

    std::vector<int> ids;
    std::vector<float> points;
    for (int t = 0; t < TIMESTAMP; t += STEP)
    {
        sprintf(timestamp, "%07d", t);
        std::string in_path = path + std::string(timestamp) + ".dat";

        IO::load_agent_data(in_path, points, ids, true);
        for (int i = 0; i < (int)ids.size(); ++ i)
        {
            int l_id = ids[i];
            float l_x = points[i * 3];
            float l_y = points[i * 3 + 1];
            agent[l_id].push_back(l_x);
            agent[l_id].push_back(l_y);
        }
    }
}

void Algorithm::get_agent_heat_map(const std::vector<std::vector<char>> & city_map, std::vector<std::vector<float>> & heat_map)
{
    std::string agent_heat_map_path = "./Data/agent_heat_map.png";
    std::string agent_heat_grid_path = "./Data/agent_heat_data.dat";
    std::string agent_heat_grid_path_txt = "./Data/agent_heat_data.txt";

    bool io_status;

    // if aready calculate the heat data, load it, else calculate it
    if (IO::file_exist(agent_heat_grid_path))
    {
        io_status = IO::load_city_grid_data(agent_heat_grid_path, heat_map, true);
        if (! io_status)
        {
            printf("Load %s failed !\n", agent_heat_grid_path.c_str());
            return;
        }
    }
    else
    {
        std::vector<std::vector<float>> agent(AGENT);
        std::string all_agent_path = "./Data/agent/agent_all.dat";
        if (IO::file_exist(all_agent_path))
        {
            io_status = IO::load_agent_path(all_agent_path, agent, true);
            if (! io_status)
            {
                printf("Load %s failed !\n", all_agent_path.c_str());
                return;
            }
        }
        else
        {
            get_agent_movement(agent);
        }

        const int MIN_DIS_THREHOLD = 10;

        // for every agent, accumulate the heat of his path point and around 8 neibor points
        for (int i = 0; i < (int)agent.size(); ++ i)
        {
            int l_last_x = 0, l_last_y = 0;
            for (int j = 0; j < (int)agent[i].size(); j += 2)
            {
                int l_x = (int)agent[i][j] - MIN_X;
                int l_y = (int)agent[i][j + 1] - MIN_Y;

                int dis = std::abs(l_x - l_last_x)  + std::abs(l_y - l_last_y);

                l_last_x = l_x;
                l_last_y = l_y;

                if (dis < MIN_DIS_THREHOLD) continue;

                for (int k = 0; k < 9; ++ k)
                {
                    int l_a = l_x + DX[k];
                    int l_b = l_y + DY[k];
                    if (0 <= l_a && l_a < WIDTH && 0 <= l_b && l_b < HEIGHT && city_map[l_a][l_b] == 0)
                    {
                        heat_map[l_a][l_b] += 1.0;
                    }
                }
            }
        }

        IO::save_city_map_data(agent_heat_map_path, city_map, heat_map);
        IO::save_city_grid_data(agent_heat_grid_path_txt, heat_map);
        IO::save_city_grid_data(agent_heat_grid_path, heat_map, true);
    }
}

void Algorithm::get_main_road_node(const std::vector<std::vector<float>> & heat_map, std::vector<std::vector<float>> & road_node)
{
    printf("calculate main road node ...\n");

    // get the statistic information
    int count = 0;
    float sum_heat = 0.0;
    for (int i = 0; i < WIDTH; ++ i)
    {
        for (int j = 0; j < HEIGHT; ++ j)
        {
            if (heat_map[i][j] > 0.0)
            {
                ++ count;
                sum_heat += heat_map[i][j];
            }
        }
    }

    // set the threhold value for main road node
    const float average = sum_heat / count;
    const float heat_threhold = average * 5;
    const float box_heat_threhold = average * 2;
    const int distance_threhold = 40;               // one side distance
    const int box_threhold = 20;                    // radius of box
    const int valid_node_threhold = 10;


    const int neibor_width = WIDTH / distance_threhold;
    const int neibor_height = HEIGHT / distance_threhold;
    std::vector<std::vector<std::pair<int, int>>> neibors(neibor_width, std::vector<std::pair<int, int>>(neibor_height));

    int l_step = (WIDTH + IO::PROCESS_STEP - 1) / IO::PROCESS_STEP;
    for (int i = 0; i < WIDTH; ++ i)
    {
        if (i % l_step == 0) printf("#");
        for (int j = 0; j < HEIGHT; ++ j)
        {
            // filter high heat position
            if (heat_map[i][j] > heat_threhold)
            {
                // check for neibors, if there is already a node near, discard the point
                bool has_neibor = false;
                int l_px = i / distance_threhold;
                int l_py = j / distance_threhold;
                for (int k = 0; k < 9; ++ k)
                {
                    int l_pa = l_px + DX[k];
                    int l_pb = l_py + DY[k];
                    if (l_pa >= 0 && l_pa < neibor_width && l_pb >= 0 && l_pb < neibor_height)
                    {
                        float l_dis = std::abs(i - neibors[l_pa][l_pb].first) + std::abs(j - neibors[l_pa][l_pb].second);
                        if (l_dis < (distance_threhold << 1))
                        {
                            has_neibor = true;
                            break;
                        }
                    }
                }

                if (has_neibor) continue;

                int diff_count = 0;
                for (int k = -box_threhold; k <= box_threhold; ++ k)
                {
                    int up = -1, down = -1; 
                    if (j - box_threhold >= 0 && i + k >= 0 && i + k < WIDTH) up = heat_map[i + k][j - box_threhold] > box_heat_threhold;
                    if (j + box_threhold < HEIGHT && i - k >= 0 && i - k < WIDTH) down = heat_map[i - k][j + box_threhold] > box_heat_threhold;
                    if (up != -1 && down != -1) diff_count += up ^ down;

                    int left = -1, right = -1;
                    if (i - box_threhold >= 0 && j + k >= 0 && j + k < HEIGHT) left = heat_map[i - box_threhold][j + k] > box_heat_threhold;
                    if (i + box_heat_threhold < WIDTH && j - k >= 0 && j - k < HEIGHT) right = heat_map[i + box_threhold][j - k] > box_heat_threhold;
                    if (left != -1 && right != -1) diff_count += left ^ down;
                }
                if (diff_count > valid_node_threhold) neibors[l_px][l_py] = std::make_pair(i, j);
                road_node[i][j] = diff_count;
            }
        }
    }
    printf("\n");
}

void Algorithm::filter_main_road_by_heat_map(const std::vector<std::vector<float>> & heat_map)
{
    const std::string road_path = "./Data/road/road.dat";
    const std::string main_road_path = "./Data/road/main_road_by_heat";

    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    // get the statistic information
    int count = 0;
    float sum_heat = 0.0;
    for (int i = 0; i < WIDTH; ++ i)
    {
        for (int j = 0; j < HEIGHT; ++ j)
        {
            if (heat_map[i][j] > 0.0)
            {
                ++ count;
                sum_heat += heat_map[i][j];
            }
        }
    }

    // set the threhold value for main road node
    const int neibor_distance = 12;
    const float average = sum_heat / count;
    const float heat_threhold = average * 1.0;

    std::vector<int> main_road_index;
    for (int i = 0; i < (int)index.size(); i += 2)
    {
        int l_a = index[i], l_b = index[i + 1];

        int l_x0 = point[l_a * 3] - MIN_X, l_y0 = point[l_a * 3 + 1] - MIN_Y;
        int l_x1 = point[l_b * 3] - MIN_X, l_y1 = point[l_b * 3 + 1] - MIN_Y;

        int value_0 = get_neibor_heat(l_x0, l_y0, neibor_distance, heat_map, 0);
        int value_1 = get_neibor_heat(l_x1, l_y1, neibor_distance, heat_map, 0);

        if (value_0 > heat_threhold && value_1 > heat_threhold)
        {
            main_road_index.push_back(l_a);
            main_road_index.push_back(l_b);
        }
    }

    IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);
}

float Algorithm::get_neibor_heat(const int x, const int y, const int w,
    const std::vector<std::vector<float>> & heat_map, const int mode)
{
    float ret = 0.0;
    for (int i = -w; i <= w; ++ i)
    {
        for (int j = -w; j <= w; ++ j)
        {
            int a = x + i, b = y + j;
            if (a < 0 || a >= WIDTH || b < 0 || b >= HEIGHT) continue;

            if (mode == 0)
            {
                ret = std::max(ret, heat_map[a][b]);
            }
            else if (mode == 1)
            {
                ret += heat_map[a][b];
            }
        }
    }
    return ret;
}

void Algorithm::calculate_road_statistic_information()
{
    const std::string road_path = "./Data/road/road.dat";
    const std::string main_road_path = "./Data/road/main_road.dat";
    const std::string road_vtk_path = "E:/MAS Data/Graph/0.Graph/Graph/road_usage_0_0000000.vtk";
    const std::string heat_map_path = "./Data/agent_heat_data.dat";

    // 获取全部的道路的顶点和连接信息
    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    // 获取初步获取的主道路道路的顶点和连接信息
    std::vector<int> main_road_index;
    std::vector<float> main_road_point;
    IO::load_line_data(main_road_path, main_road_point, main_road_index);

    // 记录哪些是主要道路
    std::set<std::pair<int, int>> main_road_set;
    for (int i = 0; i < (int)main_road_index.size(); i += 2)
    {
        int a = main_road_index[i], b = main_road_index[i + 1];
        main_road_set.insert(std::make_pair(a, b));
    }

    // 从vtk文件中获取节点是否为逃生区域信息
    std::vector<float> exits;
    IO::load_graph_attribute(road_vtk_path, "exits", exits);

    // 从与处理数据中导入heatmap数据
    std::vector<std::vector<float>> heat_map;
    IO::load_city_grid_data(heat_map_path, heat_map, true);

    // 控制heatmap中每个像素点需要累积的patch的半径
    const int box_threhold = 5;

    std::vector<float> dist;
    std::vector<float> weight;
    std::vector<float> is_main_road;

    for (int i = 0; i < (int)index.size(); i += 2)
    {
        int a = index[i], b = index[i + 1];
        int l_x0 = point[a * 3] - MIN_X, l_y0 = point[a * 3 + 1] - MIN_Y;
        int l_x1 = point[b * 3] - MIN_X, l_y1 = point[b * 3 + 1] - MIN_Y;

        // 获取该边的长度
        float dis = std::abs(l_x0 - l_x1) + std::abs(l_y0 - l_y1);
        dist.push_back(dis);

        // 获取盖条路径上的heat值作为权重
        int x_interval = std::abs(l_x0 - l_x1);
        int y_interval = std::abs(l_y0 - l_y1);
        if (x_interval == 0 && y_interval == 0)
        {
            weight.push_back(0.0);
            is_main_road.push_back(0.0);
            continue;
        }

        float dx, dy;
        if (x_interval != 0)
        {
            dx = (l_x1 - l_x0) / x_interval;
            dy = (l_y1 - l_y0) / x_interval;
        }
        else
        {
            dx = (l_x1 - l_x0) / y_interval;
            dy = (l_y1 - l_y0) / y_interval;
        }

        float x = l_x0, y = l_y0, wei = 0.0;
        do
        {
            float heat = get_neibor_heat((int)x, (int)y, box_threhold, heat_map, 1);
            wei += heat / (box_threhold * box_threhold);
            x += dx, y += dy;
        } while (Tool::sgn(x - l_x1) != 0 && Tool::sgn(y - l_y1) != 0);

        weight.push_back(wei / dis);
        is_main_road.push_back(main_road_set.find(std::make_pair(a, b)) != main_road_set.end());
    }

    const std::string attribute_exits_path = "./Data/road/attribute_exits";
    IO::save_road_attribute_data(attribute_exits_path + ".txt", exits);
    IO::save_road_attribute_data(attribute_exits_path + ".dat", exits, true);

    const std::string attribute_distance_path = "./Data/road/attribute_distance";
    IO::save_road_attribute_data(attribute_distance_path + ".txt", dist);
    IO::save_road_attribute_data(attribute_distance_path + ".dat", dist, true);

    const std::string attribute_weight_path = "./Data/road/attribute_weight";
    IO::save_road_attribute_data(attribute_weight_path + ".txt", weight);
    IO::save_road_attribute_data(attribute_weight_path + ".dat", weight, true);

    const std::string attribute_is_main_road_path = "./Data/road/attribute_is_main_road";
    IO::save_road_attribute_data(attribute_is_main_road_path + ".txt", is_main_road);
    IO::save_road_attribute_data(attribute_is_main_road_path + ".dat", is_main_road, true);
}

void Algorithm::filter_main_road_by_road_weight_and_heat()
{
    const std::string road_path = "./Data/road/road.dat";
    const std::string main_road_path = "./Data/road/main_road";
    const std::string attribute_weight_path = "./Data/road/attribute_weight.dat";
    const std::string attribute_distance_path = "./Data/road/attribute_distance.dat";
    const std::string attribute_is_main_path = "./Data/road/attribute_is_main_road.dat";

    // 获取完整的道路的顶点和连接信息
    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    int index_size = (int)index.size() >> 1;

    std::vector<float> weight;
    IO::load_road_attribute_data(attribute_weight_path, weight, true);

    std::vector<float> dist;
    IO::load_road_attribute_data(attribute_distance_path, dist, true);

    std::vector<float> is_main;
    IO::load_road_attribute_data(attribute_is_main_path, is_main, true);

    const float weight_threhold = 1.0;
    const float distance_threhold = 1.0;
    std::vector<int> main_road_index;
    for (int i = 0; i < index_size; ++ i)
    {
        int a = index[i << 1];
        int b = index[i << 1 | 1];
        if (is_main[i] && weight[i] > weight_threhold && dist[i] > distance_threhold)
        {
            main_road_index.push_back(a);
            main_road_index.push_back(b);
        }
    }

    IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);
}

void Algorithm::connect_main_road_network()
{
    const std::string road_path = "./Data/road/road.dat";
    const std::string attribute_exits_path = "./Data/road/attribute_exits.dat";
    const std::string attribute_distance_path = "./Data/road/attribute_distance.dat";
    const std::string attribute_weight_path = "./Data/road/attribute_weight.dat";
    const std::string attribute_is_main_road_path = "./Data/road/attribute_is_main_road.dat";

    // 获取完整的的道路的顶点和连接信息
    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    int point_size = (int)point.size() / 3;
    int index_size = (int)index.size() >> 1;

    // 导入完整道路的属性信息
    std::vector<float> exits;
    IO::load_road_attribute_data(attribute_exits_path, exits, true);

    std::vector<float> dist;
    IO::load_road_attribute_data(attribute_distance_path, dist, true);

    std::vector<float> weight;
    IO::load_road_attribute_data(attribute_weight_path, weight, true);

    std::vector<float> is_main_road;
    IO::load_road_attribute_data(attribute_is_main_road_path, is_main_road, true);

    const float weight_threhold = 0.3;

    // 构建Graph的中间结果，每个vector表示一个点，记录与该点相连的边信息
    std::vector<std::vector<int>> edge_connect_to_points(point_size);
    for (int i = 0; i < index_size; ++ i)
    {
        int a = index[i << 1];
        int b = index[i << 1 | 1];
        //if (weight[i] < weight_threhold) continue;
        edge_connect_to_points[a].push_back(i);
        edge_connect_to_points[b].push_back(i);
    }

    // 构建城市道路图city_graph，其中的节点是每条道路，边是道路的连接情况
    int edge_size = 0;
    std::vector<std::vector<int>> city_graph(index_size);
    for (int k = 0; k < point_size; ++ k)
    {
        int n = (int)edge_connect_to_points[k].size();
        for (int i = 0; i < n; ++ i)
        {
            for (int j = i + 1; j < n; ++ j)
            {
                int a = edge_connect_to_points[k][i];
                int b = edge_connect_to_points[k][j];
                city_graph[a].push_back(b);
                city_graph[b].push_back(a);
                ++ edge_size;
            }
        }
    }
    printf("Graph has %d edges\n", edge_size);

    const int connect_threhold = 0.95;
    int main_road_count = 0, now_connect = 0;
    for (int i = 0; i < index_size; ++ i)
    {
        if (is_main_road[i]) ++ main_road_count;
    }

    std::vector<int> connect_index;


    // 寻找在shelter区域的起始点，做bfs，直到大部分的主要道路被连接到shelter上
    std::vector<bool> is_connect(index_size, false);
}

void Algorithm::map_city_grid_and_road()
{
    // 导入city map数据(-1 for shelter, 0 for road and 1 for obstacle)
    const std::string city_map_path = "./Data/damage/city_damage_map_0.dat";
    std::vector<std::vector<char>> city_map;

    bool io_status = IO::load_city_map(city_map_path, city_map);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_map_path.c_str());
        return;
    }

    // 导入整体的道路数据，获取全部的道路的顶点和连接信息
    const std::string road_path = "./Data/road/road.dat";
    std::vector<int> index;
    std::vector<float> point;

    io_status = IO::load_line_data(road_path, point, index);
    if (! io_status)
    {
        printf("Load %s failed !\n", road_path.c_str());
        return;
    }

    std::vector<std::vector<float>> city_grid_road_map(WIDTH, std::vector<float>(HEIGHT, -1.0));
    for (int i = 0; i < (int)index.size(); i += 2)
    {
        // 获取道路的两个端点
        int a = index[i], b = index[i + 1];
        float l_x0 = point[a * 3] - MIN_X, l_y0 = point[a * 3 + 1] - MIN_Y;
        float l_x1 = point[b * 3] - MIN_X, l_y1 = point[b * 3 + 1] - MIN_Y;

        // 遍历这条道路的所有像素
        int x_interval = std::abs(l_x0 - l_x1);
        int y_interval = std::abs(l_y0 - l_y1);

        float dx, dy;
        if (x_interval != 0)
        {
            dx = (l_x1 - l_x0) / x_interval;
            dy = (l_y1 - l_y0) / x_interval;
        }
        else if (y_interval != 0)
        {
            dx = (l_x1 - l_x0) / y_interval;
            dy = (l_y1 - l_y0) / y_interval;
        }
        else
        {
            dx = dy = 0.0;
        }

        float x = l_x0, y = l_y0;
        do
        {
            int px = (int)x, py = (int)y;
            if (city_map[px][py] != 1 && city_grid_road_map[px][py] < 0)
            {
                city_grid_road_map[px][py] = (i >> 1);
            }
            x += dx, y += dy;
        } while (Tool::sgn(x - l_x1) != 0 && Tool::sgn(y - l_y1) != 0);
    }

    const std::string grid_road_map_path = "./Data/road/grid_road_map";
    IO::save_city_grid_data(grid_road_map_path + ".txt", city_grid_road_map);
    IO::save_city_grid_data(grid_road_map_path + ".dat", city_grid_road_map, true);
}

void Algorithm::calculate_road_weight()
{
    // 获取计算得到的道路对应地图网格数据
    const std::string grid_road_map_path = "./Data/road/grid_road_map.dat";
    std::vector<std::vector<float>> city_grid_road_map;

    bool io_status = IO::load_city_grid_data(grid_road_map_path, city_grid_road_map, true);
    if (! io_status)
    {
        printf("Load %s failed !\n", grid_road_map_path.c_str());
        return;
    }

    // 获取所有agent的移动点信息
    const std::string all_agent_path = "./Data/agent/agent_all.dat";
    std::vector<std::vector<float>> agent(AGENT);

    io_status = IO::load_agent_path(all_agent_path, agent, true);
    if (! io_status)
    {
        printf("Load %s failed !\n", all_agent_path.c_str());
        return;
    }

    /***************************************************************
    ** 将这些agent的移动映射到所有的道路上
    ***************************************************************/
    const int neibor_threhold = 10;
    std::vector<float> road_weight(ROAD_INDEX, 0.0);

    // 确保每个agent对每条道路只产生一次影响
    printf("Process agent path ...\n");
    int l_step = (agent.size() + IO::PROCESS_STEP - 1) / IO::PROCESS_STEP;

    // 统计出agent的平均位移步长为6.93，小于15的占99.4%，小于10的占72.4%
    //const double average_agent_step_distance = 6.93;

    std::set<int> neibor;
    std::vector<int> flag(ROAD_INDEX, -1);
    for (int k = 0; k < (int)agent.size(); ++ k)
    {
        if (k % l_step == 0) printf("#");
        for (int i = 0; i < (int)agent[k].size(); i += 2)
        {
            int x = agent[k][i] - MIN_X;
            int y = agent[k][i + 1] - MIN_Y;

            neibor.clear();
            for (int dx = -neibor_threhold; dx <= neibor_threhold; ++ dx)
            {
                for (int dy = -neibor_threhold; dy <= neibor_threhold; ++ dy)
                {
                    int a = x + dx, b = y + dy;
                    if (0 <= a && a < WIDTH && 0 <= b && b < HEIGHT && city_grid_road_map[a][b] > -0.5)
                    {
                        neibor.insert(int(city_grid_road_map[a][b]));
                    }
                }
            }

            for (auto it = neibor.begin(); it != neibor.end(); ++ it)
            {
                if (flag[*it] != k)
                {
                    flag[*it] = k;
                    road_weight[*it] += 1.0;
                }
            }
        }
    }
    printf("\n");

    // 保存得到的weight信息
    const std::string attribute_weight_path = "./Data/road/attribute_weight";
    IO::save_road_attribute_data(attribute_weight_path + ".txt", road_weight);
    IO::save_road_attribute_data(attribute_weight_path + ".dat", road_weight, true);
}

void Algorithm::filter_main_road_by_road_weight()
{
    const std::string road_path = "./Data/road/road.dat";
    const std::string attribute_weight_path = "./Data/road/attribute_weight.dat";

    const std::string main_road_path = "./Data/road/main_road";
    const std::string attribute_is_main_path = "./Data/road/attribute_is_main_road";

    // 获取完整的道路的顶点和连接信息
    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    int index_size = (int)index.size() >> 1;

    // 导入计算好的weight信息
    std::vector<float> weight;
    IO::load_road_attribute_data(attribute_weight_path, weight, true);

    const float weight_threhold = 50.0;
    std::vector<int> main_road_index;
    std::vector<float> is_main_road(index_size, 0.0);
    for (int k = 0; k < index_size; ++ k)
    {
        int a = index[k << 1];
        int b = index[k << 1 | 1];
        if (weight[k] > weight_threhold)
        {
            is_main_road[k] = 1.0;
            main_road_index.push_back(a);
            main_road_index.push_back(b);
        }
    }

    IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);

    IO::save_road_attribute_data(attribute_is_main_path + ".txt", is_main_road);
    IO::save_road_attribute_data(attribute_is_main_path + ".dat", is_main_road, true);
} 

