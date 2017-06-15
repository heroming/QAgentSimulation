#include <QtGui>

#include "Algorithm.h"
#include "Simulation.h"
#include "Utilities/IO.h"
#include "Utilities/Tool.h"

const int STEP = 50;
const int AGENT = 75000;
const int TIMESTAMP = 15000;

const int ROAD_POINT = 137434;
const int ROAD_INDEX = 311115;

const int MIN_X = 558;
const int MAX_X = 10158;
const int CITY_MAX_X = 9360;
const int MIN_Y = 59180;
const int MAX_Y = 64580;

const int WIDTH = MAX_X - MIN_X;
const int CITY_WIDTH = CITY_MAX_X - MIN_X;
const int HEIGHT = MAX_Y - MIN_Y;

int front, tail;
int que[WIDTH * HEIGHT];
bool inq[WIDTH * HEIGHT];

void Algorithm::build_city_grid_map()
{
    std::vector<int> l_index;
    std::vector<float> l_point;

    const float EPS = 1e-6;

    QImage l_city(WIDTH, HEIGHT, QImage::Format_RGB32);
    l_city.fill(QColor("white"));

    QPainter l_painter(&l_city);
    l_painter.setPen(Qt::NoPen);

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

    l_point.clear();
    l_index.clear();
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

    // city grid数据(-1 for shelter, 0 for road and 1 for obstacle)
    std::vector<std::vector<char>> city_grid(WIDTH, std::vector<char>(HEIGHT, 0));
    for (int i = 0; i < WIDTH; ++ i)
        for (int j = 0; j < HEIGHT; ++ j)
        {
            QRgb c = l_city.pixel(i, j);
            if ((c & 0xF) == 6)             // shelter
            {
                city_grid[i][j] = -1;
            }
            else if ((c & 0xF) == 3)        // city
            {
                city_grid[i][j] = 1;
            }
            else if ((c & 0xF) == 11)       // river
            {
                city_grid[i][j] = 2;
            }
        }

    l_city.save("./Data/city_grid_map.png", "PNG", 0);

    const std::string city_grid_path = "./Data/city_grid";
    IO::save_damage_data(city_grid_path + ".txt", city_grid);
    IO::save_damage_data(city_grid_path + ".dat", city_grid, true);

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


/******************************************************************************
** 1 : 通过道路流量信息
*******************************************************************************/
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

    /*
    std::vector<int> main_road_index_0;
    const float num_agents_passed_threhold_0 = 4.0;
    std::vector<int> main_road_index_1;
    const float num_agents_passed_threhold_1 = 6.0;
    std::vector<int> main_road_index_2;
    const float num_agents_passed_threhold_2 = 8.0;
    std::vector<int> main_road_index_3;
    const float num_agents_passed_threhold_3 = 10.0;
    std::vector<int> main_road_index_4;
    const float num_agents_passed_threhold_4 = 15.0;
    std::vector<int> main_road_index_5;
    const float num_agents_passed_threhold_5 = 20.0;
    std::vector<int> main_road_index_6;
    const float num_agents_passed_threhold_6 = 30.0;
    std::vector<int> main_road_index_7;
    const float num_agents_passed_threhold_7 = 50.0;
    std::vector<int> main_road_index_8;
    const float num_agents_passed_threhold_8 = 100.0;
    */
    for (int i = 0; i < index_size; ++ i)
    {
        // 过滤出口的道路
        int l_a = index[i << 1];
        int l_b = index[i << 1 | 1];

        if (exits[l_a] > EPS || exits[l_b] > EPS) continue;

        // 过滤合适的道路
        //if (weights[i] >= weight_threhold && num_agents_passed[i] >= max_agent_passed[i])
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold)
        {
            main_road_index.push_back(l_a);
            main_road_index.push_back(l_b);
        }
        /*
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_0)
        {
            main_road_index_0.push_back(l_a);
            main_road_index_0.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_1)
        {
            main_road_index_1.push_back(l_a);
            main_road_index_1.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_2)
        {
            main_road_index_2.push_back(l_a);
            main_road_index_2.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_3)
        {
            main_road_index_3.push_back(l_a);
            main_road_index_3.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_4)
        {
            main_road_index_4.push_back(l_a);
            main_road_index_4.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_5)
        {
            main_road_index_5.push_back(l_a);
            main_road_index_5.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_6)
        {
            main_road_index_6.push_back(l_a);
            main_road_index_6.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_7)
        {
            main_road_index_7.push_back(l_a);
            main_road_index_7.push_back(l_b);
        }
        if (weights[i] >= weight_threhold && max_agent_passed[i] >= num_agents_passed_threhold_8)
        {
            main_road_index_8.push_back(l_a);
            main_road_index_8.push_back(l_b);
        }
    */
    }
    IO::save_line_data("./Data/road/main_road_net.txt", point, main_road_index);
    IO::save_line_data("./Data/road/main_road_net.dat", point, main_road_index, true);
    /*
    IO::save_line_data("./Data/road/main_road_net_2.txt", point, main_road_index);
    IO::save_line_data("./Data/road/main_road_net_2.dat", point, main_road_index, true);
    IO::save_line_data("./Data/road/main_road_net_4.txt", point, main_road_index_0);
    IO::save_line_data("./Data/road/main_road_net_4.dat", point, main_road_index_0, true);
    IO::save_line_data("./Data/road/main_road_net_6.txt", point, main_road_index_1);
    IO::save_line_data("./Data/road/main_road_net_6.dat", point, main_road_index_1, true);
    IO::save_line_data("./Data/road/main_road_net_8.txt", point, main_road_index_2);
    IO::save_line_data("./Data/road/main_road_net_8.dat", point, main_road_index_2, true);
    IO::save_line_data("./Data/road/main_road_net_10.txt", point, main_road_index_3);
    IO::save_line_data("./Data/road/main_road_net_10.dat", point, main_road_index_3, true);
    IO::save_line_data("./Data/road/main_road_net_15.txt", point, main_road_index_4);
    IO::save_line_data("./Data/road/main_road_net_15.dat", point, main_road_index_4, true);
    IO::save_line_data("./Data/road/main_road_net_20.txt", point, main_road_index_5);
    IO::save_line_data("./Data/road/main_road_net_20.dat", point, main_road_index_5, true);
    IO::save_line_data("./Data/road/main_road_net_30.txt", point, main_road_index_6);
    IO::save_line_data("./Data/road/main_road_net_30.dat", point, main_road_index_6, true);
    IO::save_line_data("./Data/road/main_road_net_50.txt", point, main_road_index_7);
    IO::save_line_data("./Data/road/main_road_net_50.dat", point, main_road_index_7, true);
    IO::save_line_data("./Data/road/main_road_net_100.txt", point, main_road_index_8);
    IO::save_line_data("./Data/road/main_road_net_100.dat", point, main_road_index_8, true);
    */
}


