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
    static bool load_data(const std::string & path, std::vector<float> & point);
    static bool load_data(const std::string & path, std::vector<float> & point, std::vector<int> & index);
    static bool load_damage(const std::string & path, std::vector<int> & x_coordinate, std::vector<int> & y_coordinate, std::vector<std::vector<char>> & data);
};