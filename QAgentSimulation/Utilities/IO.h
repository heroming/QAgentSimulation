#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class IO
{
public:
    static bool load_shader(const std::string & path, std::vector<char> & data);
    static bool load_point_data(const std::string & path, std::vector<float> & point);
    static bool load_line_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_triangle_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);


    static bool load_damage(const std::string & path, std::vector<int> & x_coordinate,
        std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data);
    static bool load_graph_data(const std::string & path,
        std::vector<float> & point, std::vector<int> & index);
    static bool load_graph_atrribute(const std::string & path,
        const std::string & name, std::vector<float> & data);


    static bool save_point_data(const std::string & path,
        const std::vector<float> & point, bool binary = false);
    static bool save_line_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);
    static bool save_triangle_data(const std::string & path,
        const std::vector<float> & point, const std::vector<int> & index, bool binary = false);
};