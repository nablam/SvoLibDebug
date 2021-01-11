/*
 Name:		MainSkech_SvoLibDebugINO.ino
 Created:	1/4/2021 11:48:30 PM
 Author:	lamri
*/

#include "SvoV2.h"
#include "LegMatik.h"
 
#pragma region declarations
const int SpeedGlobal = 150;
const int DelayGlobal = 4000;
const int SpeedGlobalSlow = 30;

bool togg = false;
const int LoopLen = 12; //ms



//-----------------------------------------------------------------------------positions array indecies
const int totalPositions = 5;
int i_a_last = -1;
int i_a = 0;
int i_b = 1;
int i_c = 2;
int i_d = 3;
int positions[totalPositions][4] = {
	{0,0,12,SpeedGlobal },
	{0,8,19,SpeedGlobal},
	{0,4,19,SpeedGlobal},
	{0,-4,19,SpeedGlobal},
	{0,-4,19,SpeedGlobal},
	//{0,-8,19,SpeedGlobalSlow}
	};

int positionsTest[totalPositions][4] = {
	//	{0,11,21,SpeedGlobal },
	//{0,5,21,SpeedGlobalSlow},
	//{0,0,21,SpeedGlobalSlow},
	//{0,-4,21,SpeedGlobalSlow},
	//{0,-8,21,SpeedGlobalSlow},
	////{0,-8,21,SpeedGlobalSlow}
	{0,0,21,SpeedGlobal },
	{0,0,20,SpeedGlobal},
	{0,-4,9,SpeedGlobal},
	{0,-4,9,SpeedGlobal},
	{0,0,21,SpeedGlobal},
	//{0,-8,21,SpeedGlobalSlow}
	};

int AnglesTest[totalPositions][4] = {
	 

	{0,90,90,SpeedGlobal },
	{0,180,180,SpeedGlobal},
	{0,90,90,SpeedGlobal},
	{0,270,270,SpeedGlobal},
	{0,90,90,SpeedGlobal},
	/*{0,90,90,SpeedGlobal },
	{0,270,180,SpeedGlobal},
	{0,90,90,SpeedGlobal},
	{0,270,120,SpeedGlobal},
	{0,90,90,SpeedGlobal},*/

//		{0,90,90,SpeedGlobal },
// {0,90,90,SpeedGlobal },
//{0,90,90,SpeedGlobal },
//{0,90,90,SpeedGlobal },
//{0,90,90,SpeedGlobal },

 
	};
//----------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------counters
unsigned long startMicros;// = micros();
unsigned long currentTime = millis();
unsigned long currentTime_0_10 =-1;
unsigned long endMicros;  
unsigned long _0_10_msCounterStampCashed=0;
//
 
//------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------functionpointers
typedef void (*GenericFP_VI)(int);  
typedef void (*GenericFP_V)();
void afoo() { Serial.println("yooo"); }
GenericFP_V afooptr;
void (*F_kine)();
void (*F_ReadPots)();
//---------------------------------------------------------------------------------------------

int savedval = 0;

#pragma endregion

#pragma region staticobjectsInit
SvoV2 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
SvoV2* sarraptr = SVOarra;
LegMatik FL(sarraptr, 0, 1, 2, 'a');
LegMatik FR(sarraptr, 3, 4, 5, 'b');
LegMatik BL(sarraptr, 6, 7, 8, 'c');
LegMatik BR(sarraptr, 9, 10, 11, 'd');
#pragma endregion

void setup() {
	Serial.begin(115200);
	for (int x = 0; x < 12; x++) {
		 if(!SVOarra[x].Attached())
		SVOarra[x].AttachSelf();
		SVOarra[x].ZeroMe();
		}
	/*startMicros = micros();
	for (int n = 0; n < 1; n++) {
		moveandpause();
		}
	endMicros = micros();

	Serial.println(endMicros - startMicros);*/
	//afooptr= moveandpause_LegsMNGino;
	afooptr = DoAngleTest;
}

/*
1 *********************
1 id=1 last= 396 new=219
2 *********************
2 id=1 last= 219 new=187
3 *********************
3 id=1 last= 187 new=171
4 *********************
4 id=1 last= 171 new=162
5 *********************
5 id=1 last= 162 new=158
*/
void loop() {
	  currentTime = millis();
	  
	  /*if (currentTime - previousTimeSerialPrintPotentiometer > timeIntervalSerialPrint) {
		  previousTimeSerialPrintPotentiometer = currentTime;
		  Serial.println(savedval);*/
  //do once
		 // }

	//for (int x = 0; x < 6; x++) {
		 
	ONDoOnceEvery(5000, _0_10_msCounterStampCashed, currentTime, afooptr);
		 
		 
	//	moveandpause();
	//	}
	////Serial.println(pval10_LS_lR);
	// loopTask();
}
void loopTask() {
  // put your main code here, to run repeatedly:
	//unsigned long currentTime = millis();

	// cOUNT 10 mILLIS
	if (currentTime - _0_10_msCounterStampCashed >= 1) {
		_0_10_msCounterStampCashed = currentTime;
  
		switch (_0_10_msCounterStampCashed%10)
			{
			//--------------------------READ
				case 0:
					ReadPotpins();
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;

					//--------------------------calc
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
					//--------------------------write
				case 8:
					DecideTiltPivot();
					break;
				case 9:
					break;
					

				default:
					break;
			}

		if (_0_10_msCounterStampCashed % 100 == 0) {
			
			}
		}

		// task 2
	//if (Serial.available()) {
	//	int userInput = Serial.parseInt();
	//	if (userInput >= 0 && userInput < 256) {
	//		savedval = userInput;
	//		}
	//	}

		// task 3
	//if (digitalRead(BUTTON_PIN) == HIGH) {
	//	digitalWrite(LED_3_PIN, HIGH);
	//	}
	//else {
	//	digitalWrite(LED_3_PIN, LOW);
	//	}

		// task 4
	/*int potentiometerValue = analogRead(POTENTIOMETER_PIN);
	if (potentiometerValue > 512) {
		digitalWrite(LED_4_PIN, HIGH);
		}
	else {
		digitalWrite(LED_4_PIN, LOW);
		}*/

		// task 5
	//if (currentTime - previousTimeSerialPrintPotentiometer > timeIntervalSerialPrint) {
	//	previousTimeSerialPrintPotentiometer = currentTime;
	//	Serial.println(savedval);
//do once
		//}
	}




//old
//SvoV2& curSvo = SVOarra[3];
//curSvo.Speedmove(-30, 10);
		 