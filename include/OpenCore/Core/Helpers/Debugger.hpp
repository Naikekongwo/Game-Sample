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

// Debugger.hpp
#include <utility>
#pragma once

#include <SDL2/SDL_log.h>
#include <format>
#include <source_location>
#include <string>

#include "OpenCore/Core/Macros.hpp"

using std::format;
using std::source_location;
using std::string;
using std::vformat;

/**
 * @brief 自动填充路径的日志打印方法(带参数)
 *
 */
template <typename... Args>
inline void AutoLog(const source_location &location, const char *fmt,
                    Args &&...args)
{
#if defined(LOG_ENABLED)
    string msgBody = vformat(fmt, std::make_format_args(args...));

    string funcName = location.function_name();
    size_t leftLimit = funcName.find_last_of('\(');
    string leftPart = funcName.substr(0, leftLimit);
    size_t funcStart = leftPart.find_last_of(' ');
    string prefix = format(
        "{}() ", funcName.substr(funcStart + 1, leftLimit - funcStart - 1));

    // 删除类型名称得到方法名称，作为前缀

    msgBody = prefix + msgBody;

    SDL_Log("%s", msgBody.c_str());
#endif
}

#define LOG(fmt, ...)                                                          \
    AutoLog(std::source_location::current(), fmt, ##__VA_ARGS__)
