#include "IO.h"
#include "Simulation.h"

const int CITY_WIDTH = 8802;

bool IO::file_exist(const std::string & path)
{
    std::ifstream ifs(path, std::ifstream::in);
    return ifs.good();
}

bool IO::load_shader(const std::string & path, std::vector<char> & data)
{
    data.clear();

    // read shader source
    std::ifstream l_shader_file_stream(path);
    if (!l_shader_file_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    l_shader_file_stream.seekg(0, std::ios::end);
    std::streampos l_shader_file_length = l_shader_file_stream.tellg();
    l_shader_file_stream.seekg(0, std::ios::beg);

    data.resize(l_shader_file_length);
    l_shader_file_stream.read(data.data(), l_shader_file_length);

    l_shader_file_stream.close();

    return true;
}

bool IO::load_point_data(const std::string & path, std::vector<float> & point)
{
    point.clear();

    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_point_size;
    l_stream.read((char *)&l_point_size, sizeof(int));

    point.resize(l_point_size * 3);

    l_stream.read((char *)point.data(), point.size() * sizeof(float));

    l_stream.close();

    return true;
}

bool IO::load_index_data(const std::string & path, std::vector<int> & index)
{
    index.clear();

    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_index_size;
    l_stream.read((char *)&l_index_size, sizeof(int));

    index.resize(l_index_size * 2);

    l_stream.read((char *)index.data(), index.size() * sizeof(int));

    l_stream.close();

    return true;
}

bool IO::load_line_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
{
    point.clear();
    index.clear();

    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_point_size, l_index_size;
    l_stream.read((char *)&l_point_size, sizeof(int));
    l_stream.read((char *)&l_index_size, sizeof(int));

    point.resize(l_point_size * 3);
    index.resize(l_index_size * 2);

    l_stream.read((char *)point.data(), point.size() * sizeof(float));
    l_stream.read((char *)index.data(), index.size() * sizeof(int));

    l_stream.close();

    return true;
}

bool IO::load_triangle_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
{
    point.clear();
    index.clear();

    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_point_size, l_index_size;
    l_stream.read((char *)&l_point_size, sizeof(int));
    l_stream.read((char *)&l_index_size, sizeof(int));

    point.resize(l_point_size * 3);
    index.resize(l_index_size * 3);

    l_stream.read((char *)point.data(), point.size() * sizeof(float));
    l_stream.read((char *)index.data(), index.size() * sizeof(int));

    l_stream.close();

    return true;
}

bool IO::load_damage(const std::string & path, std::vector<int> & x_coordinate,
    std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data)
{
    x_coordinate.clear();
    y_coordinate.clear();
    data.clear();

    printf("loading damage file : %s ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int x_count, y_count;
    std::string line;
    while (std::getline(l_stream, line)) 
    {
        if (line.find("X_COORDINATES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> x_count >> line;
            x_coordinate.resize(x_count);
            for (int i = 0; i < x_count; ++ i) l_stream >> x_coordinate[i];
        }
        else if (line.find("Y_COORDINATES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> y_count >> line;
            y_coordinate.resize(y_count);
            for (int i = 0; i < y_count; ++ i) l_stream >> y_coordinate[i];
        }
        else if (line.find("obstacle_data") == 0)
        {
            int l_t;
            -- x_count;
            -- y_count;
            std::vector<char> l_v(y_count, 0);
            data.resize(x_count, l_v);
            int l_step = (y_count + PROCESS_STEP - 1) / PROCESS_STEP;
            for (int i = 0; i < y_count; ++ i)
            {
                if (i % l_step == 0) printf("#");
                for (int j = 0; j < x_count; ++ j)
                {
                    l_stream >> l_t;
                    data[j][y_count - i - 1] = (char)l_t;
                }
            }
            printf("\n");
        }
    }
    l_stream.close();
    printf("Finish loading : %s !\n", path.c_str());
    return true;
}

bool IO::save_damage_data(const std::string & path,
    const std::vector<std::vector<char>> & data, bool binary)
{
    if (data.empty()) return false;

    printf("saving data : %s  ... \n", path.c_str());
    const int width = (int)data.size();
    const int height = (int)data[0].size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&width, sizeof(int));
        stream.write((char *)&height, sizeof(int));

        for (int i = 0; i < width; ++ i)
        {
            stream.write(data[i].data(), height);
        }
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << width << " " << height << std::endl;
        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << (int)data[i][j];
            }
            stream << std::endl;
        }
        printf("\n");
        stream.close();
    }
    printf("Finish saving : %s !\n", path.c_str());
    return true;
}

