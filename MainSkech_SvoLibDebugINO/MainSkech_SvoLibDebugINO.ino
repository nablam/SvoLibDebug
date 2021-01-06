/*
 Name:		MainSkech_SvoLibDebugINO.ino
 Created:	1/4/2021 11:48:30 PM
 Author:	lamri
*/

#include "SvoV2.h"
#include "LegMatik.h"
const int SpeedGlobal = 36;
const int DelayGlobal = 4000;
const int SpeedGlobalSlow = SpeedGlobal/2;
const int totalPositions = 5;
bool togg = false;
SvoV2 s0,s1,s2,s3,s4,s5,s6,s7, s8, s9, s10, s11;
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
SvoV2* sarraptr = SVOarra;
LegMatik FL(sarraptr,0,1,2,'a');
LegMatik FR(sarraptr, 3, 4, 5,'b');
LegMatik BL(sarraptr, 6, 7, 8,'c');
LegMatik BR(sarraptr, 9, 10, 11,'d');

int positions[totalPositions][4] = {
	{3,0,13,SpeedGlobal },
	{0,8,19,SpeedGlobalSlow},
	{0,4,19,SpeedGlobalSlow},
	{0,-4,19,SpeedGlobalSlow},
	{0,-8,19,SpeedGlobalSlow}
	};
/*,a 13,56,68
,a 0,8,118
,a 0,24,108
,a 0,48,108
,a 0,54,118
,a 13,56,68*/
int positionsTest[totalPositions][4] = {
	{0,0,24,SpeedGlobal },
	{0,0,20,SpeedGlobalSlow},
	{0,4,20,SpeedGlobalSlow},
	{0,8,20,SpeedGlobalSlow},
	{0,12,20,SpeedGlobalSlow}
	};

/*,a 0,0,180
,a 0,34,113
,a 0,20,116
,a 0,4,128
,a 0,17,153*/
int i_a = 0;
int i_b = 1;
int i_c = 2;
int i_d = 3;
void IncremantIndex(int & argindex, int& brgindex, int& crgindex, int& drgindex) {
	argindex++;
	if (argindex >= totalPositions)argindex = 0;
	brgindex++;
	if (brgindex >= totalPositions)brgindex = 0;
	crgindex++;
	if (crgindex >= totalPositions)crgindex = 0;
	drgindex++;
	if (drgindex >= totalPositions)drgindex = 0;

	}
void setup() {
	Serial.begin(115200);
	for (int x = 0; x < 12; x++) {
		 if(!SVOarra[x].Attached())
		SVOarra[x].AttachSelf();
		SVOarra[x].ZeroMe();
		}
}

void loop() {

	for (int x = 0; x < 20; x++) {
		// 
		 // 
		// 
		// 
		moveandpause();
		}
 

}

void moveandpause( ) {
	delay(DelayGlobal);
	FL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	//FR.MoveToBySpeed(positions[i_b][0], positions[i_b][1], positions[i_b][2], positions[i_b][3], togg);
	//BL.MoveToBySpeed(positions[i_c][0], positions[i_c][1], positions[i_c][2], positions[i_c][3], togg);
	//BR.MoveToBySpeed(positions[i_d][0], positions[i_d][1], positions[i_d][2], positions[i_d][3], togg);

	//FL.MoveToBySpeed(t, d, h , SpeedGlobal, togg);
	//FR.MoveToBySpeed(t, d, h , SpeedGlobal, togg);
	//BL.MoveToBySpeed(t, d, h , SpeedGlobal, togg);
	//BR.MoveToBySpeed(t, d, h , SpeedGlobal, togg);
	
	IncremantIndex(i_a, i_b, i_c, i_d);
	}

void GoToLow() {
	FL.MoveToBySpeed(0, 0, 13, SpeedGlobal,togg);
	FR.MoveToBySpeed(0, 0, 13, SpeedGlobal,togg);
	BL.MoveToBySpeed(0, 0, 13, SpeedGlobal,togg);
	BR.MoveToBySpeed(0, 0, 13, SpeedGlobal,togg);
	delay(DelayGlobal);
	}
void GoToHigh() {
	FL.MoveToBySpeed(0, 0, 23, SpeedGlobal,togg);
	FR.MoveToBySpeed(0, 0, 23, SpeedGlobal,togg);
	BL.MoveToBySpeed(0, 0, 23, SpeedGlobal,togg);
	BR.MoveToBySpeed(0, 0, 23, SpeedGlobal,togg);
	delay(DelayGlobal);
	}
void GoToForward() {
	FL.MoveToBySpeed(0, -10, 15, SpeedGlobal,togg);
	FR.MoveToBySpeed(0, -10, 15, SpeedGlobal,togg);
	BL.MoveToBySpeed(0, 10, 15, SpeedGlobal,togg);
	BR.MoveToBySpeed(0, 10, 15, SpeedGlobal,togg);
	delay(DelayGlobal);
	}
void GoToBack() {
	FL.MoveToBySpeed(0, 8, 15, SpeedGlobal,togg);
	FR.MoveToBySpeed(0, 8, 15, SpeedGlobal,togg);
	BL.MoveToBySpeed(0, -8, 15, SpeedGlobal,togg);
	BR.MoveToBySpeed(0, -8, 15, SpeedGlobal,togg);
	delay(DelayGlobal);
	}




//old
//SvoV2& curSvo = SVOarra[3];
//curSvo.Speedmove(-30, 10);
		 