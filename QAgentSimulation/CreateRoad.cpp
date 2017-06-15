#include "CreateRoad.h"

#include "Utilities/IO.h"
#include "Utilities/Tool.h"

const int MIN_X = 558;
const int MIN_Y = 59180;

std::string CreateRoad::get_init_path() const
{
    return m_init_path;
}

void CreateRoad::set_init_path(const std::string & s)
{
    m_init_path = s;
}

std::string CreateRoad::get_save_path() const
{
    return m_save_path;
}

void CreateRoad::set_save_path(const std::string & s)
{
    m_save_path = s;
}

int CreateRoad::get_radius() const
{
    return m_radius;
}

void CreateRoad::set_radius(const int x)
{
    m_radius = x;
}

int CreateRoad::get_length() const
{
    return m_length;
}

void CreateRoad::set_length(const int x)
{
    m_length = x;
}

float CreateRoad::get_x()
{
    return m_x;
}

float CreateRoad::get_y()
{
    return m_y;
}

bool CreateRoad::is_begin() const
{
    return m_begin;
}

void CreateRoad::set_begin(const bool flag)
{
    m_begin = flag;
}

const std::vector<int> & CreateRoad::get_index() const
{
    return m_index;
}

const std::vector<float> & CreateRoad::get_point() const
{
    return m_point;
}

void CreateRoad::load_data(const std::string & l_path)
{
    m_point.clear();
    m_index.clear();
    if (! l_path.empty())
    {
        IO::load_line_data(l_path, m_point, m_index);
        m_point_id = (int)m_point.size() / 3;
        m_use.resize(m_point_id, 0);

        for (int k = 0; k < m_point_id; ++ k)
        {
            ++ m_use[k];
            if (m_use[k] == 1)
            {
                update_vertex_id(m_point[k * 3], m_point[k * 3 + 1], k);
            }
        }
    }
}

void CreateRoad::point_clicked(const float l_x, const float l_y)
{
    printf("%.2f -- %.2f\n", l_x, l_y);

    // 检测是否在地图内，是否在空白区域或者安全区
    int x = (int)l_x - MIN_X, y = (int)l_y - MIN_Y;
    if (! valid(x, y) || m_grid[x][y] >= 1) return;

    int click_id = get_vertex_id(l_x, l_y);
    printf("Click : (%.2f, %.2f), id = %d\n", l_x, l_y, click_id);

    if (! m_begin)
    {
        float len = Tool::distance(m_x, m_y, l_x, l_y);
        if (len * 2 < m_length) return;

        int part = ((int)len + m_length - 1) / m_length + 1;

        printf("len : %.2f, part : %d\n", len, part);

        int pre_id = -1;
        float dx = 0, dy = 0;
        float nx, ny;

        for (int k = 0; k <= part; ++ k)
        {
            float x0 = m_x, y0 = m_y;
            if (pre_id != -1)
            {
                x0 = m_point[pre_id * 3];
                y0 = m_point[pre_id * 3 + 1];
            }

            nx = x0 + dx;
            ny = y0 + dy;

            int l_id = get_vertex_id(nx, ny);
            if (l_id == -1)
            {
                l_id = m_point_id ++;
                m_use.push_back(0);

                get_vertex_position(nx, ny);
                m_point.push_back(nx);
                m_point.push_back(ny);
                m_point.push_back(0.0);
                update_vertex_id(nx, ny, l_id);
            }
            if (pre_id != -1 && l_id != pre_id)
            {
                ++ m_use[l_id];
                ++ m_use[pre_id];
                m_index.push_back(pre_id);
                m_index.push_back(l_id);

                float x1 = m_point[l_id * 3], y1 = m_point[l_id * 3 + 1];
                if (k < part)
                {
                    // 使得路径均匀，防止前面路径太长导致后面路径很短
                    float dis = Tool::distance(x1, y1, l_x, l_y);
                    if (part - k > 1 && dis / (part - k) * 2 < m_length)
                    {
                        ++ k;
                    }
                    dx = (l_x - x1) / (part - k);
                    dy = (l_y - y1) / (part - k);
                }
            }
            if (pre_id == -1 && k < part)
            {
                dx = (l_x - nx) / (part - k);
                dy = (l_y - ny) / (part - k);
            }
            pre_id = l_id;
        }
    }
    else
    {
        m_begin = !m_begin;
    }
    m_x = l_x, m_y = l_y;
}

