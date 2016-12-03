#include <vector>
#include <algorithm>
#include <functional>

#include <QtGui>

#include "Algorithm.h"
#include "Utilities/IO.h"

const int STEP = 50;
const int AGENT = 75000;
const int TIMESTAMP = 15000;

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

    IO::save_line_data("./Data/road.txt", point, index);
    IO::save_line_data("./Data/road.dat", point, index, true);

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
    IO::save_line_data("./Data/main_road.txt", point, main_road_index);
    IO::save_line_data("./Data/main_road.dat", point, main_road_index, true);

    return;
}

void Algorithm::find_the_main_road_by_agent_movement()
{
    std::vector<std::vector<char>> city_map;
    IO::load_city_map("./Data/damage/city_damage_map_0.dat", city_map);

    std::vector<std::vector<float>> agent(AGENT);
    IO::load_agent_path("./Data/agent/agent_all.dat", agent, true);

    std::vector<std::vector<float>> heat_map(WIDTH, std::vector<float>(HEIGHT, 0));

    const int MIN_DIS_THREHOLD = 10;

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

    IO::save_city_map_data("./Data/agent_heat_map.png", city_map, heat_map);
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