bool IO::load_graph_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
{
    point.clear();
    index.clear();

    printf("loading graph file : %s  ... ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_count = 0;
    std::string line;
    while (std::getline(l_stream, line))
    {
        if (line.empty()) continue;
        if (line.find("POINTS") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count >> line;
            point.resize(l_count * 3);
            for (int i = 0; i < l_count; ++ i)
            {
                l_stream >> point[i * 3] >> point[i * 3 + 1] >> point[i * 3 + 2];
            }
        }
        else if (line.find("CELLS") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count >> line;
            index.resize(l_count << 1);
            for (int i = 0; i < l_count; ++ i)
            {
                l_stream >> line >> index[i << 1] >> index[i << 1 | 1];
            }
        }
        else if (line.find("CELL_TYPES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
            for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
        }
        else if (line.find("POINT_DATA") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
        }
        else if (line.find("CELL_DATA") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
        }
        else if (line.find("SCALARS") == 0)
        {
            std::string attribute_name;
            std::stringstream ss(line);
            ss >> line >> attribute_name >> line >> line;
            std::getline(l_stream, line);
            for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
        }
    }
    l_stream.close();
    printf("Finish loading graph file : %s !\n", path.c_str());
    return true;
}

bool IO::load_graph_attribute(const std::string & path, const std::string & name, std::vector<float> & data)
{
    data.clear();

    printf("loading graph file : %s  ... ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int l_count = 0;
    std::string line;
    while (std::getline(l_stream, line))
    {
        if (line.empty()) continue;
        if (line.find("POINTS") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count >> line;
            for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
        }
        else if (line.find("CELLS") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count >> line;
            for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
        }
        else if (line.find("CELL_TYPES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
            for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
        }
        else if (line.find("POINT_DATA") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
        }
        else if (line.find("CELL_DATA") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_count;
        }
        else if (line.find("SCALARS") == 0)
        {
            std::string attribute_name;
            std::stringstream ss(line);
            ss >> line >> attribute_name >> line >> line;
            std::getline(l_stream, line);
            if (attribute_name == name)
            {
                data.resize(l_count);
                for (int i = 0; i < l_count; ++ i) l_stream >> data[i];
            }
            else
            {
                for (int i = 0; i < l_count; ++ i) std::getline(l_stream, line);
            }
        }
    }
    l_stream.close();
    printf("Finish loading graph file : %s !\n", path.c_str());
    return true;
}

bool IO::load_agent_data(const std::string & path, std::vector<float> & points, std::vector<int> & ids, bool binary)
{
    points.clear();
    ids.clear();

    printf("loading agent : %s  ... ", path.c_str());

    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        int point_size;
        stream.read((char *)&point_size, sizeof(int));

        ids.resize(point_size);
        points.resize(point_size * 3);

        stream.read((char *)points.data(), points.size() * sizeof(float));
        stream.read((char *)ids.data(), ids.size() * sizeof(int));

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        int point_size;
        stream >> point_size;

        ids.resize(point_size);
        points.resize(point_size * 3);

        for (int i = 0; i < (int)points.size(); ++ i) stream >> points[i];
        for (int i = 0; i < (int)ids.size(); ++ i) stream >> ids[i];

        stream.close();
    }

    printf("Done !\n");
    return true;
}

bool IO::save_agent_data(const std::string & path, const std::vector<float> & points, const std::vector<int> & ids, bool binary)
{
    printf("saving agent : %s  ... ", path.c_str());

    int point_size = (int)ids.size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&point_size, sizeof(int));

        stream.write((char *)points.data(), points.size() * sizeof(float));
        stream.write((char *)ids.data(), ids.size() * sizeof(int));

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << point_size << std::endl;

        for (int i = 0; i < point_size; ++ i)
        {
            stream << points[i * 3] << " " << points[i * 3 + 1] << " " << points[i * 3 + 2] << std::endl;
        }
        for (int i = 0; i < (int)ids.size(); ++ i) stream << ids[i] << std::endl;

        stream.close();
    }

    printf("Done !\n");
    return true;
}

bool IO::load_agent_path(const std::string & path, std::vector<std::vector<float>> & agents, bool binary)
{
    agents.clear();

    printf("Loading agent path : %s  ...\n", path.c_str());

    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        int agent_size, point_size;
        stream.read((char *)&agent_size, sizeof(int));
        agents.resize(agent_size);

        int l_step = (agent_size + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < agent_size; ++ i)
        {
            if (i % l_step == 0) printf("#");

            stream.read((char *)&point_size, sizeof(int));
            agents[i].resize(point_size * 2);
            stream.read((char *)agents[i].data(), agents[i].size() * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        int agent_size, point_size;

        stream >> agent_size;
        agents.resize(agent_size);

        int l_step = (agent_size + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < agent_size; ++ i)
        {
            if (i % l_step == 0) printf("#");

            stream >> point_size;
            agents[i].resize(point_size * 2);
            for (int j = 0; j < (int)agents[i].size(); ++ j) stream >> agents[i][j];
        }

        stream.close();
    }

    printf("\nFinish loading : %s !\n", path.c_str());
    return true;
}

bool IO::save_agent_path(const std::string & path, const std::vector<std::vector<float>> & agents, bool binary)
{
    printf("saving agent path : %s  ...\n", path.c_str());

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        int agent_size = (int)agents.size();
        stream.write((char *)&agent_size, sizeof(int));

        int l_step = (agent_size + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < agent_size; ++ i)
        {
            if (i % l_step == 0) printf("#");

            int point_size = agents[i].size() / 2;
            stream.write((char *)&point_size, sizeof(int));
            stream.write((char *)agents[i].data(), agents[i].size() * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        int agent_size = (int)agents.size();
        stream << agent_size << std::endl;

        int l_step = (agent_size + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < agent_size; ++ i)
        {
            if (i % l_step == 0) printf("#");

            int point_size = agents[i].size() / 2;
            stream << point_size << std::endl;
            for (int j = 0; j < (int)agents[i].size(); ++ j)
            {
                if (j > 0) stream << " ";
                stream << agents[i][j];
            }
            stream << std::endl;
        }

        stream.close();
    }

    printf("\nFinish saving: %s !\n", path.c_str());
    return true;
}

bool IO::save_point_data(const std::string & path, const std::vector<float> & point, bool binary)
{
    printf("saving data : %s  ... ...\n", path.c_str());

    int point_size = (int)point.size() / 3;

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&point_size, sizeof(int));
        for (int i = 0; i < (int)point.size(); ++ i)
        {
            stream.write((char *)&point[i], sizeof(float));
        }
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << point_size << std::endl;
        for (int i = 0; i < (int)point.size(); i += 3)
        {
            stream << point[i] << " " << point[i + 1] << " " << point[i + 2] << std::endl;
        }
        stream.close();
    }
    return true;
}

bool IO::save_index_data(const std::string & path, const std::vector<int> & index, bool binary)
{
    printf("saving data : %s  ... ...\n", path.c_str());

    int index_size = (int)index.size() >> 1;

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&index_size, sizeof(int));
        for (int i = 0; i < (int)index.size(); ++ i)
        {
            stream.write((char *)&index[i], sizeof(int));
        }
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << index_size << std::endl;
        for (int i = 0; i < (int)index.size(); i += 2)
        {
            stream << index[i] << " " << index[i + 1] << std::endl;
        }
        stream.close();
    }
    return true;
}

