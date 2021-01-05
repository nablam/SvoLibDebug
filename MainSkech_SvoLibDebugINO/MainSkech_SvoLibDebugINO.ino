/*
 Name:		MainSkech_SvoLibDebugINO.ino
 Created:	1/4/2021 11:48:30 PM
 Author:	lamri
*/

// the setup function runs once when you press reset or power the board
#include "SvoV2.h"

SvoV2 s0(false);
SvoV2 s1(false);
SvoV2 s2(false);
SvoV2 s3(false);
SvoV2 s4(false);
SvoV2 s5(false);
SvoV2 s6(false);
SvoV2 s7(false);
SvoV2 s8(false);
SvoV2 s9(false);
SvoV2 s10(false);
SvoV2 s11(false);
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
void setup() {
	Serial.begin(115200);
	
	for (int x = 0; x < 12; x++) {
		 
		SVOarra[x].AttachSelf();
		SVOarra[x].ZeroMe();
		SVOarra[x].PrintMe();
		}


}
bool togg = false;
void loop() {

	delay(2000);

	togg = !togg;

	SvoV2& curSvo = SVOarra[2];

	if(togg)
	curSvo.Speedmove(200, 20);
	else
		curSvo.Speedmove(110, 50);
}
