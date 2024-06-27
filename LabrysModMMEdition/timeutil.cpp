#include "pch.h"
#include "timeutil.h"
#include <memory>


std::string safe_strftime(const char* fmt, const std::tm* t)
{
    std::size_t len = 10; // Adjust initial length as desired. Maybe based on the length of fmt?
    auto buff = std::make_unique<char[]>(len);
    while (std::strftime(buff.get(), len, fmt, t) == 0) {
        len *= 2;
        buff = std::make_unique<char[]>(len);
    }
    return std::string{ buff.get() };
}

