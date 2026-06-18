#pragma once
#include <vector>
#include <string>

struct TimeEntry {
	int stage;
	int setnum;
	double igt;
};

std::ofstream csvdb_f;
void loadEntryDB(std::string fpath, std::string sfxname);
void saveTimeToDB(int stage, int setnum, double igt);