bool IO::save_line_data(const std::string & path, const std::vector<float> & point,
    const std::vector<int> & index, bool binary)
{
    printf("saving data : %s  ... ...\n", path.c_str());

    int point_size = (int)point.size() / 3;
    int line_size = (int)index.size() / 2;

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&point_size, sizeof(int));
        stream.write((char *)&line_size, sizeof(int));

        for (int i = 0; i < (int)point.size(); ++ i)
        {
            stream.write((char *)&point[i], sizeof(float));
        }
        for (int i = 0; i < (int)index.size(); ++ i)
        {
            stream.write((char *)&index[i], sizeof(int));
        }
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << point_size << " " << line_size << std::endl;
        for (int i = 0; i < (int)point.size(); i += 3)
        {
            stream << point[i] << " " << point[i + 1] << " " << point[i + 2] << std::endl;
        }
        for (int i = 0; i < (int)index.size(); i += 2)
        {
            stream << index[i] << " " << index[i + 1] << std::endl;
        }
        stream.close();
    }
    return true;
}

bool IO::save_triangle_data(const std::string & path, const std::vector<float> & point,
    const std::vector<int> & index, bool binary)
{
    printf("saving data : %s  ... ...\n", path.c_str());

    int point_size = (int)point.size() / 3;
    int triangle_size = (int)index.size() / 3;

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&point_size, sizeof(int));
        stream.write((char *)&triangle_size, sizeof(int));

        for (int i = 0; i < (int)point.size(); ++ i)
        {
            stream.write((char *)&point[i], sizeof(float));
        }
        for (int i = 0; i < (int)index.size(); ++ i)
        {
            stream.write((char *)&index[i], sizeof(int));
        }
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << point_size << " " << triangle_size << std::endl;
        for (int i = 0; i < (int)point.size(); i += 3)
        {
            stream << point[i] << " " << point[i + 1] << " " << point[i + 2] << std::endl;
        }
        for (int i = 0; i < (int)index.size(); i += 3)
        {
            stream << index[i] << " " << index[i + 1] << " " << index[i + 2] << std::endl;
        }
        stream.close();
    }
    return true;
}

