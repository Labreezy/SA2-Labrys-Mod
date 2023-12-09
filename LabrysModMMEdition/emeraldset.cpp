#include "pch.h"
#include "emeraldset.h"
#include "asmutils.h"
#include <random>
#include <iterator>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

int hookEntryPt = 0x7380BF;
DWORD hookExit = hookEntryPt + 6;
std::vector<int> setIDsGeneric;
std::map<int, std::vector<int>*> stgSets({
	{LevelIDs_AquaticMine, new std::vector<int>()},
	{LevelIDs_DeathChamber, new std::vector<int>()},
	{LevelIDs_DryLagoon, new std::vector<int>()},
	{LevelIDs_EggQuarters, new std::vector<int>()},
	{LevelIDs_MadSpace, new std::vector<int>()},
	{LevelIDs_MeteorHerd, new std::vector<int>()},
	{LevelIDs_SecurityHall, new std::vector<int>()},
	{LevelIDs_WildCanyon, new std::vector<int>()}
});
DWORD current_id;
int lastStage = -1;


template<typename Iter, typename RandomGenerator>
Iter choose_random(Iter start, Iter end, RandomGenerator &g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter choose_random(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return choose_random(start, end, gen);
}


void __declspec(naked)hook1024() {
	__asm {
		pushad
	}
	if (setIDsGeneric.size() > 0 && NumDeathsRestarts == 0) {
		current_id = *choose_random(setIDsGeneric.begin(), setIDsGeneric.end());
		PrintDebug("Loading Set %d", current_id);
	}
	else {
		PrintDebug("No set(s) loaded");
	}
	__asm {
		popad
		mov edi, current_id
		jmp hookExit
	}
}

void initSetHook() {
	if (setIDsGeneric.size() > 0) {
		Hook((void*)hookEntryPt, hook1024, 6);
	}
}

void LoadSetsFromFile(std::string fpath) {
	std::ifstream infile(fpath);
	int id;
	if (!setIDsGeneric.empty()) {
		PrintDebug("got rid of %d sets", setIDsGeneric.size());
		setIDsGeneric.clear();
	}
	PrintDebug("Sets Loaded from %s", fpath.c_str());
	while (infile >> id) {
		setIDsGeneric.push_back(id);
		PrintDebug("%d", id);
	}
	infile.close();
}