/******************************************************************************
** 2 : 通过各个时间步的人员移动信息构建heat map，在计算道路的heat值，用于过滤主道路
*******************************************************************************/
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
    std::string path = "./Data/agent/run0/timestep0_";

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
        std::string all_agent_path = "./Data/agent/agent_all_0.dat";
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
    const std::string main_road_path = "./Data/road/main_road_heat";

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

    /*
    const int batch_size = 10;
    const float heat_batch_threhold[batch_size] = {2.0, 5.0, 10.0, 15.0, 20.0, 30.0, 40.0, 50.0, 70.0, 100.0};
    const std::string heat_batch_path[batch_size] = {"2", "5", "10", "15", "20", "30", "40", "50", "70", "100"};
    std::vector<int> main_road_index_batch[batch_size];
    */

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
        /*
        for (int k = 0; k < batch_size; ++ k)
        {
            if (value_0 > heat_batch_threhold[k] && value_1 > heat_batch_threhold[k])
            {
                main_road_index_batch[k].push_back(l_a);
                main_road_index_batch[k].push_back(l_b);
            }
        }
        */
    }

    IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);
    /*
    for (int k = 0; k < batch_size; ++ k)
    {
        IO::save_line_data(main_road_path + "_" + heat_batch_path[k] + ".txt", point, main_road_index_batch[k]);
        IO::save_line_data(main_road_path + "_" + heat_batch_path[k] + ".dat", point, main_road_index_batch[k], true);
    }
    */
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



/******************************************************************************
** 3 : 根据道路的信息统计，路网构建 
*******************************************************************************/
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
    const std::string main_road_path = "./Data/road/main_road_heat_weight";
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

// 未完成...
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


/******************************************************************************
** 4 : 先将道路映射到网格中，在根据agent移动将heat值累积到道路上，用于过滤主道路
*******************************************************************************/
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

    // 将道路经过的像素填补在网格上
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

    const std::string main_road_path = "./Data/road/main_road_weight";
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

    /*
    const int batch_size = 10;
    const float weight_batch_threhold[batch_size] = {2.0, 5.0, 10.0, 15.0, 20.0, 30.0, 40.0, 50.0, 70.0, 100.0};
    const std::string weight_batch_path[batch_size] = {"2", "5", "10", "15", "20", "30", "40", "50", "70", "100"};
    std::vector<int> main_road_index_batch[batch_size];
    */

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

        /*
        for (int i = 0; i < batch_size; ++ i)
        {
            if (weight[k] > weight_batch_threhold[i])
            {
                main_road_index_batch[i].push_back(a);
                main_road_index_batch[i].push_back(b);
            }
        }
        */
    }

    IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);

    IO::save_road_attribute_data(attribute_is_main_path + ".txt", is_main_road);
    IO::save_road_attribute_data(attribute_is_main_path + ".dat", is_main_road, true);

    /*
    for (int k = 0; k < batch_size; ++ k)
    {
        IO::save_line_data(main_road_path + "_" + weight_batch_path[k] + ".txt", point, main_road_index_batch[k]);
        IO::save_line_data(main_road_path + "_" + weight_batch_path[k] + ".dat", point, main_road_index_batch[k], true);
    }
    */
} 


/******************************************************************************
** 5 : 直接计算道路的宽度，用于过滤主道路
*******************************************************************************/
bool Algorithm::is_valid_position(const std::vector<std::vector<char>> & grid, const int x, const int y, const bool actrual)
{
    if (grid.empty() || grid[0].empty()) return 0;
    if (actrual) return x >= 0 && x < CITY_WIDTH && x < (int)grid.size() && y >= 0 && y < (int)grid[0].size();
    return x >= 0 && x < (int)grid.size() && y >= 0 && y < (int)grid[0].size();
}

bool Algorithm::is_valid_edge(const std::vector<std::vector<char>> & grid, const int a, const int b, const int c, const int d)
{
    /*
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
    */
    return true;
}

int Algorithm::get_point_clearance(const std::vector<std::vector<char>> & grid,
    const int x, const int y, const bool actual, const bool river)
{
    static const int MIN_CLEARANCE = 1;
    static const int MAX_CLEARANCE = 32;
    int l = MIN_CLEARANCE, r = MAX_CLEARANCE;
    while (l <= r)
    {
        int m = (l + r) >> 1;
        if (check_for_clearance(grid, x, y, m, actual, river))
        {
            l = m + 1;
        }
        else
        {
            r = m - 1;
        }
    }
    return r;
}

bool Algorithm::check_for_clearance(const std::vector<std::vector<char>> & grid,
    const int x, const int y, const int r, const bool actrual, const bool river)
{
    for (int i = -r; i <= r; ++ i)
    {
        if (! is_valid_position(grid, x + i, y - r, actrual) || grid[x + i][y - r] == 1 || (river && grid[x + i][y - r] == 2)) return 0;
        if (! is_valid_position(grid, x + i, y + r, actrual) || grid[x + i][y + r] == 1 || (river && grid[x + i][y + r] == 2)) return 0;
        if (! is_valid_position(grid, x - r, y + i, actrual) || grid[x - r][y + i] == 1 || (river && grid[x - r][y + i] == 2)) return 0;
        if (! is_valid_position(grid, x + r, y + i, actrual) || grid[x + r][y + i] == 1 || (river && grid[x + r][y + i] == 2)) return 0;
    }
    return 1;
}

