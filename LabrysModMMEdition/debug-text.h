#pragma once
#include "pch.h"
#include <string>

void SetDebugFontColor(int color);
void DisplayDebugStringFormatted(int loc, const char* Format, ...);
void SendTimedDebugMessage(std::string msg, int timer);
void Init_DebugText();
void DisplayTimed_DebugMessage_OnFrames();