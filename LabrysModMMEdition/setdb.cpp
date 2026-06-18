#include "pch.h"
#include "setdb.h"
#include "SA2Functions.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "debug-text.h"

static std::vector<TimeEntry> timeEntryDatabase;
static std::string db_path;
LPCSTR sfx_filename;
int EMBLEM_SE = 0x8006;
int CASH_REGISTER_SE = 0x8004;
int SE_ID = -1;
bool SFX_ON = false;
void loadEntryDB(std::string fpath, std::string sfxname)
{
	db_path = fpath;
	std::ifstream csvdb_f(db_path.c_str());
	std::string line;
	
	
	while (std::getline(csvdb_f, line)) {
		std::vector<std::string> line_tokens;
		std::string token;
		if (!line.empty()) {
			PrintDebug(line.c_str());
			std::stringstream ss(line);
			while (std::getline(ss, token, ',')) {
				line_tokens.push_back(token);
			}
			if (line_tokens.size() != 3) {
				continue;
			}
			int stage = stoi(line_tokens[0]);
			int setnum = stoi(line_tokens[1]);
			double igt = stof(line_tokens[2]);
			timeEntryDatabase.push_back({ stage, setnum, igt });
		}
	}
	csvdb_f.close();
	if (sfxname == "emblem") {
		SE_ID = 0x8006;
		SFX_ON = true;
	}
	else if (sfxname == "pipe") {
		sfx_filename = "pipe.adx";
		SFX_ON = true;
	}
	else if (!sfxname.empty()) {
		sfx_filename = sfxname.c_str();
		PrintDebug("SFX Path: %s", sfx_filename);
		SFX_ON = true;
	}
}

void saveTimeToDB(int stage, int setnum, double igt)
{
	//check if pb
	std::vector<TimeEntry> currentSetEntries;
	std::copy_if(timeEntryDatabase.begin(), timeEntryDatabase.end(), std::back_inserter(currentSetEntries), [setnum, stage](TimeEntry e) { return e.setnum == setnum && e.stage == stage; });
	bool is_pb = false;
	if (!currentSetEntries.empty()) {
		for (TimeEntry entry : currentSetEntries) {
			if (entry.igt > igt) {
				is_pb = true;
				break;
			}
		}
	}
	else {
		is_pb = true;
	}
	if (is_pb) {
		if (SFX_ON) {
			PrintDebug("SFX ON");
			PlayMusicOnce(0, sfx_filename);
			PrintDebug("OK DONE");
			if (SE_ID != -1) {
				PlaySoundProbably(SE_ID, 0, 0, 0x40);
			}
			
		}
		std::stringstream msg(std::stringstream::in | std::stringstream::out);
		int minutes = (igt / 60);
		double seconds = igt - (minutes * 60);
		
		msg << "New PB of ";
		if (minutes > 0) {
			msg << minutes << ":";
		}
		msg << std::setprecision(4) << seconds << " for set " << setnum;
		SendTimedDebugMessage(msg.str(), 180);

	}
	timeEntryDatabase.push_back({ stage, setnum, igt });
	
	std::ofstream csvdb(db_path, std::ios::app);
	csvdb << stage << "," << setnum << "," << igt << std::endl;
	csvdb.close();
}
