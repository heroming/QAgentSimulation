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