void Algorithm::map_road_end_point_to_city_map_with_clearance()
{
    // 导入city map数据(-1 for shelter, 0 for road and 1 for obstacle)
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
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

    // 记录点和边的条数
    const int number_point = (int)point.size() / 3;
    const int number_index = (int)index.size() >> 1;

    // 记录主要道路的端点
    const int clearance_threhold = 5;
    std::vector<int> main_road_index;

    // 将道路经过的像素填补在网格上
    printf("Calculate clearance information ...\n");
    std::vector<bool> is_main_road(number_index, false);

    /*
    const int batch_size = 10;
    const int clearance_batch_threhold[batch_size] = {2, 3, 4, 5, 6, 8, 10, 12, 15, 18};
    const std::string clearance_batch_path[batch_size] = {"2", "3", "4", "5", "6", "8", "10", "12", "15", "18"};
    std::vector<int> main_road_index_batch[batch_size];
    */

    std::vector<std::vector<float>> clearance_grid(WIDTH, std::vector<float>(HEIGHT, 0.0));
    std::vector<std::vector<float>> clearance_grid_river(WIDTH, std::vector<float>(HEIGHT, 0.0));
    for (int i = 0; i < number_index; ++ i)
    {
        // 获取道路的两个端点
        int a = index[i << 1], b = index[i << 1 | 1];
        int l_x0 = (int)point[a * 3] - MIN_X, l_y0 = (int)point[a * 3 + 1] - MIN_Y;
        int l_x1 = (int)point[b * 3] - MIN_X, l_y1 = (int)point[b * 3 + 1] - MIN_Y;

        if (is_valid_position(city_grid, l_x0, l_y0, true) && is_valid_position(city_grid, l_x1, l_y1, true)
            && city_grid[l_x0][l_y0] + city_grid[l_x1][l_y1] != -2)
        {

            if (clearance_grid[l_x0][l_y0] < 0.5)
            {
                clearance_grid[l_x0][l_y0] = get_point_clearance(city_grid, l_x0, l_y0, true, false);
                clearance_grid_river[l_x0][l_y0] = get_point_clearance(city_grid, l_x0, l_y0, true, true);
            }
            if (clearance_grid[l_x1][l_y1] < 0.5)
            {
                clearance_grid[l_x1][l_y1] = get_point_clearance(city_grid, l_x1, l_y1, true, false);
                clearance_grid_river[l_x1][l_y1] = get_point_clearance(city_grid, l_x1, l_y1, true, true);
            }

            // filter满足clearance大小的边
            if (clearance_grid[l_x0][l_y0] >= clearance_threhold && clearance_grid[l_x1][l_y1] >= clearance_threhold)
            {
                is_main_road[i] = true;
            }
            /*
            for (int k = 0; k < batch_size; ++ k)
            {
                if (clearance_grid[l_x0][l_y0] >= clearance_batch_threhold[k] && clearance_grid[l_x1][l_y1] >= clearance_batch_threhold[k])
                {
                    main_road_index_batch[k].push_back(a);
                    main_road_index_batch[k].push_back(b);
                }
            }
            */
        }
    }

    /*
    const std::string main_road_path_save = "./Data/road/main_road_clearance";
    for (int k = 0; k < batch_size; ++ k)
    {
        IO::save_line_data(main_road_path_save + "_" + clearance_batch_path[k] + ".txt", point, main_road_index_batch[k]);
        IO::save_line_data(main_road_path_save + "_" + clearance_batch_path[k] + ".dat", point, main_road_index_batch[k], true);
    }
    return;
    */


    /**********************************************************************
    ** 构建道路的Graph，过滤掉其中异常或者孤立的边
    **********************************************************************/
    printf("Build graph, and calculate connect component ...\n");
    std::vector<int> connect_component(number_point, -1);
    std::vector<std::vector<int>> city_graph(number_point);
    for (int i = 0; i < number_index; ++ i)
    {
        if (is_main_road[i])
        {
            int a = index[i << 1], b = index[i << 1 | 1];
            city_graph[a].push_back(b);
            city_graph[b].push_back(a);
        }
    }

    for (int i = 0; i < number_point; ++ i)
    {
        if (connect_component[i] == -1)
        {
            int weight = get_connect_component_dfs(city_graph, connect_component, i);
            set_connect_component_dfs(city_graph, connect_component, i, weight);
        }
    }

    // 利用联通分量的大小过滤掉孤立的细小结构，过滤掉曼哈顿距离较小的边
    printf("Filter main road by connect componet ...\n");
    const int distance_threhold = 1;
    const int connect_component_threhold = 64;
    for (int i= 0; i < number_index; ++ i)
    {
        if (is_main_road[i])
        {
            int a = index[i << 1], b = index[i << 1 | 1];
            float l_x0 = point[a * 3] - MIN_X, l_y0 = point[a * 3 + 1] - MIN_Y;
            float l_x1 = point[b * 3] - MIN_X, l_y1 = point[b * 3 + 1] - MIN_Y;
            float dis = std::abs(l_x0 - l_x1) + std::abs(l_y0 - l_y1);

            if (dis >= distance_threhold && connect_component[a] >= connect_component_threhold && connect_component[b] >= connect_component_threhold)
            {
                main_road_index.push_back(a);
                main_road_index.push_back(b);
            }
        }
    }

    /**********************************************************************
    ** 根据新筛选的主要道路，对道路系统进行重构，使用收缩半径r将节点v周围的其他节点
    ** 收缩到节点v上，以减少节点和边的数量，并减少细小的分支结构
    **********************************************************************/
    const int shrink_radius = 12;
    const int min_shrink_branch = 3;
    const int min_shrink_clearance = 6;

    std::vector<float> new_point;
    std::vector<int> point_id(number_point, -1);

    // 记录节点在网格上的位置，便于收缩时计算
    std::vector<std::vector<int>> point_grid(WIDTH, std::vector<int>(HEIGHT, -1));

    // 根据现在的主要道路，重新建立路网
    printf("Rebuild graph, and shrink ...\n");
    for (int i = 0; i < number_point; ++ i) city_graph[i].clear();
    for (int i = 0; i < (int)main_road_index.size(); i += 2)
    {
        int a = main_road_index[i], b = main_road_index[i | 1];
        city_graph[a].push_back(b);
        city_graph[b].push_back(a);

        int l_x0 = (int)point[a * 3] - MIN_X, l_y0 = (int)point[a * 3 + 1] - MIN_Y;
        int l_x1 = (int)point[b * 3] - MIN_X, l_y1 = (int)point[b * 3 + 1] - MIN_Y;
        point_grid[l_x0][l_y0] = a;
        point_grid[l_x1][l_y1] = b;
    }

    // 遍历所有的节点进行收缩
    for (int i = 0; i < number_point; ++ i)
    {
        int l_x = (int)point[i * 3] - MIN_X, l_y = (int)point[i * 3 + 1] - MIN_Y;
        if (point_id[i] == -1 && city_graph[i].size() >= min_shrink_branch && clearance_grid_river[l_x][l_y] >= min_shrink_clearance)
        {
            point_id[i] = (int)new_point.size() / 3;
            new_point.push_back(point[i * 3]);
            new_point.push_back(point[i * 3 + 1]);
            new_point.push_back(0.0);
            for (int dx = -shrink_radius; dx <= shrink_radius; ++ dx)
                for (int dy = -shrink_radius; dy <= shrink_radius; ++ dy)
                {
                    int px = l_x + dx, py = l_y + dy;
                    if (is_valid_position(city_grid, px, py, true) && point_grid[px][py] != -1)
                    {
                        int l_id = point_grid[px][py];
                        if (point_id[l_id] == -1) point_id[l_id] = point_id[i];
                    }
                }
        }
    }

    // 对所有未被收缩的点，任然作为一个节点
    printf("Deal with nonshrink point ...\n");
    for (int i = 0; i < number_point; ++ i)
    {
        // 表明该点是出现在主要道路中的，并且没有被收缩
        int l_x = (int)point[i * 3] - MIN_X, l_y = (int)point[i * 3 + 1] - MIN_Y;
        if (point_grid[l_x][l_y] != -1 && point_id[i] == -1)
        {
            point_id[i] = (int)new_point.size() / 3;
            new_point.push_back(point[i * 3]);
            new_point.push_back(point[i * 3 + 1]);
            new_point.push_back(0.0);
        }
    }

    // 构建新的主要道路的边
    printf("Build main road by shrink result ...\n");
    std::vector<int> new_index;
    std::set<std::pair<int, int>> new_index_set;
    for (int i = 0; i < (int)main_road_index.size(); i += 2)
    {
        int a = main_road_index[i], b = main_road_index[i | 1];
        if (point_id[a] != point_id[b])
        {
            int p = std::min(point_id[a], point_id[b]), q = std::max(point_id[a], point_id[b]);
            std::pair<int, int> it = std::make_pair(p, q);
            if (new_index_set.find(it) == new_index_set.end())
            {
                new_index_set.insert(it);
                new_index.push_back(p);
                new_index.push_back(q);
            }
        }
    }

    /**********************************************************************
    ** 根据新筛选的主要道路，对道路系统进行重构，使用收缩半径r将节点v周围的其他节点
    ** 收缩到节点v上，以减少节点和边的数量，并减少细小的分支结构
    **********************************************************************/
    /*
    printf("Remove violate edge which cross the obstacle ...\n");
    for (int i = 0; i < (int)new_index.size(); i += 2)
    {
        int a = new_index[i << 1], b = new_index[i << 1 | 1];
        int l_x0 = (int)new_point[a * 3] - MIN_X, l_y0 = (int)new_point[a * 3 + 1] - MIN_Y;
        int l_x1 = (int)new_point[b * 3] - MIN_X, l_y1 = (int)new_point[b * 3 + 1] - MIN_Y;

        if (!is_valid_edge(city_grid, l_x0, l_y0, l_x1, l_y1))
        {

        }
    }

    printf("Rebuild graph ...\n");
    */


    const std::string main_road_path = "./Data/road/main_road_shrink";
    IO::save_line_data(main_road_path + ".txt", new_point, new_index);
    printf("point : %d, index : %d\n", (int)new_point.size() / 3, (int)new_index.size() >> 1);
    IO::save_line_data(main_road_path + ".dat", new_point, new_index, true);

    //const std::string main_road_path = "./Data/road/main_road_clearance";
    //IO::save_line_data(main_road_path + ".txt", point, main_road_index);
    //printf("index size : %d\n", (int)main_road_index.size() >> 1);
    //IO::save_line_data(main_road_path + ".dat", point, main_road_index, true);

    //const std::string clearance_grid_path = "./Data/road/clearance_grid";
    //IO::save_city_grid_data(clearance_grid_path + ".txt", clearance_grid);
    //IO::save_city_grid_data(clearance_grid_path + ".dat", clearance_grid, true);

    //const std::string clearance_grid_map_path = "./Data/clearance_grid_map.png";
    //IO::save_city_map_data(clearance_grid_map_path, city_grid, clearance_grid, 0, 32);
}