bool IO::load_city_map(const std::string & path, std::vector<std::vector<char>> & data)
{
    data.clear();

    printf("load city map : %s  ... \n", path.c_str());

    std::ifstream stream(path, std::ios::binary);
    if (!stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int width, height;
    stream.read((char *)&width, sizeof(int));
    stream.read((char *)&height, sizeof(int));

    std::vector<char> v(height);
    data.resize(width, v);

    int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
    for (int i = 0; i < width; ++ i)
    {
        if (i % l_step == 0) printf("#");
        stream.read(data[i].data(), height);
    }

    stream.close();

    printf("\nFinish Loading: %s !\n", path.c_str());
    return true;
}

bool IO::save_city_map_data(const std::string & path,
    const std::vector<std::vector<char>> & city_map, std::vector<std::vector<float>> & data)
{
    printf("save city map data : %s ... \n", path.c_str());

    const int width = (int)city_map.size();
    const int height = (int)city_map[0].size();

    QImage city(width, height, QImage::Format_RGB32);
    city.fill(QColor("white"));
    QPainter painter(&city);

    // find the min and max values
    int count = 0;
    double sum = 0.0;
    float max_data = 0.0, min_data = 0.0;

    if (!data.empty())
    {
        for (int i = 0; i < width; ++ i)
        {
            for (int j = 0; j < height; ++ j)
            {
                if (city_map[i][j] == 0)
                {
                    ++ count;
                    sum += data[i][j];
                    if (max_data < data[i][j]) max_data = data[i][j];
                }
            }
        }
    }

    double average = sum / count;
    max_data = average * 20.0;

    bool data_exist = true;
    float div = max_data - min_data;
    if (div < 1e-6)
    {
        div = 1.0;
        data_exist = false;
    }

    int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
    for (int i = 0; i < width; ++ i)
    {
        if (i % l_step == 0) printf("#");
        for (int j = 0; j < height; ++ j)
        {
            if (city_map[i][j] == -1)
            {
                painter.setPen(QColor(29, 131, 72));
                painter.drawPoint(i, j);
            }
            else if (city_map[i][j] == 1)
            {
                painter.setPen(QColor(86, 101, 115));
                painter.drawPoint(i, j);
            }
            else
            {
                if (data_exist)
                {
                    float w = data[i][j];
                    if (max_data < w) w = max_data;

                    float l_r = 0.9, l_g = 1.0, l_b = 0.75;
                    if (w < min_data + div * 0.25)
                    {
                        l_r = 0.0;
                        l_g = 4 * (w - min_data) / div;
                    }
                    else if (w < min_data + div * 0.50)
                    {
                        l_r = 0.0;
                        l_g = 1.0 + 4 * (min_data + div * 0.25 - w) / div;
                    }
                    else if (w < min_data + div * 0.75)
                    {
                        l_r = 4 * (w - min_data - div * 0.50) / div;
                        l_b = 0.0;
                    }
                    else
                    {
                        l_g = 1.0 + 4 * (min_data + div * 0.75 - w) / div;
                        l_b = 0.0;
                    }
                    painter.setPen(QColor(int(l_r * 255), int(l_g * 255), int(l_b * 255)));
                    painter.drawPoint(i, j);
                }
            }
        }
    }

    QString out_path = QString(path.c_str());
    city.save(out_path, "PNG", 0);

    printf("\nFinish Saving : %s !\n", path.c_str());
    return true;
}

bool IO::save_city_map_data(const std::string & path, const std::vector<std::vector<char>> & city_map,
    std::vector<std::vector<float>> & data, const float min_value, const float max_value)
{
    printf("save city map data : %s ... \n", path.c_str());

    const int width = (int)city_map.size();
    const int height = (int)city_map[0].size();

    QImage city(width, height, QImage::Format_RGB32);
    city.fill(QColor("white"));
    QPainter painter(&city);

    bool data_exist = true;
    float div = max_value - min_value;
    if (div < 1e-6)
    {
        div = 1.0;
        data_exist = false;
    }

    int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
    for (int i = 0; i < width; ++ i)
    {
        if (i % l_step == 0) printf("#");
        for (int j = 0; j < height; ++ j)
        {
            if (city_map[i][j] == -1)
            {
                painter.setPen(QColor(29, 131, 72));
                painter.drawPoint(i, j);
            }
            else if (city_map[i][j] == 1)
            {
                painter.setPen(QColor(86, 101, 115));
                painter.drawPoint(i, j);
            }
            else
            {
                if (data_exist)
                {
                    float w = data[i][j];
                    if (max_value < w) w = max_value;
                    if (w < min_value) w = min_value;

                    float l_r = 0.9, l_g = 1.0, l_b = 0.75;
                    if (w < min_value + div * 0.25)
                    {
                        l_r = 0.0;
                        l_g = 4 * (w - min_value) / div;
                    }
                    else if (w < min_value + div * 0.50)
                    {
                        l_r = 0.0;
                        l_g = 1.0 + 4 * (min_value + div * 0.25 - w) / div;
                    }
                    else if (w < min_value + div * 0.75)
                    {
                        l_r = 4 * (w - min_value - div * 0.50) / div;
                        l_b = 0.0;
                    }
                    else
                    {
                        l_g = 1.0 + 4 * (min_value + div * 0.75 - w) / div;
                        l_b = 0.0;
                    }
                    painter.setPen(QColor(int(l_r * 255), int(l_g * 255), int(l_b * 255)));
                    painter.drawPoint(i, j);
                }
            }
        }
    }

    QString out_path = QString(path.c_str());
    city.save(out_path, "PNG", 0);

    printf("\nFinish Saving : %s !\n", path.c_str());
    return true;
}

bool IO::save_city_map_shelter_influence(const std::string & path, const int shelter_number,
    const std::vector<std::vector<char>> & city_map, const std::vector<std::vector<char>> & data)
{
    printf("save city map data : %s ... \n", path.c_str());

    const int width = (int)city_map.size();
    const int height = (int)city_map[0].size();

    QImage city(width, height, QImage::Format_RGB32);
    city.fill(QColor("white"));
    QPainter painter(&city);

    if (data.empty() || city_map.empty()) return false;

    srand(time(NULL));
    std::vector<int> color;
    for (int i = 0; i < shelter_number; ++ i)
    {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        color.push_back((b << 16) | (g << 8) | r);
    }
    std::random_shuffle(color.begin(), color.end());

    int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
    for (int i = 0; i < width && i < CITY_WIDTH; ++ i)
    {
        if (i % l_step == 0) printf("#");
        for (int j = 0; j < height; ++ j)
        {
            if (city_map[i][j] == 1)
            {
                //painter.setPen(QColor(150, 155, 170));
                painter.setPen(QColor(86, 101, 115));
                painter.drawPoint(i, height - j - 1);
            }
            else if (city_map[i][j] == 2)
            {
                painter.setPen(QColor(65, 170, 243));
                painter.drawPoint(i, height - j - 1);
            }
            else
            {
                if (data[i][j] != -1)
                {
                    int id = data[i][j];
                    if (id == -1) continue;
                    int s = color[id];

                    int r = s & 255;
                    int g = (s >> 8) & 255;
                    int b = (s >> 16) & 255;

                    if (city_map[i][j] == -1)
                    {
                        painter.setPen(QColor(r, g, b, 172));
                    }
                    else
                    {
                        painter.setPen(QColor(r, g, b, 96));
                    }
                    painter.drawPoint(i, height - j - 1);
                }
            }
        }
    }

    QString out_path = QString(path.c_str());
    city.save(out_path, "PNG", 0);

    printf("\nFinish Saving : %s !\n", path.c_str());
    return true;
}

bool IO::save_city_space_clearance(const std::string & path,
    const std::vector<std::vector<char>> & city_map, const std::vector<std::vector<float>> & data)
{
    printf("save city map data : %s ... \n", path.c_str());

    const int width = (int)city_map.size();
    const int height = (int)city_map[0].size();

    QImage city(width, height, QImage::Format_RGB32);
    city.fill(QColor("white"));
    QPainter painter(&city);

    if (data.empty() || city_map.empty()) return false;

    const int lit = 32;
    const int inf = 50;

    int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
    for (int i = 0; i < width; ++ i)
    {
        if (i % l_step == 0) printf("#");
        for (int j = 0; j < height; ++ j)
        {
            if (city_map[i][j] == 1)
            {
                painter.setPen(QColor(86, 101, 115));
                painter.drawPoint(i, height - j - 1);
            }
            else if (city_map[i][j] == 2)
            {
                painter.setPen(QColor(133, 192, 233));
                painter.drawPoint(i, height - j - 1);
            }
            else
            {
                if (city_map[i][j] == -1)
                {
                    painter.setPen(QColor(29, 131, 72));
                }
                else
                {
                    int dis = (int)data[i][j];
                    if (dis > inf) dis = inf;
                    painter.setPen(QColor(110, 147, 45, lit + dis * 4));
                    /*
                    if (dis > inf)
                    {
                        painter.setPen(QColor(34, 153, 84, 72));
                    }
                    else
                    {
                        painter.setPen(QColor(243, 156, 18, lit + inf - dis));    // 橙色
                    }
                    */
                }
                painter.drawPoint(i, height - j - 1);
            }
        }
    }

    QString out_path = QString(path.c_str());
    city.save(out_path, "PNG", 0);

    printf("\nFinish Saving : %s !\n", path.c_str());
    return true;
}

bool IO::save_city_image_data(const std::string & path,
    const std::vector<std::vector<char>> & city_map, const std::vector<std::vector<char>> & data)
{
    printf("save city map data : %s ... \n", path.c_str());

    if (data.empty() || city_map.empty()) return false;

    const int width = (int)data.size();
    const int height = (int)data[0].size();

    std::string id = "0";
    for (int c = 0; c < 5; ++ c)
    {
        srand(time(NULL));
        std::vector<int> color;
        const int color_count = 1024;
        for (int i = 0; i < color_count; ++ i)
        {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            color.push_back((b << 16) | (g << 8) | r);
        }
        std::random_shuffle(color.begin(), color.end());

        QImage city(width, height, QImage::Format_RGB32);
        city.fill(QColor("white"));
        QPainter painter(&city);

        int l_step = (std::min(width, CITY_WIDTH) + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width && i < CITY_WIDTH; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                // 如果想看shelter情况，不要这个if
                //if (city_map[i][j] == -1)
                //{
                    //painter.setPen(QColor(29, 131, 72));
                    //painter.drawPoint(i, height - j - 1);
                //}
                if (city_map[i][j] == 1)
                {
                    painter.setPen(QColor(86, 101, 115));
                    painter.drawPoint(i, height - j - 1);
                }
                else if (city_map[i][j] == 2)
                {
                    painter.setPen(QColor(65, 170, 243));
                    painter.drawPoint(i, height - j - 1);
                }
                else
                {
                    if (data[i][j] != -1)
                    {
                        int id = data[i][j];
                        if (id == -1) continue;
                        int s = color[id];

                        int r = s & 255;
                        int g = (s >> 8) & 255;
                        int b = (s >> 16) & 255;

                        if (city_map[i][j] == -1)
                        {
                            painter.setPen(QColor(r, g, b, 172));
                        }
                        else
                        {
                            painter.setPen(QColor(r, g, b, 96));
                        }
                        painter.drawPoint(i, height - j - 1);
                    }
                }
            }
        }

        id[0] = '0' + c;
        std::string l_path = path + "_" + id + ".png";
        QString out_path = QString(l_path.c_str());
        city.save(out_path, "PNG", 0);

        printf("\nFinish Saving : %s !\n", path.c_str());
    }
    return true;
}

