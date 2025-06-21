#include "pch.h"
#include "pieceutils.h"
#include "emeraldset.h"
#include "asmutils.h"

int hookEntryPtPostDeath = 0x7380BF;
int hookJmpBackP3 = 0x73850E;
int hookJmpBackPostP3 = 0x7387BD;

int *deathChamberAllPingIds = new int[9]{ 0, 0x100, 2, 0x102, 0x501, 0x205, 0x305, 0x405, 0x505 };
int *deathChamberYRPingIds = new int[3] {2, 0x102, 0x305};
int *deathChamberYellowPingIds = new int[5] {0, 0x100, 0x501, 0x205, 0x405};
int *eggQuartersYellowPingIds = new int[7]{ 2, 0x302, 5, 0x205, 0x405, 0x605, 0x705};
int *eggQuartersGreenPingIds = new int[4]{ 0, 0x100, 0x102, 0x305 };
int *eggQuartersAllPingIds = new int[7 + 4 + 1];
int* wcFreeP2s = new int[2] {0x305, 0xC05}; //bird box middle, on a railway, always included
int* wcLevel1FreePieces = new int[4] { 0x607, 0x707, 0x608, 0x708 };// altar with six pillars, between two stone statues, storm zone (lower), beginning room
int* wcLevel2FreePieces = new int[4] {0xC04, 8, 0x804, 0x904}; //tall pillar, mountain (containers), four whirling eyes (both)
int* wcLevel3FreePieces = new int[3] {0x504, 0x108, 0x604}; //mountain (chest), lap, pillar no treasure
int* wcLevel4FreePieces = new int[2] {0x208, 0x408}; //storm zone upper, terrace with two legs
int* N_LEVEL_PIECES = new int[4] {4, 4 + 4, 4 + 4 + 3, 4 + 4 + 3 + 2};

const int DC_PIECE_ID = 0x301; //Water Bug
const int EQ_PIECE_ID = 0x503; //Holy Altar
const int WC_PIECE_ID = 0x401; //Suspended Ceiling Exposed

int group_arr_len = 0;
int* group_arr;

void setPieceByID(int id) {
	//PrintDebug("Setting Piece ID %X", id & 0xFFFF);
	byte major_id = id & 0xFF;
	Emerald curr_piece;
	Emerald* slot_arr = (Emerald*)0;
	int slot_arr_len = 0;
	switch (major_id) {
	case 0:
	case 2:
	case 5:
		slot_arr = EmeraldManager->slot_2_emeralds;
		slot_arr_len = EmeraldManager->slot_2_array_len;
		break;
	case 1:
	case 3:
		slot_arr = EmeraldManager->slot_1_emeralds;
		slot_arr_len = EmeraldManager->slot_1_array_len;
		break;
	case 4:
	case 7:	
	case 8:
		slot_arr = EmeraldManager->slot_3_emeralds;
		slot_arr_len = EmeraldManager->slot_3_array_len;
		break;
	case 0xA:
		slot_arr = EmeraldManager->slot_enemy_emeralds;
		slot_arr_len = EmeraldManager->slot_enemy_array_len;
		break;
	}
	if (slot_arr_len == 0) {
		PrintDebug("PIECE %X HAS INVALID ID", id);
		return;
	}
	for (int i = 0; i < slot_arr_len; i++) {
		curr_piece = slot_arr[i];
		if (curr_piece.id == id) {
			if (slot_arr == EmeraldManager->slot_2_emeralds) {
				EmeraldManager->piece_2.id = curr_piece.id;
				EmeraldManager->piece_2.position.x = curr_piece.position.x;
				EmeraldManager->piece_2.position.y = curr_piece.position.y;
				EmeraldManager->piece_2.position.z = curr_piece.position.z;
				//PrintDebug("P2 STORED ID: %X",EmeraldManager->piece_2.id);
				return;
			}
			else if (slot_arr == EmeraldManager->slot_3_emeralds) {
				EmeraldManager->piece_3.id = curr_piece.id;
				EmeraldManager->piece_3.position.x = curr_piece.position.x;
				EmeraldManager->piece_3.position.y = curr_piece.position.y;
				EmeraldManager->piece_3.position.z = curr_piece.position.z;
			}
			else if (slot_arr == EmeraldManager->slot_1_emeralds || slot_arr == EmeraldManager->slot_enemy_emeralds) {
				EmeraldManager->piece_1.id = curr_piece.id;
				EmeraldManager->piece_1.position.x = curr_piece.position.x;
				EmeraldManager->piece_1.position.y = curr_piece.position.y;
				EmeraldManager->piece_1.position.z = curr_piece.position.z;
			}
		}
	}
	if (slot_arr_len > 0) {
		PrintDebug("PIECE %X NOT FOUND", id);
	}
	

}

void __declspec(naked)hookCustomSetGen() {
	__asm {
		pushad
	}
	 if (CurrentLevel == LevelIDs_DeathChamber || CurrentLevel == LevelIDs_EggQuarters) {
	
		
		
		if (CurrentLevel == LevelIDs_DeathChamber) {
			setPieceByID(DC_PIECE_ID); //Water Bug
			EmeraldManager->piece_1.id = -2; //pre-collect piece	
			

			//give a guaranteed yellow ping on respawn
			int rand_idx = (rand() % group_arr_len); //fuck it we ball fuck it we ball fuck it we ball
			setPieceByID(group_arr[rand_idx]);
			
		}
		else if (CurrentLevel == LevelIDs_EggQuarters) {
			setPieceByID(EQ_PIECE_ID); //Holy Altar
			EmeraldManager->piece_1.id = -2; //pre-collect
			int* pingread_arr = eggQuartersAllPingIds;
			int pingread_arr_sz = 12;
			int rand_idx = (rand() % pingread_arr_sz);
			setPieceByID(pingread_arr[rand_idx]);
			

		}
		EmeraldManager->emeralds_spawned = 1;

		__asm {
			popad
			mov esi,[ebp+8]
			mov ecx,esi
			jmp hookJmpBackP3
		}

	}
	else {
		__asm {
			popad
			jmp (hookEntryPtPostDeath + 6)
		}

	}
}

void initPostDeathHook(std::string piecegroup)
{
	
	std::copy(eggQuartersYellowPingIds, eggQuartersYellowPingIds + 7, eggQuartersAllPingIds);
	std::copy(eggQuartersGreenPingIds, eggQuartersGreenPingIds + 4, eggQuartersAllPingIds + 7);
	eggQuartersAllPingIds[11] = 0x105; //First Place You Landed

		if (piecegroup == "AllP2") {
			group_arr = deathChamberAllPingIds;
			group_arr_len = 9;
		}
		else if (piecegroup == "RedYellowP2") {
			group_arr = deathChamberYRPingIds;
			group_arr_len = 3;
		}
		else if (piecegroup == "YellowP2") {
			group_arr = deathChamberYellowPingIds;
			group_arr_len = 5;
		}

	



	Hook((void*)hookEntryPtPostDeath, hookCustomSetGen, 6);
	srand(time(NULL));
}
