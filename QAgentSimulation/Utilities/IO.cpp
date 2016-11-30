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

bool IO::load_data(const std::string & path, std::vector<float> & point, std::vector<int> & index)
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

bool IO::load_data(const std::string & path, std::vector<float> & point)
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

bool IO::load_damage(const std::string & path, std::vector<int> & x_coordinate, std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data)
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
    printf("Finish loading damage file : %s\n", path.c_str());
    return true;
}