bool IO::save_damage_image_data(const std::string & path,
    const std::vector<std::vector<char>> & city_map, const std::vector<std::vector<char>> & data)
{
    printf("save city map data : %s ... \n", path.c_str());

    if (data.empty() || city_map.empty()) return false;

    const int width = (int)data.size();
    const int height = (int)data[0].size();

    std::string id = "0";
    for (int c = 0; c < 3; ++ c)
    {
        srand(time(NULL));
        std::vector<int> color;
        const int color_count = 1024;
        for (int i = 0; i < color_count; ++ i)
        {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            color.push_back((b << 16) | (g << 8) | r);
        }
        std::random_shuffle(color.begin(), color.end());

        QImage city(width, height, QImage::Format_RGB32);
        city.fill(QColor("white"));
        QPainter painter(&city);

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                if (city_map[i][j] == -1)
                {
                    painter.setPen(QColor(29, 131, 72));
                    painter.drawPoint(i, height - j - 1);
                }
                else if (city_map[i][j] == 1)
                {
                    painter.setPen(QColor(86, 101, 115));
                    painter.drawPoint(i, height - j - 1);
                }
                else if (city_map[i][j] == 2)
                {
                    painter.setPen(QColor(65, 170, 243));
                    painter.drawPoint(i, height - j - 1);
                }
                else
                {
                    if (data[i][j] != -1 && data[i][j] != 0)
                    {
                        int id = data[i][j];
                        if (id == -1) continue;
                        int s = color[id];

                        int r = s & 255;
                        int g = (s >> 8) & 255;
                        int b = (s >> 16) & 255;

                        painter.setPen(QColor(r, g, b));
                        painter.drawPoint(i, height - j - 1);
                    }
                }
            }
        }

        id[0] = '0' + c;
        std::string l_path = path + "_" + id + ".png";
        QString out_path = QString(l_path.c_str());
        city.save(out_path, "PNG", 0);

        printf("\nFinish Saving : %s !\n", path.c_str());
    }
    return true;
}

