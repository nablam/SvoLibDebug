





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





//void ONDoOnceEvery(int argInterval, unsigned long&argPrevStamp, unsigned long argCurtime, void(*FooFunc) ) {
void ONDoOnceEvery(int argInterval, unsigned long& argPrevStamp, unsigned long argCurtime, GenericFP_V& argVoidfunc) {

	if (argCurtime - argPrevStamp > argInterval) {
		argPrevStamp = argCurtime;
		//do once
		//Serial.println("wtf");
		argVoidfunc();
		}
	}


int manualD;
int manualDinv;
int manualT;
int manualTinv;
int manualH;
int manualHInv;

void CalculateForSlide() {
	  manualD= map(_masterjds.LS_dU, 0, PotReadScale, -11, 11);
	  manualDinv = map(_masterjds.LS_dU, 0, PotReadScale, 11, -11);
	  manualT= map(_masterjds.LS_lR, 0, PotReadScale, -11, 11);
	  manualTinv = map(_masterjds.LS_lR, 0, PotReadScale, 11, -11);
	  manualH = map(_masterjds.RS_uD, 0, PotReadScale, 13, 23);
	    
	    manualHInv = map(_masterjds.RS_uD, 0, PotReadScale, 23, 13 );

		//String outstr =
		//	"D" + String(manualD) + " D^ " + String(manualDinv) + "\n" +
		//	"T" + String(manualT) + " T^ " + String(manualTinv) + "\n" +
		//	"H " + String(manualH) + " H^ " + String(manualHInv) + "\n";
		//Serial.print(outstr);
	}
void CalculateforPivrot() {}
void SlideLAt() {
	CalculateForSlide();
	FL.MoveToBySpeed(manualT, manualD, manualH, SpeedGlobal, togg);
	FR.MoveToBySpeed(manualT, manualD, manualH, SpeedGlobal, togg);
	BL.MoveToBySpeed(manualT, manualD, manualH, SpeedGlobal, togg);
	BR.MoveToBySpeed(manualT, manualD, manualH, SpeedGlobal, togg);
	}

void SlideFor() {
	
	}

void Tilt() {}
void Pivot() {}
