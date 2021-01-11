





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

	i_a_last = argindex - 1;
	if (i_a_last < 0)i_a_last = (totalPositions - 1);
	}

int cntCap = 0;
void moveandpause_LegsMNGino() {
	//delay(DelayGlobal);
	cntCap++;
	if (cntCap > 100) {
		Serial.println("Terminated");
		return;
		}
	Serial.print(cntCap); Serial.println(" *********************");
	 
	FL.XYZ_inputConversion(positionsTest[i_a][0], positionsTest[i_a][1], positionsTest[i_a][2]);
	FR.XYZ_inputConversion(positionsTest[i_b][0], positionsTest[i_b][1], positionsTest[i_b][2]);
	BL.XYZ_inputConversion(positionsTest[i_c][0], positionsTest[i_c][1], positionsTest[i_c][2]);
	BR.XYZ_inputConversion(positionsTest[i_d][0], positionsTest[i_d][1], positionsTest[i_d][2]);


	IncremantIndex(i_a, i_b, i_c, i_d);

	FL.CalcOptimalSpeed_injectCoeffs();
	FR.CalcOptimalSpeed_injectCoeffs();
	BL.CalcOptimalSpeed_injectCoeffs();
	BR.CalcOptimalSpeed_injectCoeffs();

	FL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	FR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	}

void DoAngleTest() {

	cntCap++;
	if (cntCap > 100) {
		Serial.println("Terminated");
		return;
		}
	Serial.print(cntCap); Serial.println(" *********************");
	
	float speed1coef, speed2coeff=1.0;

	int diff1, diff2;

	diff1 = abs(AnglesTest[i_a][1] - AnglesTest[i_a_last][1]);
	diff2 = abs(AnglesTest[i_a][2] - AnglesTest[i_a_last][2]);
	//if 1 has to go farther than 2
	if (diff1 >= diff2) {
		speed1coef = 1.0; 
		speed2coeff = ((float)diff2 / (float)diff1);
		}
	else
		{
		speed2coeff = 1.0;
		speed1coef =  ((float)diff2 / (float)diff2) ;
		}

	int speedOptimized1 = speed1coef * SpeedGlobal;

	int speedOptimized2 = speed2coeff * SpeedGlobal;

	if (!togg) {
		  speedOptimized1 =SpeedGlobalSlow;

		  speedOptimized2 = SpeedGlobal;

		
		}
	Serial.print("s1 A="); Serial.print(AnglesTest[i_a][1]); Serial.print(" sp= "); Serial.print(speedOptimized1);
	Serial.print(" s2 A="); Serial.print(AnglesTest[i_a][2]); Serial.print(" sp= "); Serial.print(speedOptimized2); Serial.println(" ");
	s1.Speedmove(AnglesTest[i_a][1], speedOptimized1);
	s2.Speedmove(AnglesTest[i_a][2], speedOptimized2);
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

void ControlWithPotsForSlide() {
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
void DecideTiltPivot() {
	
	if (_masterjds.RS_rot > 700) { SlideFlat(); }
	else { PivotMid(); }

	}
void SlideFlat() {
	ControlWithPotsForSlide();
	MoveMembersTOPointTo(manualT, manualD, manualH);	 
	}

void MoveMembersTOPointTo(int t, int d, int h) {
	FL.XYZ_inputConversion(t, d, h);
	FR.XYZ_inputConversion(t, d, h);
	BL.XYZ_inputConversion(t, d, h);
	BR.XYZ_inputConversion(t, d, h);
	FL.CalcOptimalSpeed_injectCoeffs();
	FR.CalcOptimalSpeed_injectCoeffs();
	BL.CalcOptimalSpeed_injectCoeffs();
	BR.CalcOptimalSpeed_injectCoeffs();

	FL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	FR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	}

 
void PivotMid() {
	/*ControlWithPotsForSlide();
	FL.XYZ_inputConversion(manualT, manualD, manualHInv);
	FR.XYZ_inputConversion(manualT, manualD, manualHInv);
	BL.XYZ_inputConversion(manualTinv, manualD, manualH);
	BR.XYZ_inputConversion(manualTinv, manualD, manualH);
	FL.CalcOptimalSpeed();
	FR.CalcOptimalSpeed();
	BL.CalcOptimalSpeed();
	BR.CalcOptimalSpeed();

	FL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	FR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BL.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	BR.MoveTo_curReqval_BySpeedoeff(SpeedGlobal, togg);
	*/
	}