bool IO::load_city_grid_data(const std::string & path, std::vector<std::vector<char>> & city, bool binary)
{
    city.clear();

    printf("Loading city grid data: %s  ...\n", path.c_str());

    int width, height;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&width, sizeof(int));
        stream.read((char *)&height, sizeof(int));

        city.resize(width, std::vector<char>(height));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.read((char *)city[i].data(), height * sizeof(char));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> width >> height;

        city.resize(width, std::vector<char>(height));

        int c;
        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                stream >> c;
                city[i][j] = c;
            }
        }

        stream.close();
    }

    printf("\nFinish loading : %s !\n", path.c_str());
    return true;
}

bool IO::load_city_grid_data(const std::string & path, std::vector<std::vector<int>> & city, bool binary)
{
    city.clear();

    printf("Loading city grid data: %s  ...\n", path.c_str());

    int width, height;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&width, sizeof(int));
        stream.read((char *)&height, sizeof(int));

        city.resize(width, std::vector<int>(height));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.read((char *)city[i].data(), height * sizeof(int));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> width >> height;

        city.resize(width, std::vector<int>(height));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j) stream >> city[i][j];
        }

        stream.close();
    }

    printf("\nFinish loading : %s !\n", path.c_str());
    return true;
}

bool IO::load_city_grid_data(const std::string & path, std::vector<std::vector<float>> & city, bool binary)
{
    city.clear();

    printf("Loading city grid data: %s  ...\n", path.c_str());

    int width, height;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&width, sizeof(int));
        stream.read((char *)&height, sizeof(int));

        city.resize(width, std::vector<float>(height));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.read((char *)city[i].data(), height * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> width >> height;

        city.resize(width, std::vector<float>(height));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j) stream >> city[i][j];
        }

        stream.close();
    }

    printf("\nFinish loading : %s !\n", path.c_str());
    return true;
}