int Algorithm::get_connect_component_dfs(const std::vector<std::vector<int>> & city_graph, std::vector<int> & weight, const int u)
{
    int ret = 1;
    weight[u] = 0;

    for (int i = 0; i < (int)city_graph[u].size(); ++ i)
    {
        int v = city_graph[u][i];
        if (weight[v] == -1)
        {
            ret += get_connect_component_dfs(city_graph, weight, v);
        }
    }
    return ret;
}

void Algorithm::set_connect_component_dfs(const std::vector<std::vector<int>> & city_graph, std::vector<int> & weight, const int u, const int w)
{
    weight[u] = w;;
    for (int i = 0; i < (int)city_graph[u].size(); ++ i)
    {
        int v = city_graph[u][i];
        if (weight[v] == 0)
        {
            set_connect_component_dfs(city_graph, weight, v, w);
        }
    }
}


/******************************************************************************
** 1 : 计算shelter容量及范围
*******************************************************************************/
void Algorithm::calculate_shelter_influence()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
        return;
    }

    const std::string agent_path = "./Data/agent/agent_rand_uniform.dat";
    std::vector<float> agent;

    // 导入agent位置数据
    io_status = IO::load_point_data(agent_path, agent);
    if (! io_status)
    {
        printf("Load %s failed !\n", agent_path.c_str());
        return;
    }

    // 将agent位置信息进行转换
    std::vector<int> agent_position;
    for (int i = 0; i < (int)agent.size(); i += 3)
    {
        int x = (int)agent[i] - MIN_X;
        int y = (int)agent[i + 1] - MIN_Y;
        agent_position.push_back(x << 16 | y);
    }
    std::sort(agent_position.begin(), agent_position.end());

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    // 获取每个shelter区域的id和所有格点
    printf("Find each shelter ...\n");
    int number_shelter = 0;
    std::vector<std::vector<int>> dist(l_row, std::vector<int>(l_col, -1));
    std::vector<std::vector<char>> shelter_id(l_row, std::vector<char>(l_col, -1));
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
            if (city_grid[i][j] == -1 && shelter_id[i][j] == -1)
            {
                calculate_shelter_influence_bfs(city_grid, shelter_id, i, j, number_shelter ++);
            }

    // 统计shelter的面积
    printf("Calculate shelter area and capactiy ...\n");
    front = tail = 0;
    std::vector<int> area(number_shelter, 0);
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
            if (city_grid[i][j] == -1)
            {
                dist[i][j] = 0;
                que[front ++] = i << 16 | j;
                ++ area[shelter_id[i][j]];
            }

    //for (int i = 0; i < number_shelter; ++ i) printf("%d : %d\n", i, area[i]);

    // 计算shelter的容量
    std::vector<int> capacity(number_shelter, 0);
    for (int i = 0; i < number_shelter; ++ i)
    {
        capacity[i] = area[i] / 10;
    }

    //std::vector<int> sort_area(area.begin(), area.end());
    //std::sort(sort_area.begin(), sort_area.end());
    //for (int i = 0; i < number_shelter; ++ i) printf("%d : %d\n", i, sort_area[i]);

    // 统计shelter影响范围
    printf("Calculate shelter influence area ...\n");
    //tail = front = 0;
    while (tail < front)
    {
        int s = que[tail ++];
        int x = s >> 16;
        int y = s & ((1 << 16) - 1);
        int id = shelter_id[x][y];

        for (int k = 0; k < 4; ++ k)
        {
            if (capacity[id] <= 0) break;

            int a = x + DX[k];
            int b = y + DY[k];
            if (is_valid_position(city_grid, a, b, false) && city_grid[a][b] == 0 && shelter_id[a][b] == -1)
            {
                int w = a << 16 | b;

                que[front ++] = w;
                shelter_id[a][b] = id;
                dist[a][b] = dist[x][y] + 1;
                if (std::binary_search(agent_position.begin(), agent_position.end(), w))
                {
                    -- capacity[id];
                }
            }
        }
    }
    const std::string shelter_influence_id_path = "./Data/shelter/shelter_influence_id";
    //IO::save_city_grid_data(shelter_influence_id_path + ".dat", shelter_id, true);
    //IO::save_damage_image_data(shelter_influence_id_path + ".png", city_grid, shelter_id);


    const std::string distance_to_shelter_path = "./Data/grid/distance_to_shelter";
    //IO::save_city_grid_data(distance_to_shelter_path + ".dat", dist, true);
    //IO::save_city_grid_data(distance_to_shelter_path + ".txt", dist, false);

    //const std::string shelter_influence_path = "./Data/shelter_id.png";
    const std::string shelter_influence_path = "./Data/shelter_influence";
    for (char c = '0'; c < '5'; ++ c)
    {
        std::string s = "0";
        s[0] = c;
        IO::save_city_map_shelter_influence(shelter_influence_path + "_" + s + ".png", number_shelter, city_grid, shelter_id);
    }

}

