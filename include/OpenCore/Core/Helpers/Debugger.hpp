/**
 * @file Debugger.hpp
 * @author your name (you@domain.com)
 * @brief 调试相关，会打印日志
 * @version 0.1
 * @date 2026-04-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include <SDL2/SDL_log.h>

#include "OpenCore/Core/Macros.hpp"

inline void Console_Log(const char *fmt, ...)
{
#if defined(LOG_ENABLED)
    va_list args;
    va_start(args, fmt);

    // 四个参数都提供
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt,
                    args);

    va_end(args);
#endif
    // 此处为宏，如果没有LOG_ENABLED宏存在，那么就在编译阶段优化这个函数
}