bool IO::save_city_grid_data(const std::string & path, const std::vector<std::vector<char>> & city, bool binary)
{
    printf("Saving grid data : %s  ...\n", path.c_str());
    if (city.empty()) return false;

    int width = (int)city.size();
    int height = (int)city[0].size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&width, sizeof(int));
        stream.write((char *)&height, sizeof(int));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.write((char *)city[i].data(), height * sizeof(char));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << width << " " << height << std::endl;

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << city[i][j];
            }
            stream << std::endl;
        }

        stream.close();
    }

    printf("\nFinish saving: %s !\n", path.c_str());
    return true;
}

bool IO::save_city_grid_data(const std::string & path, const std::vector<std::vector<int>> & city, bool binary)
{
    printf("Saving grid data : %s  ...\n", path.c_str());
    if (city.empty()) return false;

    int width = (int)city.size();
    int height = (int)city[0].size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&width, sizeof(int));
        stream.write((char *)&height, sizeof(int));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.write((char *)city[i].data(), height * sizeof(int));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << width << " " << height << std::endl;

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << city[i][j];
            }
            stream << std::endl;
        }

        stream.close();
    }

    printf("\nFinish saving: %s !\n", path.c_str());
    return true;
}

bool IO::save_city_grid_data(const std::string & path, const std::vector<std::vector<float>> & city, bool binary)
{
    printf("Saving grid data : %s  ...\n", path.c_str());
    if (city.empty()) return false;

    int width = (int)city.size();
    int height = (int)city[0].size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }
        
        stream.write((char *)&width, sizeof(int));
        stream.write((char *)&height, sizeof(int));

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            stream.write((char *)city[i].data(), height * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << width << " " << height << std::endl;

        int l_step = (width + PROCESS_STEP - 1) / PROCESS_STEP;
        for (int i = 0; i < width; ++ i)
        {
            if (i % l_step == 0) printf("#");
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << city[i][j];
            }
            stream << std::endl;
        }

        stream.close();
    }

    printf("\nFinish saving: %s !\n", path.c_str());
    return true;
}

bool IO::load_road_attribute_data(const std::string & path, std::vector<char> & data, bool binary)
{
    data.clear();

    printf("Loading road attribute : %s  ...\n", path.c_str());

    int data_size;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&data_size, sizeof(int));
        data.resize(data_size);

        stream.read((char *)data.data(), data.size() * sizeof(char));
        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> data_size;
        data.resize(data_size);

        int w;
        for (int i = 0; i < data_size; ++ i)
        {
            stream >> w;
            data[i] = w;
        }
        stream.close();
    }

    printf("Finish loading : %s  ...\n", path.c_str());
    return true;
}

bool IO::load_road_attribute_data(const std::string & path, std::vector<int> & data, bool binary)
{
    data.clear();

    printf("Loading road attribute : %s  ...\n", path.c_str());

    int data_size;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&data_size, sizeof(int));
        data.resize(data_size);

        stream.read((char *)data.data(), data.size() * sizeof(int));
        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> data_size;
        data.resize(data_size);

        for (int i = 0; i < data_size; ++ i) stream >> data[i];
        stream.close();
    }

    printf("Finish loading : %s  ...\n", path.c_str());
    return true;
}

