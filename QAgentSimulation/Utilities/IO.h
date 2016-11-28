#pragma once

#include <fstream>
#include <iostream>
#include <vector>

class IO
{
public:
    static bool load_shader(const std::string & path, std::vector<char> & data);
    static bool load_city_building(const std::string & path, std::vector<float> & point, std::vector<int> & index);
};