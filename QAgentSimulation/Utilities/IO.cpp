#include "IO.h"

bool IO::file_exist(const std::string & path)
{
    std::ifstream ifs(path, std::ifstream::in);
    return ifs.good();
}

bool IO::load_shader(const std::string & path, std::vector<char> & data)
{
    // read shader source
    std::ifstream l_shader_file_stream(path);
    if (!l_shader_file_stream.good()) return false;

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
    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good()) return false;

    int l_point_size;
    l_stream.read((char *)&l_point_size, sizeof(int));

    point.resize(l_point_size * 3);

    l_stream.read((char *)point.data(), point.size() * sizeof(float));

    l_stream.close();

    return true;
}

bool IO::load_line_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
{
    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good()) return false;

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
    std::ifstream l_stream(path, std::ios::binary);
    if (!l_stream.good()) return false;

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
    printf("loading damage file : %s ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good()) return false;

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
            for (int i = 0; i < y_count; ++ i)
            {
                if (i > 0 && i % 500 == 0) printf("%.2lf%% ...\n", i * 100.0 / y_count);
                for (int j = 0; j < x_count; ++ j)
                {
                    l_stream >> l_t;
                    data[j][y_count - i - 1] = (char)l_t;
                }
            }
        }
    }
    l_stream.close();
    printf("Finish loading : %s ...\n", path.c_str());
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
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

        stream << width << " " << height << std::endl;
        for (int i = 0; i < width; ++ i)
        {
            if (i > 0 && i % 500 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << (int)data[i][j];
            }
            stream << std::endl;
        }
        stream.close();
    }
    printf("Finish saving : %s  ... \n", path.c_str());
    return true;
}

bool IO::load_graph_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
{
    printf("loading graph file : %s  ... ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good()) return false;

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
    printf("Finish loading graph file : %s\n", path.c_str());
    return true;
}

bool IO::load_graph_attribute(const std::string & path, const std::string & name, std::vector<float> & data)
{
    printf("loading graph file : %s  ... ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good()) return false;

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
    printf("Finish loading graph file : %s\n", path.c_str());
    return true;
}

bool IO::load_agent_data(const std::string & path, std::vector<float> & points, std::vector<int> & ids, bool binary)
{
    printf("loading agent : %s  ... ", path.c_str());

    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

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
    printf("loading agent : %s  ... ", path.c_str());

    int point_size = (int)ids.size();

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good()) return false; 

        stream.write((char *)&point_size, sizeof(int));

        stream.write((char *)points.data(), points.size() * sizeof(float));
        stream.write((char *)ids.data(), ids.size() * sizeof(int));

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good()) return false;

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
    printf("Loading agent path : %s  ...\n", path.c_str());

    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good()) return false;

        int agent_size, point_size;
        stream.read((char *)&agent_size, sizeof(int));
        agents.resize(agent_size);

        for (int i = 0; i < agent_size; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / agent_size);

            stream.read((char *)&point_size, sizeof(int));
            agents[i].resize(point_size * 2);
            stream.read((char *)agents[i].data(), agents[i].size() * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good()) return false;

        int agent_size, point_size;

        stream >> agent_size;
        agents.resize(agent_size);

        for (int i = 0; i < agent_size; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / agent_size);

            stream >> point_size;
            agents[i].resize(point_size * 2);
            for (int j = 0; j < (int)agents[i].size(); ++ j) stream >> agents[i][j];
        }

        stream.close();
    }

    printf("Finish loading : %s  ...\n", path.c_str());
    return true;
}

bool IO::save_agent_path(const std::string & path, const std::vector<std::vector<float>> & agents, bool binary)
{
    printf("saving agent path : %s  ...\n", path.c_str());

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good()) return false; 

        int agent_size = (int)agents.size();
        stream.write((char *)&agent_size, sizeof(int));

        for (int i = 0; i < agent_size; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / agent_size);

            int point_size = agents[i].size() / 2;
            stream.write((char *)&point_size, sizeof(int));
            stream.write((char *)agents[i].data(), agents[i].size() * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good()) return false;

        int agent_size = (int)agents.size();
        stream << agent_size << std::endl;

        for (int i = 0; i < agent_size; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / agent_size);

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

    printf("Finish saving: %s  ...\n", path.c_str());
    return true;
}

bool IO::save_point_data(const std::string & path, const std::vector<float> & point, bool binary)
{
    printf("saving data : %s  ... ...\n", path.c_str());

    int point_size = (int)point.size() / 3;

    if (binary)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

        stream << point_size << std::endl;
        for (int i = 0; i < (int)point.size(); i += 3)
        {
            stream << point[i] << " " << point[i + 1] << " " << point[i + 2] << std::endl;
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
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

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
        if (!stream.good()) return false;

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
    printf("load city map : %s  ... ", path.c_str());

    std::ifstream stream(path, std::ios::binary);
    if (!stream.good()) return false;

    int width, height;
    stream.read((char *)&width, sizeof(int));
    stream.read((char *)&height, sizeof(int));

    std::vector<char> v(height);
    data.resize(width, v);

    for (int i = 0; i < width; ++ i) stream.read(data[i].data(), height);

    stream.close();

    printf("Done!\n");
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

    for (int i = 0; i < width; ++ i)
    {
        if (i > 0 && i % 500 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
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

    printf("Finish Saving : %s ... \n", path.c_str());
    return true;
}

bool IO::load_city_grid_data(const std::string & path, std::vector<std::vector<float>> & city, bool binary)
{
    printf("Loading city grid data: %s  ...\n", path.c_str());

    int width, height;
    if (binary)
    {
        std::ifstream stream(path, std::ios::binary);
        if (!stream.good()) return false;

        stream.read((char *)&width, sizeof(int));
        stream.read((char *)&height, sizeof(int));

        city.resize(width, std::vector<float>(height));

        for (int i = 0; i < width; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
            stream.read((char *)city[i].data(), height * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ifstream stream(path);
        if (!stream.good()) return false;

        stream >> width >> height;

        city.resize(width, std::vector<float>(height));

        for (int i = 0; i < width; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
            for (int j = 0; j < height; ++ j) stream >> city[i][j];
        }

        stream.close();
    }

    printf("Finish loading : %s  ...\n", path.c_str());
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
        if (!stream.good()) return false; 

        
        stream.write((char *)&width, sizeof(int));
        stream.write((char *)&height, sizeof(int));

        for (int i = 0; i < width; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
            stream.write((char *)city[i].data(), height * sizeof(float));
        }

        stream.close();
    }
    else
    {
        std::ofstream stream(path);
        if (!stream.good()) return false;

        stream << width << " " << height << std::endl;

        for (int i = 0; i < width; ++ i)
        {
            if (i > 0 && i % 10000 == 0) printf("%.2lf%% ...\n", i * 100.0 / width);
            for (int j = 0; j < height; ++ j)
            {
                if (j > 0) stream << " ";
                stream << city[i][j];
            }
            stream << std::endl;
        }

        stream.close();
    }

    printf("Finish saving: %s  ...\n", path.c_str());
    return true;
}