void CreateRoad::delete_recent()
{
    if (m_index.empty()) return;
    int a = m_index.back();
    m_index.pop_back();
    int b = m_index.back();
    m_index.pop_back();

    if (-- m_use[a] == 0)
    {
        int k = (int)m_point.size() - 3;
        if (k >= 0) update_vertex_id(m_point[k], m_point[k + 1], -1);
        m_point.pop_back();
        m_point.pop_back();
        m_point.pop_back();
        m_use.pop_back();
        -- m_point_id;
    }
    if (-- m_use[b] == 0)
    {
        int k = (int)m_point.size() - 3;
        if (k >= 0) update_vertex_id(m_point[k], m_point[k + 1], -1);
        m_point.pop_back();
        m_point.pop_back();
        m_point.pop_back();
        m_use.pop_back();
        -- m_point_id;
    }
}


void CreateRoad::clear()
{
    const std::string GRID_PATH = "./Data/city_grid.dat";
    const std::string CLREANCE_PATH = "./Data/grid/space_clearance.dat";

    m_begin = 1;
    m_x = m_y = 0.0;
    m_point_id = 0;
    m_radius = 0;
    m_length = 0;
    m_init_path = "";
    m_save_path = "";

    m_grid.clear();
    // 导入city map数据(-1 for shelter, 0 for road and 1 for obstacle)
    IO::load_city_map(GRID_PATH, m_grid);
    IO::load_city_grid_data(CLREANCE_PATH, m_clearance, true);

    m_row = (int)m_grid.size();
    m_col = (int)m_grid[0].size();
    m_use.clear();
    m_vert.resize(m_row, std::vector<int>(m_col, -1));
}

void CreateRoad::save() const
{
    const std::string l_save_dat = m_save_path + ".dat";
    IO::save_line_data(l_save_dat, m_point, m_index, true);
    const std::string l_save_txt = m_save_path + ".txt";
    IO::save_line_data(l_save_txt, m_point, m_index);
}


bool CreateRoad::valid(const int x, const int y) const
{
    return x >= 0 && x < m_row && y >= 0 && y < m_col;
}

bool CreateRoad::valid(const float x, const float y) const
{
    int l_x = (int)x - MIN_X;
    int l_y = (int)y - MIN_Y;
    return valid(l_x, l_y);
}

int CreateRoad::get_vertex_id(const float x, const float y) const
{
    int a = (int)x - MIN_X;
    int b = (int)y - MIN_Y;
    if (valid(a, b)) return m_vert[a][b];
    return -1;
}

void CreateRoad::update_vertex_id(const float x, const float y, const int id)
{
    int ix = (int)x - MIN_X;
    int iy = (int)y - MIN_Y;
    for (int a = ix - m_radius; a <= ix + m_radius; ++ a)
        for (int b = iy - m_radius; b <= iy + m_radius; ++ b)
            if (valid(a, b))
            {
                m_vert[a][b] = id;
            }
}

void CreateRoad::get_vertex_position(float & x, float & y) const
{
    int space = 0;
    const float l_x = x;
    const float l_y = y;
    for (int a = -m_radius; a <= m_radius; ++ a)
        for (int b = -m_radius; b <= m_radius; ++ b)
        {
            int px = (int)l_x + a - MIN_X;
            int py = (int)l_y + b - MIN_Y;
            if (valid(px, py))
            {
                int w = get_point_space_by_clearance(px, py);
                if (space < w)
                {
                    space = w;
                    x = l_x + a;
                    y = l_y + b;
                }
            }
        }
}

int CreateRoad::get_point_space_by_clearance(const int x, const int y) const
{
    return m_clearance[x][y];
}

int CreateRoad::get_point_space(const int x, const int y) const
{
    static const int MIN_CLEARANCE = 1;
    static const int MAX_CLEARANCE = 32;
    int l = MIN_CLEARANCE, r = MAX_CLEARANCE;
    while (l <= r)
    {
        int m = (l + r) >> 1;
        if (check_space(x, y, m))
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

bool CreateRoad::check_space(const int x, const int y, const int r) const
{
    for (int i = -r; i <= r; ++ i)
    {
        if (! valid(x + i, y - r) || m_grid[x + i][y - r] >= 1) return 0;
        if (! valid(x + i, y + r) || m_grid[x + i][y + r] >= 1) return 0;
        if (! valid(x - r, y + i) || m_grid[x - r][y + i] >= 1) return 0;
        if (! valid(x + r, y + i) || m_grid[x + r][y + i] >= 1) return 0;
    }
    return 1;
}


