#include "pch.h"
#include "emeraldset.h"
#include "asmutils.h"
#include <random>
#include <iterator>
#include <vector>
#include <fstream>
#include <sstream>

int hookEntryPt = 0x7380BF;
DWORD hookExit = hookEntryPt + 6;
std::vector<int> setIDs;
DWORD current_id;

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
	if (setIDs.size() > 0) {
		current_id = *choose_random(setIDs.begin(), setIDs.end());
	}
	__asm {
		popad
		mov edi, current_id
		jmp hookExit
	}
}

void initSetHook() {
	if (setIDs.size() > 0) {
		Hook((void*)hookEntryPt, hook1024, 6);
	}
}

void LoadSetsFromFile(std::string fpath) {
	std::ifstream infile(fpath);
	int id;
	PrintDebug("Sets Loaded:");
	while (infile >> id) {
		setIDs.push_back(id);
		PrintDebug("%d", id);
	}
	infile.close();
}