void Algorithm::calculate_shelter_influence_bfs(const std::vector<std::vector<char>> & grid, std::vector<std::vector<char>> & id_map, const int px, const int py, const int id)
{
    front = tail = 0;
    id_map[px][py] = id;
    que[front ++] = px << 16 | py;
    while (tail < front)
    {
        int w = que[tail ++];
        int x = w >> 16;
        int y = w & ((1 << 16) - 1);
        for (int k = 0; k < 4; ++ k)
        {
            int a = x + DX[k];
            int b = y + DY[k];
            if (is_valid_position(grid, a, b, false) && grid[a][b] == -1 && id_map[a][b] == -1)
            {
                id_map[a][b] = id;
                que[front ++] = a << 16 | b;
            }
        }

    }
}


/******************************************************************************
** 2 : 计算所有格点clearance
*******************************************************************************/
void Algorithm::calculate_space_clearance()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
        return;
    }

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    std::vector<std::vector<float>> clearance(l_row, std::vector<float>(l_col, -1.0));

    front = tail = 0;
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
            if (city_grid[i][j] > 0)
            {
                clearance[i][j] = 0.0;
                que[front ++] = i << 16 | j;
            }

    // 统计shelter影响范围
    printf("Calculate grid clearance ...\n");
    while (tail < front)
    {
        int s = que[tail ++];
        int x = s >> 16;
        int y = s & ((1 << 16) - 1);

        for (int k = 0; k < 8; ++ k)
        {
            int a = x + DX[k];
            int b = y + DY[k];
            if (is_valid_position(city_grid, a, b, false) && city_grid[a][b] <= 0 && clearance[a][b] < -0.5)
            {
                int w = a << 16 | b;

                que[front ++] = w;
                clearance[a][b] = clearance[x][y] + DIS[k];
            }
        }
    }

    const std::string space_clearance_path = "./Data/grid/space_clearance";
    IO::save_city_grid_data(space_clearance_path + ".dat", clearance, true);
    //IO::save_city_grid_data(space_clearance_path + ".txt", clearance, false);

    const std::string space_clearance_map_path = "./Data/space_clearance.png";
    IO::save_city_space_clearance(space_clearance_map_path, city_grid, clearance);
}

void Algorithm::calculate_damage_clearance()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    const std::string space_clearance_path = "./Data/damage/city_damage_clearance_";
    const std::string space_clearance_map_path = "./Data/damage/city_damage_clearance_";

    std::string idx = "#";
    for (char c = '0'; c <= '9'; ++ c)
    {
        idx[0] = c;
        const std::string damage_grid_path = "./Data/damage/city_damage_grid_" + idx + ".dat";
        std::vector<std::vector<char>> damage_grid;

        // 导入// city damage数据(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
        IO::load_city_map(damage_grid_path, damage_grid);

        std::vector<std::vector<float>> clearance(l_row, std::vector<float>(l_col, -1.0));

        front = tail = 0;
        for (int i = 0; i < l_row; ++ i)
            for (int j = 0; j < l_col; ++ j)
                if (damage_grid[i][j] >= 2)
                {
                    clearance[i][j] = 0.0;
                    que[front ++] = i << 16 | j;
                }

        // 统计shelter影响范围
        printf("Calculate damage %c clearance ...\n", c);
        while (tail < front)
        {
            int s = que[tail ++];
            int x = s >> 16;
            int y = s & ((1 << 16) - 1);

            for (int k = 0; k < 8; ++ k)
            {
                int a = x + DX[k];
                int b = y + DY[k];
                if (is_valid_position(city_grid, a, b, false) && city_grid[a][b] <= 0 && clearance[a][b] < -0.5)
                {
                    int w = a << 16 | b;

                    que[front ++] = w;
                    clearance[a][b] = clearance[x][y] + DIS[k];
                }
            }
        }

        IO::save_city_grid_data(space_clearance_path + idx + ".dat", clearance, true);
        //IO::save_city_grid_data(space_clearance_path + ".txt", clearance, false);

        IO::save_city_space_clearance(space_clearance_map_path + idx + ".png", city_grid, clearance);
    }
}

void Algorithm::calculate_shelter_clearance()
{
    const std::string space_clearance_path = "./Data/shelter/shelter_clearance_";
    const std::string space_clearance_map_path = "./Data/shelter/shelter_clearance_";

    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    std::string idx = "#";
    for (char c = '0'; c <= '9'; ++ c)
    {
        idx[0] = c;
        const std::string damage_grid_path = "./Data/damage/city_damage_grid_" + idx + ".dat";
        std::vector<std::vector<char>> damage_grid;

        // 导入// city damage数据(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
        IO::load_city_map(damage_grid_path, damage_grid);

        std::vector<std::vector<float>> clearance(l_row, std::vector<float>(l_col, 1e6));

        front = tail = 0;
        for (int i = 0; i < l_row; ++ i)
            for (int j = 0; j < l_col; ++ j)
                if (city_grid[i][j] == -1)
                {
                    clearance[i][j] = 0.0;
                    que[front ++] = i << 16 | j;
                }

        // 统计shelter影响范围
        const float DISTANCE_THREHOLD = 200.0;
        printf("Calculate shelter %c clearance ...\n", c);
        while (tail < front)
        {
            int s = que[tail ++];
            int x = s >> 16;
            int y = s & ((1 << 16) - 1);

            for (int k = 0; k < 8; ++ k)
            {
                int a = x + DX[k];
                int b = y + DY[k];
                if (is_valid_position(city_grid, a, b, false) && damage_grid[a][b] <= 0 && clearance[a][b] > 1e5)
                {
                    int w = a << 16 | b;

                    clearance[a][b] = clearance[x][y] + DIS[k];
                    if (clearance[a][b] < DISTANCE_THREHOLD)
                    {
                        que[front ++] = w;
                    }
                }
            }
        }

        IO::save_city_grid_data(space_clearance_path + idx + ".dat", clearance, true);
        //IO::save_city_grid_data(space_clearance_path + ".txt", clearance, false);

        IO::save_city_space_clearance(space_clearance_map_path + idx + ".png", city_grid, clearance);
    }
}

