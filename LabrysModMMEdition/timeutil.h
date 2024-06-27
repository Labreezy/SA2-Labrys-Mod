#pragma once
#include "pch.h"
#include <string>
#include <ctime>

std::string safe_strftime(const char* fmt, const std::tm* t);
