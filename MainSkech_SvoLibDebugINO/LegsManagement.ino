





/*\\\
 0,(0 + 0)=0,180a
, 0,(0 + 60)=60,60a
, 0,(28 + 9)=37,162a
, 0,(23 + 18)=41,144a
, 0,(0 + 29)=29,122a
, 0,(-21 + 21)=0,139a
*/

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

int cntCap = 0;
void moveandpause() {
	//delay(DelayGlobal);
	cntCap++;
	if (cntCap > 100) {
		Serial.println("Terminated");
		return;
		}
	Serial.print(cntCap); Serial.println(" *********************");
	FL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	FR.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	BL.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	BR.MoveToBySpeed(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2], positionsTest[i_a][3], togg);
	IncremantIndex(i_a, i_b, i_c, i_d);
	}



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


//void ONDoOnceEvery(int argInterval, unsigned long&argPrevStamp, unsigned long argCurtime, void(*FooFunc) ) {
void ONDoOnceEvery(int argInterval, unsigned long& argPrevStamp, unsigned long argCurtime, GenericFP_V& argVoidfunc) {

	if (argCurtime - argPrevStamp > argInterval) {
		argPrevStamp = argCurtime;
		//do once
		//Serial.println("wtf");
		argVoidfunc();
		}
	}
