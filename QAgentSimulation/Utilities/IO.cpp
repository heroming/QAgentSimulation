#include "IO.h"

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
    printf("loading damage file : %s  ... ...\n", path.c_str());
    std::ifstream l_stream(path);
    if (!l_stream.good()) return false;

    int l_x_count, l_y_count;
    std::string line;
    while (std::getline(l_stream, line)) 
    {
        if (line.find("X_COORDINATES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_x_count >> line;
            x_coordinate.resize(l_x_count);
            for (int i = 0; i < l_x_count; ++ i) l_stream >> x_coordinate[i];
        }
        else if (line.find("Y_COORDINATES") == 0)
        {
            std::stringstream ss(line);
            ss >> line >> l_y_count >> line;
            y_coordinate.resize(l_y_count);
            for (int i = 0; i < l_y_count; ++ i) l_stream >> y_coordinate[i];
        }
        else if (line.find("obstacle_data") == 0)
        {
            int l_t;
            -- l_x_count;
            -- l_y_count;
            std::vector<char> l_v(l_x_count, 0);
            data.resize(l_y_count, l_v);
            for (int i = 0; i < l_y_count; ++ i)
            {
                if (i % 100 == 0) printf("%.2lf%% ...\n", i * 100.0 / l_y_count);
                for (int j = 0; j < l_x_count; ++ j)
                {
                    l_stream >> l_t;
                    data[i][j] = (char)l_t;
                }
            }
        }
    }
    l_stream.close();
    printf("Finish loading damage file : %s\n", path.c_str());
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

bool IO::load_graph_atrribute(const std::string & path, const std::string & name, std::vector<float> & data)
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
