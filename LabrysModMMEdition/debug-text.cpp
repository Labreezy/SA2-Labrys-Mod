#include "pch.h"
#include <string>


char texPosY = 4;
int DebugMessageTimer;
std::string debugText;

void SetDebugFontColor(int color) {
	return HelperFunctionsGlobal.SetDebugFontColor(color);
}

void DisplayDebugStringFormatted(int loc, const char* Format, ...) {
	return HelperFunctionsGlobal.DisplayDebugStringFormatted(loc, Format);
}

void Init_DebugText() {
	SetDebugFontColor(0xFFFF00FF);	
}

void DisplayTimed_DebugMessage_OnFrames()
{
	if (DebugMessageTimer && debugText != "")
	{
		SetDebugFontColor(0xFFBFBFBF);
		DisplayDebugStringFormatted(NJM_LOCATION(1, 2), debugText.c_str());
		SetDebugFontColor(0xFFBFBFBF);
		DebugMessageTimer--;
	}
}

void SendTimedDebugMessage(std::string msg, int timer)
{
	DebugMessageTimer = timer;
	debugText = msg;
}

void UpdateDebugText_OnFrame() {
	if (DebugMessageTimer && debugText != "") {
		DisplayDebugStringFormatted(NJM_LOCATION(25, 10), debugText.c_str());
		DebugMessageTimer--;
	}
}
