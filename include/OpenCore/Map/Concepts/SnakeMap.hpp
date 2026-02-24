#pragma once

// SnakeMap.hpp
// 链式地图

#include <string>

class SnakeMap
{
  public:
    SnakeMap(short id, const std::string &path)
    {
        this->id = id;
        this->MapPath = path;
    }

    bool save();
    bool load(int x, int y);

    int getDistance(int x, int y);

  private:
    short id;
    std::string MapPath;

    // cur_Distance 当前缓冲区头部相距0,0坐标的链式距离
    // buffer_size 缓冲区的大小(每个缓冲区作为一个单独的链式区块)
    int cur_Distance = 0;
    int buffer_size = 25;
    // 这里的逻辑是这样的，一旦我们发现玩家移动到了新的缓冲区块，则立即触发区块加载
};