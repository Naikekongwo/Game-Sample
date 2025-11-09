#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

int16_t ItemConsumable::changeAmount(uint8_t src, int16_t offset)
{
    int16_t low = src + offset - getMaxSize();
    // 若low为负数：代表未及堆栈数量极限
    // 若low为正数，代表已经超出
    return (low>0)?(0-low):(getMaxSize() - low);
}