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
const int totalPositions = 6;
bool togg = true;
const int LoopLen = 12; //ms
typedef void (*GenericFP_VI)(int); //function pointer prototype to a function which takes an 'int' an returns 'void'
typedef void (*GenericFP_V)();  
void afoo() { Serial.println("yooo"); }
GenericFP_V afooptr; 

  void (*F_kine)();
void (*F_ReadPots)();
SvoV2 s0,s1,s2,s3,s4,s5,s6,s7, s8, s9, s10, s11;
SvoV2 SVOarra[12] = { s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11 };
SvoV2* sarraptr = SVOarra;
LegMatik FL(sarraptr,0,1,2,'a');
LegMatik FR(sarraptr, 3, 4, 5,'b');
LegMatik BL(sarraptr, 6, 7, 8,'c');
LegMatik BR(sarraptr, 9, 10, 11,'d');

int positions[totalPositions][4] = {
	{0,0,12,SpeedGlobal },
	{0,8,19,SpeedGlobalSlow},
	{0,4,19,SpeedGlobalSlow},
	{0,-4,19,SpeedGlobalSlow},
	{0,-4,19,SpeedGlobalSlow},
	{0,-8,19,SpeedGlobalSlow}
	};
 
int positionsTest[totalPositions][4] = {
	{0,0,24,SpeedGlobal },
	{0,0,12,SpeedGlobalSlow},
	{0,11,21,SpeedGlobalSlow},
	{0,9,21,SpeedGlobalSlow},
	{0,0,21,SpeedGlobalSlow},
	{0,-8,21,SpeedGlobalSlow}
	};

/*\\\
 0,(0 + 0)=0,180a
, 0,(0 + 60)=60,60a
, 0,(28 + 9)=37,162a
, 0,(23 + 18)=41,144a
, 0,(0 + 29)=29,122a
, 0,(-21 + 21)=0,139a
*/
int i_a = 0;
int i_b = 1;
int i_c = 2;
int i_d = 3;
unsigned long startMicros;// = micros();
unsigned long currentTime = millis();
unsigned long endMicros;  
unsigned long _0_10_msCounterStampCashed=0;
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
	afooptr= afoo;
}

//void ONDoOnceEvery(int argInterval, unsigned long&argPrevStamp, unsigned long argCurtime, void(*FooFunc) ) {
	void ONDoOnceEvery(int argInterval, unsigned long& argPrevStamp, unsigned long argCurtime, GenericFP_V &argVoidfunc) {
	
	if (argCurtime - argPrevStamp > argInterval) {
		argPrevStamp = argCurtime;
//do once

		//Serial.println("wtf");
		
		argVoidfunc();
			 
			
		}
	
	}


void loop() {
	  currentTime = millis();
	  ONDoOnceEvery(1000, _0_10_msCounterStampCashed, currentTime, afooptr);
	  /*if (currentTime - previousTimeSerialPrintPotentiometer > timeIntervalSerialPrint) {
		  previousTimeSerialPrintPotentiometer = currentTime;
		  Serial.println(savedval);*/
  //do once
		 // }


	//ReadPotpins();
	//for (int x = 0; x < 20; x++) {
	//	 
	//	  
	//	 
	//	 
	//	moveandpause();
	//	}
	////Serial.println(pval10_LS_lR);
	//loopTask();
}

void loopCaseSwitch() {}

void moveandpause( ) {
	//delay(DelayGlobal);
	FL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	FR.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	BL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	BR.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	IncremantIndex(i_a, i_b, i_c, i_d);
	}
void IncremantIndex(int& argindex, int& brgindex, int& crgindex, int& drgindex) {
	argindex++;
	if (argindex >= totalPositions)argindex = 0;
	brgindex++;
	if (brgindex >= totalPositions)brgindex = 0;
	crgindex++;
	if (crgindex >= totalPositions)crgindex = 0;
	drgindex++;
	if (drgindex >= totalPositions)drgindex = 0;

	}

 
unsigned long previousTimeStamp = millis();
long timeIntervalLed1 = 1000;
unsigned long previousTimeSerialPrintPotentiometer = millis();
long timeIntervalSerialPrint = 2000;
int savedval = 0;
void loopTask() {
  // put your main code here, to run repeatedly:
	unsigned long currentTime = millis();

	// task 1
	if (currentTime - previousTimeStamp > timeIntervalLed1) {
		previousTimeStamp = currentTime;
// Do once foo
		FL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
		}

		// task 2
	if (Serial.available()) {
		int userInput = Serial.parseInt();
		if (userInput >= 0 && userInput < 256) {
			savedval = userInput;
			}
		}

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
	if (currentTime - previousTimeSerialPrintPotentiometer > timeIntervalSerialPrint) {
		previousTimeSerialPrintPotentiometer = currentTime;
		Serial.println(savedval);
//do once
		}
	}



//old
//SvoV2& curSvo = SVOarra[3];
//curSvo.Speedmove(-30, 10);
		 