/******************************************************************************
** 3 : 将主要道路与整体道路相关联
*******************************************************************************/
void Algorithm::connect_main_road_and_detailed_road()
{
    // 导入主要道路和整体道路数据
    std::vector<int> main_index, detail_index;
    std::vector<float> main_point, detail_point;

    IO::load_line_data("./Data/road/road.dat", detail_point, detail_index);
    IO::load_line_data("./Data/road/create_main_road.dat", main_point, main_index);

    int number_main_point = (int)main_point.size() / 3;
    int number_detail_point = (int)detail_point.size() / 3;

    /*
    // 去掉构建的主要道路的重边
    std::vector<int> new_index;
    for (int i = 0; i < (int)main_index.size(); i += 2)
    {
        int a = main_index[i];
        int b = main_index[i + 1];
        if (a != b)
        {
            new_index.push_back(a);
            new_index.push_back(b);
        }
    }

    IO::save_line_data("./Data/road/create_main_road_0.dat", main_point, new_index, true);
    IO::save_line_data("./Data/road/create_main_road_0.txt", main_point, new_index, false);
    */

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
        return;
    }

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    const int DIS_THREHOLD = 16;

    std::vector<std::vector<int>> grid_main_point(l_row, std::vector<int>(l_col, -1));
    for (int k = 0; k < number_main_point; ++ k)
    {
        int x = (int)main_point[k * 3] - MIN_X;
        int y = (int)main_point[k * 3 + 1] - MIN_Y;
        for (int a = x - DIS_THREHOLD; a <= x + DIS_THREHOLD; ++ a)
            for (int b = y - DIS_THREHOLD; b <= y + DIS_THREHOLD; ++ b)
                if (is_valid_position(city_grid, a, b, false) && city_grid[a][b] <= 0)
                {
                    grid_main_point[a][b] = k;
                }
    }

    std::vector<int> connect;
    for (int i = 0; i < number_detail_point; ++ i)
    {
        int x = (int)detail_point[i * 3] - MIN_X;
        int y = (int)detail_point[i * 3 + 1] - MIN_Y;
        if (is_valid_position(city_grid, x, y, false) && grid_main_point[x][y] != -1)
        {
            connect.push_back(i);
            connect.push_back(grid_main_point[x][y]);
        }
    }

    IO::save_index_data("./Data/road/connect.dat", connect, true);
    IO::save_index_data("./Data/road/connect.txt", connect, false);
}


/******************************************************************************
** 4 : 仿真预处理
*******************************************************************************/
void Algorithm::pre_calculate_shelter_infomation()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
        return;
    }

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    // 获取每个shelter区域的id和所有格点
    printf("Find each shelter ...\n");
    int number_shelter = 0;
    std::vector<std::vector<int>> dist(l_row, std::vector<int>(l_col, -1));
    std::vector<std::vector<char>> shelter_id(l_row, std::vector<char>(l_col, -1));
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
            if (city_grid[i][j] == -1 && shelter_id[i][j] == -1)
            {
                calculate_shelter_influence_bfs(city_grid, shelter_id, i, j, number_shelter ++);
            }

    const std::string shelter_id_path = "./Data/shelter/shelter_id.dat";
    IO::save_city_grid_data(shelter_id_path, shelter_id, true);

    // 统计shelter的面积
    printf("Calculate shelter area and capactiy ...\n");
    front = tail = 0;
    std::vector<int> area(number_shelter, 0);
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
            if (city_grid[i][j] == -1)
            {
                dist[i][j] = 0;
                que[front ++] = i << 16 | j;
                ++ area[shelter_id[i][j]];
            }

    for (int i = 0; i < number_shelter; ++ i) printf("%d : %d\n", i, area[i]);

    // ================== 对于所有道路顶点 =================
    const std::string road_path = "./Data/road/road.dat";
    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(road_path, point, index);

    const int number_point = (int)point.size() / 3;
    const int number_index = (int)index.size() / 2;

    std::vector<int> flag(number_point, -1);

    for (int k = 0; k < number_index; ++ k)
    {
        int a = index[k << 1], b = index[k << 1 | 1];
        int x0 = (int)point[a * 3] - MIN_X;
        int y0 = (int)point[a * 3 + 1] - MIN_Y;
        int x1 = (int)point[b * 3] - MIN_X;
        int y1 = (int)point[b * 3 + 1] - MIN_Y;
        int sa = city_grid[x0][y0], sb = city_grid[x1][y1];
        if (sa == -1 && sb == 0)
        {
            flag[a] = shelter_id[x0][y0]; 
        }
        else if (sa == 0 && sb == -1)
        {
            flag[b] = shelter_id[x1][y1];
        }
    }
    int count_a = 0;
    for (int i = 0; i < number_point; ++ i)
    {
        if (flag[i] != -1) ++ count_a;
    }

    // ================== 对于所有主要道路顶点 =================
    const std::string main_road_path = "./Data/road/create_main_road.dat";
    IO::load_line_data(main_road_path, point, index);

    const int number_main_point = (int)point.size() / 3;

    std::vector<int> main_flag(number_main_point, -1);
    for (int id = 0; id < number_main_point; ++ id)
    {
        int x = (int)point[id * 3] - MIN_X;
        int y = (int)point[id * 3 + 1] - MIN_Y;
        if (city_grid[x][y] == -1)
        {
            main_flag[id] = shelter_id[x][y];
        }
        else
        {
            const int threhold = 10;
            bool near_shelter = false;
            for (int a = x - threhold; a <= x + threhold; ++ a)
                for (int b = y - threhold; b <= y + threhold; ++ b)
                    if (is_valid_position(city_grid, a, b, false) && city_grid[a][b] == -1)
                    {
                        near_shelter = true;
                    }

            if (near_shelter) main_flag[id] = shelter_id[x][y];
        }
    }

    int count_b = 0;
    for (int i = 0; i < number_main_point; ++ i)
    {
        if (main_flag[i] != -1) ++ count_b;
    }

    printf("Number %d, %d\n", count_a, count_b);

    std::vector<Shelter> shelter;
    for (int id = 0; id < number_shelter; ++ id)
    {
        Shelter it;
        it.area = area[id];
        for (int k = 0; k < number_point; ++ k)
        {
            if (flag[k] == id) it.ids.push_back(k);
        }
        for (int k = 0; k < number_main_point; ++ k)
        {
            if (main_flag[k] == id) it.main_ids.push_back(k);
        }
        shelter.push_back(it);
    }

    const std::string shelter_path = "./Data/shelter/shelter.txt";
    IO::save_shelters(shelter_path, shelter);
}

void Algorithm::pre_calculate_road_distance_and_width()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    bool io_status = IO::load_city_map(city_grid_path, city_grid);
    if (! io_status)
    {
        printf("Load %s failed !\n", city_grid_path.c_str());
        return;
    }

    const std::string clearance_path = "./Data/grid/space_clearance.dat";
    std::vector<std::vector<int>> clearance;
    IO::load_city_grid_data(clearance_path, clearance, true);

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size(); 

    // ================== 对于所有道路顶点 =================
    const std::string road_path = "./Data/road/road.dat";
    const std::string road_path_out = "./Data/road/road_edge_attribute";
    pre_calculate_road_distance_and_width(city_grid, clearance, road_path, road_path_out);
    

    // ================== 对于所有主要道路顶点 =================
    const std::string main_road_path = "./Data/road/create_main_road.dat";
    const std::string main_road_path_out = "./Data/road/main_road_edge_attribute";
    pre_calculate_road_distance_and_width(city_grid, clearance, main_road_path, main_road_path_out);
}

