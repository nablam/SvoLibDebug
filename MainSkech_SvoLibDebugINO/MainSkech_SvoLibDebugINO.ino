/*
 Name:		MainSkech_SvoLibDebugINO.ino
 Created:	1/4/2021 11:48:30 PM
 Author:	lamri
*/

// the setup function runs once when you press reset or power the board
#include "SvoV2.h"
SvoV2 s0,s1,s2,s3,s4,s5,s6,s7, s8, s9, s10, s11;
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
void setup() {
	Serial.begin(115200);
	for (int x = 0; x < 12; x++) {
		 if(!SVOarra[x].Attached())
		SVOarra[x].AttachSelf();
		SVOarra[x].SitMe();
		//SVOarra[x].PrintMe();
		}
}
bool togg = false;
void loop() {

	 delay(2000);
	 for (int x = 0; x < 12; x++) {
		 SVOarra[x].ZeroMe();
		 
		 }
	//togg = !togg;

	////SvoV2& curSvo = SVOarra[3];

	//if (togg)
	////curSvo.Speedmove(-30, 10);
	//	Move1(-30,10 );
	//else
	//	Move2(30,50);
	 
}
void Move1(int pos, uint8_t speed  ) {
	for (int x = 0; x < 12; x++) {
		if(x%3==1)
			SVOarra[x].Speedmove(pos, speed);
		}
	 
	}
void Move2(int pos, uint8_t speed  ) {
	for (int x = 0; x < 12; x++) {
		if (x % 3 == 1)
			SVOarra[x].Speedmove(pos, speed);
		}
 
	}
