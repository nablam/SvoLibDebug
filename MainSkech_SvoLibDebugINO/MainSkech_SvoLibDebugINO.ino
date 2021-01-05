/*
 Name:		MainSkech_SvoLibDebugINO.ino
 Created:	1/4/2021 11:48:30 PM
 Author:	lamri
*/

#include "SvoV2.h"
#include "LegMatik.h"
const int SpeedGlobal = 35;
const int DelayGlobal = 4000;
bool togg = false;
SvoV2 s0,s1,s2,s3,s4,s5,s6,s7, s8, s9, s10, s11;
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
SvoV2* sarraptr = SVOarra;
LegMatik FL(sarraptr,0,1,2);
LegMatik FR(sarraptr, 3, 4, 5);
LegMatik BL(sarraptr, 6, 7, 8);
LegMatik BR(sarraptr, 9, 10, 11);
void setup() {
	Serial.begin(115200);
	for (int x = 0; x < 12; x++) {
		 if(!SVOarra[x].Attached())
		SVOarra[x].AttachSelf();
		SVOarra[x].SitMe();
		}
}

void loop() {

	GoToLow();
	GoToHigh();
	GoToForward();
	GoToBack();

}

void GoToLow() {
	FL.MoveToBySpeed(0, 0, 13, SpeedGlobal);
	FR.MoveToBySpeed(0, 0, 13, SpeedGlobal);
	BL.MoveToBySpeed(0, 0, 13, SpeedGlobal);
	BR.MoveToBySpeed(0, 0, 13, SpeedGlobal);
	delay(DelayGlobal);
	}
void GoToHigh() {
	FL.MoveToBySpeed(0, 0, 23, SpeedGlobal);
	FR.MoveToBySpeed(0, 0, 23, SpeedGlobal);
	BL.MoveToBySpeed(0, 0, 23, SpeedGlobal);
	BR.MoveToBySpeed(0, 0, 23, SpeedGlobal);
	delay(DelayGlobal);
	}
void GoToForward() {
	FL.MoveToBySpeed(0, -10, 15, SpeedGlobal);
	FR.MoveToBySpeed(0, -10, 15, SpeedGlobal);
	BL.MoveToBySpeed(0, 10, 15, SpeedGlobal);
	BR.MoveToBySpeed(0, 10, 15, SpeedGlobal);
	delay(DelayGlobal);
	}
void GoToBack() {
	FL.MoveToBySpeed(0, 8, 15, SpeedGlobal);
	FR.MoveToBySpeed(0, 8, 15, SpeedGlobal);
	BL.MoveToBySpeed(0, -8, 15, SpeedGlobal);
	BR.MoveToBySpeed(0, -8, 15, SpeedGlobal);
	delay(DelayGlobal);
	}




//old
//SvoV2& curSvo = SVOarra[3];
//curSvo.Speedmove(-30, 10);
		 