void Algorithm::pre_calculate_road_distance_and_width(const std::vector<std::vector<char>> & grid,
     const std::vector<std::vector<int>> & clearance, const std::string & path_in, const std::string & path_out)
{
    const int inf = 1e9;

    std::vector<int> index;
    std::vector<float> point;
    IO::load_line_data(path_in, point, index);

    const int number_point = (int)point.size() / 3;
    const int number_index = (int)index.size() / 2;

    std::vector<float> dis(number_index, 0.0);
    std::vector<int> width(number_index, 0.0);

    for (int k = 0; k < number_index; ++ k)
    {
        int a = index[k << 1], b = index[k << 1 | 1];
        float x0 = point[a * 3] - MIN_X, y0 = point[a * 3 + 1] - MIN_Y;
        float x1 = point[b * 3] - MIN_X, y1 = point[b * 3 + 1] - MIN_Y;

        int l_width = inf;
        dis[k] = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

        int part = 0;
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
            width[k] = clearance[(int)x0][(int)y0];
        }
        else
        {
            float x = x0, y = y0, px = (x1 - x0) / part, py = (y1 - y0) / part;
            for (int i = 0; i <= part; ++ i)
            {
                l_width = std::min(l_width, clearance[(int)x][(int)y]);
                x += px, y += py;
            }
            width[k] = l_width;
        }
    }
    IO::save_road_attribute_data(path_out + "_distance.dat", dis, true);
    IO::save_road_attribute_data(path_out + "_distance.txt", dis, false);
    IO::save_road_attribute_data(path_out + "_width.dat", width, true);
    IO::save_road_attribute_data(path_out + "_width.txt", width, false);
}

void Algorithm::pre_get_agent_timestep_data()
{
    // 根据时间步拼接文件名字
    std::string path = "./Data/agent/run9/timestep9_";

    char timestamp[16];

    std::vector<int> ids;
    std::vector<float> points;
    for (int t = 0; t < TIMESTAMP; t += STEP)
    {
        sprintf(timestamp, "%07d", t);
        std::string in_path = path + std::string(timestamp) + ".txt";
        std::string out_path = path + std::string(timestamp) + ".dat";

        IO::load_agent_data(in_path, points, ids, false);
        IO::save_agent_data(out_path, points, ids, true);
    }
}

void Algorithm::pre_calculate_orignal_shelter_influence()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    std::vector<std::vector<float>> agent;
    std::string all_agent_path = "./Data/agent/agent_all_0.dat";
    IO::load_agent_path(all_agent_path, agent, true);

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    std::vector<std::vector<char>> shelter_id;
    const std::string shelter_id_path = "./Data/shelter/shelter_id.dat";
    IO::load_city_grid_data(shelter_id_path, shelter_id, true);

    const int THREHOLD = 10;
    std::vector<float> agent_to_shelter;
    std::vector<int> id_to_shelter;

    for (int k = 0; k < (int)agent.size(); ++ k)
    {
        const int m = (int)agent[k].size();
        if (m < 2) continue;
        int x = (int)agent[k][m - 2] - MIN_X;
        int y = (int)agent[k][m - 1] - MIN_Y;

        int id = -1;
        float dis = 1e9;
        for (int a = x - THREHOLD; a <= x + THREHOLD; ++ a)
            for (int b = y - THREHOLD; b <= y + THREHOLD; ++ b)
            {
                if (is_valid_position(shelter_id, a, b, false) && shelter_id[a][b] != -1)
                {
                    float d = std::abs(a - x) + std::abs(b - y);
                    if (d < dis) id = shelter_id[a][b];
                }
            }
        if (id != -1)
        {
            agent_to_shelter.push_back(agent[k][0]);
            agent_to_shelter.push_back(agent[k][1]);
            id_to_shelter.push_back(id);
        }
    }
    
    std::vector<std::vector<char>> l_image(l_row, std::vector<char>(l_col, -1));

    front = tail = 0;
    
    for (int i = 0; i < l_row; ++ i)
        for (int j = 0; j < l_col; ++ j)
        {
            if (city_grid[i][j] == -1)
            {
                l_image[i][j] = shelter_id[i][j];
            }
            else if (city_grid[i][j] >= 1)
            {
                l_image[i][j] = -2;
            }
        }

    const int n = (int)agent_to_shelter.size() >> 1;
    for (int k = 0; k < n; ++ k)
    {
        int x = (int)agent_to_shelter[k << 1] - MIN_X;
        int y = (int)agent_to_shelter[k << 1 | 1] - MIN_Y;
        l_image[x][y] = id_to_shelter[k];
        que[front ++] = x << 16 | y;
    }

    const int BIT = (1 << 16) - 1;
    while (tail < front)
    {
        int s = que[tail ++];
        int x = s >> 16;
        int y = s & BIT;
        int id = l_image[x][y];

        for (int k = 0; k < 4; ++ k)
        {
            int a = x + DX[k];
            int b = y + DY[k];
            if (is_valid_position(l_image, a, b, false) && l_image[a][b] == -1)
            {
                int w = a << 16 | b;
                que[front ++] = w;
                l_image[a][b] = id;
            }
        }
    }

    const std::string original_shelter_influence_path = "./Data/shelter/original_influence";
    IO::save_city_image_data(original_shelter_influence_path, city_grid, l_image);

}

void Algorithm::pre_calculate_my_shelter_influence()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;

    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    std::vector<int> l_ids;
    std::vector<float> l_points;
    std::string agent_path = "./Data/agent/agent_to_shelter.txt";
    IO::load_agent_data(agent_path, l_points, l_ids, false);

    const int l_row = (int)city_grid.size();
    const int l_col = (int)city_grid[0].size();

    std::vector<std::vector<char>> shelter_id;
    const std::string shelter_id_path = "./Data/shelter/shelter_id.dat";
    IO::load_city_grid_data(shelter_id_path, shelter_id, true);

    std::vector<float> agent_to_shelter;
    std::vector<int> id_to_shelter;
    for (int i = 0; i < l_ids.size(); ++ i)
    {
        if (l_ids[i] >= 0 && l_ids[i] < 50)
        {
            id_to_shelter.push_back(l_ids[i]);
            agent_to_shelter.push_back(l_points[i * 3]);
            agent_to_shelter.push_back(l_points[i * 3 + 1]);
        }
    }

    std::vector<std::vector<char>> l_image(l_row, std::vector<char>(l_col, -1));

    front = tail = 0;

    for (int i = 0; i < l_row; ++ i)
    {
        for (int j = 0; j < l_col; ++ j)
        {
            if (city_grid[i][j] == -1)
            {
                l_image[i][j] = shelter_id[i][j];
            }
            else if (city_grid[i][j] >= 1)
            {
                l_image[i][j] = -2;
            }
        }
    }

    const int n = (int)agent_to_shelter.size() >> 1;
    for (int k = 0; k < n; ++ k)
    {
        int x = (int)agent_to_shelter[k << 1] - MIN_X;
        int y = (int)agent_to_shelter[k << 1 | 1] - MIN_Y;
        l_image[x][y] = id_to_shelter[k];
        que[front ++] = x << 16 | y;
    }

    const int BIT = (1 << 16) - 1;
    while (tail < front)
    {
        int s = que[tail ++];
        int x = s >> 16;
        int y = s & BIT;
        int id = l_image[x][y];

        for (int k = 0; k < 4; ++ k)
        {
            int a = x + DX[k];
            int b = y + DY[k];
            if (is_valid_position(l_image, a, b, false) && l_image[a][b] == -1)
            {
                int w = a << 16 | b;
                que[front ++] = w;
                l_image[a][b] = id;
            }
        }
    }

    const std::string original_shelter_influence_path = "./Data/shelter/my_shelter_influence";
    IO::save_city_image_data(original_shelter_influence_path, city_grid, l_image);
}