bool IO::load_road_attribute_data(const std::string & path, std::vector<float> & data, bool binary)
{
    data.clear();

    printf("Loading road attribute : %s  ...\n", path.c_str());

    int data_size;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream.read((char *)&data_size, sizeof(int));
        data.resize(data_size);

        stream.read((char *)data.data(), data.size() * sizeof(float));
        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good())
        {
            printf("Load %s faild !\n", path.c_str());
            return false;
        }

        stream >> data_size;
        data.resize(data_size);

        for (int i = 0; i < data_size; ++ i) stream >> data[i];
        stream.close();
    }

    printf("Finish loading : %s  ...\n", path.c_str());
    return true;
}

bool IO::save_road_attribute_data(const std::string & path, const std::vector<char> & data, bool binary)
{
    printf("Saving road attribute : %s\n", path.c_str());
    if (data.empty())
    {
        printf("Attribute data is empty !\n");
        return false;
    }

    int data_size = (int)data.size();
    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&data_size, sizeof(int));
        stream.write((char *)data.data(), data_size * sizeof(char));
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << data_size << std::endl;
        for (int i = 0; i < data_size; ++ i) stream << data[i] << std::endl;
        stream.close();
    }

    printf("Finish saving: %s !\n", path.c_str());
    return true;
}

bool IO::save_road_attribute_data(const std::string & path, const std::vector<int> & data, bool binary)
{
    printf("Saving road attribute : %s\n", path.c_str());
    if (data.empty())
    {
        printf("Attribute data is empty !\n");
        return false;
    }

    int data_size = (int)data.size();
    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&data_size, sizeof(int));
        stream.write((char *)data.data(), data_size * sizeof(int));
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << data_size << std::endl;
        for (int i = 0; i < data_size; ++ i) stream << data[i] << std::endl;
        stream.close();
    }

    printf("Finish saving: %s !\n", path.c_str());
    return true;
}

bool IO::save_road_attribute_data(const std::string & path, const std::vector<float> & data, bool binary)
{
    printf("Saving road attribute : %s\n", path.c_str());
    if (data.empty())
    {
        printf("Attribute data is empty !\n");
        return false;
    }

    int data_size = (int)data.size();
    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream.write((char *)&data_size, sizeof(int));
        stream.write((char *)data.data(), data_size * sizeof(float));
        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good())
        {
            printf("Save %s faild !\n", path.c_str());
            return false;
        }

        stream << data_size << std::endl;
        for (int i = 0; i < data_size; ++ i) stream << data[i] << std::endl;
        stream.close();
    }

    printf("Finish saving: %s !\n", path.c_str());
    return true;
}

bool IO::load_shelters(const std::string & path, std::vector<Shelter> & shelter)
{
    shelter.clear();

    printf("loading shelter data : %s  ...\n", path.c_str());
    shelter.clear();

    std::ifstream stream(path);
    if (!stream.good())
    {
        printf("Load %s faild !\n", path.c_str());
        return false;
    }

    int number_shelter;
    stream >> number_shelter;

    for (int k = 0; k < number_shelter; ++ k)
    {
        Shelter she;
        int l_id, area, number_ids, number_main_ids;
        stream >> area >> number_ids >> number_main_ids;

        she.area = area;
        she.initialize();

        for (int i = 0; i < number_ids; ++ i)
        {
            stream >> l_id;
            she.ids.push_back(l_id);
        }

        for (int i = 0; i < number_main_ids; ++ i)
        {
            stream >> l_id;
            she.main_ids.push_back(l_id);
        }
        shelter.push_back(she);
    }

    stream.close();

    printf("Finish loading: %s !\n", path.c_str());
    return true;
}

bool IO::save_shelters(const std::string & path, const std::vector<Shelter> shelter)
{
     printf("Saving shelter data : %s  ...\n", path.c_str());
     if (shelter.empty()) return false;

     std::ofstream stream(path);
     if (!stream.good())
     {
         printf("Save %s faild !\n", path.c_str());
         return false;
     }

     const int number_shelter = (int)shelter.size();
     stream << number_shelter << std::endl;

     for (int k = 0; k < number_shelter; ++ k)
     {
         if (k) stream << std::endl;
         int area = shelter[k].area;
         int number_ids = (int)shelter[k].ids.size();
         int number_main_ids = (int)shelter[k].main_ids.size();

         stream << area << " " << number_ids << " " << number_main_ids << std::endl;

         for (int i = 0; i < number_ids; ++ i)
         {
             if (i) stream << " ";
             stream << shelter[k].ids[i];
         }
         if (number_ids) stream << std::endl;

         for (int i = 0; i < number_main_ids; ++ i)
         {
             if (i) stream << " ";
             stream << shelter[k].main_ids[i];
         }
         if (number_main_ids) stream << std::endl;
     }

     stream.close();

     printf("Finish saving: %s !\n", path.c_str());
    return true;
}