/*=============================================================================
  =============================================================================
  =============================  其他功能函数  ==================================
  =============================================================================
  ============================================================================*/

/*********************************** 添加一条道路 ************************************/
void Algorithm::add_detailed_road()
{
    std::vector<int> index;
    std::vector<float> point;

    IO::load_line_data("./Data/road/road_original.dat", point, index);
    index.push_back(1);
    index.push_back(2);

    IO::save_line_data("./Data/road/road.dat", point, index, true);
    IO::save_line_data("./Data/road/road.txt", point, index, false);
}

void Algorithm::get_agent_subset()
{
    const std::string agent_path = "./Data/agent/run0/timestep0_0000000.dat";
    const std::string agent_path_out = "./Data/agent/agent_5_rand.dat";
    std::vector<float> points, sub_points;
    IO::load_point_data(agent_path, points);

    const int point_number = (int)points.size() / 3;
    const int point_wanted = 5;

    srand(time(NULL));
    std::vector<int> l_rand;
    for (int i = 0; i < point_number; ++ i) l_rand.push_back(i);

    std::random_shuffle(l_rand.begin(), l_rand.end());
    for (int i = 0; i < point_wanted; ++ i)
    {
        int id = l_rand[i];
        sub_points.push_back(points[id * 3]);
        sub_points.push_back(points[id * 3 + 1]);
        sub_points.push_back(0.0);
    }
    IO::save_point_data(agent_path_out, sub_points, true);
}

void Algorithm::get_init_agent()
{
    const std::string city_grid_path = "./Data/city_grid.dat";
    std::vector<std::vector<char>> city_grid;
    // 导入city map数据(-1 for shelter, 0 for road, 1 for obstacle and 2 for river)
    IO::load_city_map(city_grid_path, city_grid);

    const std::string clearance_path = "./Data/grid/space_clearance.dat";
    std::vector<std::vector<float>> clearance;
    IO::load_city_grid_data(clearance_path, clearance, true);

    std::vector<std::vector<float>> clearance_shelter;
    const std::string city_shelter_clearance_path = "./Data/shelter/shelter_clearance_0.dat";
    // 导入city shelter clearance数据
    IO::load_city_grid_data(city_shelter_clearance_path, clearance_shelter, true);

    const std::string city_damage_clearance_path = "./Data/damage/city_damage_grid_0.dat";
    std::vector<std::vector<char>> damage_grid;
    // 导入city damage数据(-1 for shelter, 0 for road, 1 for obstacle, 2 for river and 3 for damage)
    IO::load_city_grid_data(city_damage_clearance_path, damage_grid, true);

    std::vector<float> l_point;

    srand(time(NULL));
    int l_count = 0;
    while (l_count < AGENT)
    {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;

        if (city_grid[x][y] == 0 && damage_grid[x][y] != 3 && clearance[x][y] > 2.0 && clearance_shelter[x][y] > 5.0)
        {
            if (x > CITY_WIDTH) if (rand() % 5 != 0) continue;
            else if (clearance[x][y] < 5) if (rand() % 100 > 30) continue;
            else if (clearance[x][y] > 50) if (rand() % 100 > 70) continue;
            else if (clearance[x][y] > 100) if (rand() % 100 > 50) continue;
            if (clearance_shelter[x][y] < 32) if (rand() % 100 > 25) continue;
            else if (clearance_shelter[x][y] < 128) if (rand() % 100 > 50) continue;
            else if (clearance_shelter[x][y] < 200) if (rand() % 100 > 75) continue;

            ++ l_count;
            city_grid[x][y] = 'x';
            x += rand() % 1000 / 1000.0;
            y += rand() % 1000 / 1000.0;
            l_point.push_back(x + MIN_X);
            l_point.push_back(y + MIN_Y);
            l_point.push_back(0.0);
        }
    }
    const std::string agent_path_out = "./Data/agent/agent_rand_uniform.dat";
    IO::save_point_data(agent_path_out, l_point, true);
}

void Algorithm::get_agent_information()
{
    std::vector<std::vector<float>> agent(AGENT);
    std::string all_agent_path = "./Data/agent/agent_all_0.dat";
    IO::load_agent_path(all_agent_path, agent, true);

    int l_arrive = 0;
    double l_sum = 0;
    for (int k = 0; k < AGENT; ++ k)
    {
        if (agent[k].size() == 600) continue;

        ++ l_arrive;
        double l_count = 0;
        for (int i = 2; i < (int)agent[k].size(); i += 2)
        {
            double dx = agent[k][i] - agent[k][i - 2];
            double dy = agent[k][i + 1] - agent[k][i - 1];
            double dis = std::sqrt(dx * dx + dy * dy);
            if (dis < 100.0)
            {
                l_count += dis;
            }
        }
        l_sum += l_count;
    }
    printf("%.6lf\n", l_sum / l_arrive);

    /*
    int l_count = 0;
    double l_sum = 0.0;
    for (int k = 0; k < AGENT; ++ k)
    {
        for (int i = 2; i < (int)agent[k].size(); i += 2)
        {
            double dx = agent[k][i] - agent[k][i - 2];
            double dy = agent[k][i + 1] - agent[k][i - 1];
            double dis = std::sqrt(dx * dx + dy * dy);
            if (dis < 0.0001) continue;
            if (dis < 100.0)
            {
                ++ l_count;
                l_sum += dis;
            }
        }
    }
    printf("%.6lf\n", l_sum / (l_count == 0 ? 1.0 : l_count));
    */

    /*
    const int N = 300;
    std::vector<int> l_count(N + 1, 0);
    for (int i = 0; i < AGENT; ++ i)
    {
        ++ l_count[(int)agent[i].size() >> 1];
    }
    for (int i = 1; i < N; ++ i) l_count[i] += l_count[i - 1];
    l_count[N] = l_count[N - 1];

    for (int k = 0; k < 30; ++ k)
    {
        printf("%d\n", l_count[(k + 1) * 10]);
    }
    */
    //for (int i = 0; i <= 300; ++ i)
    //{
        //printf("%d\n", l_count[i]);
    //}
}



