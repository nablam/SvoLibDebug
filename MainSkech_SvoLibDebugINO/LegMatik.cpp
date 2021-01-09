// 
// 
// 

#include "LegMatik.h"
  #define LOGDEBUG

#pragma region PUBS
//LegMatik::LegMatik() {}
LegMatik::LegMatik(SvoV2* argArra, uint8_t argShoulderIndex, uint8_t argArmIndex, uint8_t argCalfIndex,char argcharId) {
	_myShoulder= &argArra[argShoulderIndex];
	_myArm= &argArra[argArmIndex];;
	_myCalf=& argArra[argCalfIndex];;
	_charId = argcharId;
	_myMembers[0] = _myShoulder;
	_myMembers[1] = _myArm;
	_myMembers[2] = _myCalf;
	}
//void LegMatik::TestMyServos() {
//	uint8_t a, b, c = 0;
//	if (_myShoulder) {
//		_myShoulder->SitMe();
//		_myShoulder->PrintMe();
//		a = 1;
//		}
//	if (_myArm) {
//		_myArm->SitMe();
//		_myArm->PrintMe();
//		b = 1;
//		}
//	if (_myCalf) {
//		_myCalf->SitMe();
//		_myCalf->PrintMe();
//		c = 1;
//		}
//	if ((a + b + c) != 3) {
//	#ifdef  LEGDEBUG
//		Serial.println("nullptr?!");
//	#endif //  LEGDEBUG
//		}
//	}

//void LegMatik::PrintAngles(int argT, int argD, int argH) {
//	Serial.println(XYZ_inputConversion(argT, argD, argH, false, 0));
//	}
void LegMatik::MoveTo_curReqval_BySpeedoeff(uint8_t argInputSpeed, bool argdomove) {
	/*_myShoulder ->Speedmove();
	_myArm = &argArra[argArmIndex];;
	_myCalf =*/
#ifdef LOGDEBUG
	if (this->_charId == 'a') {
		Serial.println("**** MoveTo_curReqval_BySpeedoeff *****");
		Serial.print(" speed global"); Serial.println(argInputSpeed);
		 
		}
#endif
	for (int m = 0; m < 3; m++) {
		_myMembers[m]->Speedmove(argInputSpeed, argdomove);
		}
	}

#pragma endregion
#pragma region priv


	/*
* acos in radd
*
* a=8
* b=6
* c=7
*
* cos A= (b^2+c^2- a^2)/ 2bc
* cos A= (36+49-64) / 84
* cos A=0.25
* A=cos^-1(0.25)
* A=75.5224
*
* cos B= (a^2+c^2- b^2)/ 2ac
* cos B=0.68
* B=cos^-1(0.68)
* B= 46.6
*
* shoulderanle= sq(armangle) + sg(shinangle)
*
*
*          <--------------------Trabsverse T ---------------------->
*
*
*								front
*				FR0		[0]								FL0   [3]
*				FR1		[1]								FL1   [4]
*				FR2		[2]								FL2   [5]
*
*
*				BR0		[6]								BL0	  [9]
*				BR1		[7]								BL1  [10]
*				BR2		[8]								BL2	 [11]
*
*	front view looing at FR0
*
*				|\
*				| \
* input H		|  \
* input T		|   \
*			H	|    \  Z'  find Z'   Angle T addjudt for FR[0]
*				|     \
*				|      \
*				|_      \
*				|_|______\
*					T
*
*    H^2 + T^2 = H^
*
*	AngleT = Tan^-1(T/H)
*
*/


	float LegMatik::GetRawAngle(float argOposit, float argAdjascent) {
		float rads = atan(argOposit / argAdjascent);
		return rads * (180 / PI);
		}

	double LegMatik::GetZprime(float arhHeight, float argTransverse) {
		return sqrt((arhHeight * arhHeight) + (argTransverse * argTransverse));
	//	return sqrt88x((arhHeight * arhHeight) + (argTransverse * argTransverse));
		}

	/*
	* side view looking ar FR0
	*
	* *				| \
	*				|   \
	* input Z'		|     \
	* input F		|       \
	*		     Z' |         \  Z  find Z
	*				|           \
	*				|            \
	*				|_            \
	*				|_|____________\
	*					F
	*/

	//reuse get z'func

	/*
	*                    A
	*				---------------
	*				\             /
	*				  \          /
	*				Z   \       /  C
	*					  \    /
	*					   \  /
	*						\/
	*
	*   Find angle C  adjust for  FR[1]   ACZ
	*   Find angle Z  adjust for  FR[2]   CZA
	*
	*
	* */

	float LegMatik::getANgleSSS(float argArm, float argOpositEdge, float argthird) {

		float rads = acos(((argArm * argArm) + (argthird * argthird) - (argOpositEdge * argOpositEdge)) / (argArm * argthird * 2));

		return rads * (180 / PI);
		}

	float LegMatik::sqrt88x(const float x)
		{
		union {
			int i;
			float x;
			} u;

		u.x = x;
		u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
		return u.x;
		}

	void LegMatik::XYZ_inputConversion(int argT, int argD, int argH   ) {
		
			#ifdef LOGDEBUG
			if (this->_charId == 'a') {
				Serial.println(" ProcessRawAnglesGetDiffs for leg ");
				Serial.println(String(this->_charId)); Serial.print(" ["); Serial.print(argT); Serial.print(","); Serial.print(argD); Serial.print(","); Serial.print(argH); Serial.println("]");
				//Serial.print(""); Serial.println(String(this->_charId) + " [" + String(argT) + "," + String(argT) + "," + String(argT) + "]");
				}
		#endif


		float Zprime = GetZprime(argH, argT);
		float S0Angle = GetRawAngle(argT, Zprime);

		float z = GetZprime(argD, Zprime);
		float D_ZprimeAngle = GetRawAngle(argD, Zprime);
		float S1Angle = getANgleSSS(ARMLEN, CALFLEN, z);
		float S2Angle = getANgleSSS(CALFLEN, z, ARMLEN);	
		// INT CONVERSION HERE 

		int s0AngleInt = (int)S0Angle;
		int sDZprime = (int)D_ZprimeAngle;
		int s1AngleInt = (int)S1Angle;
		int s2AngleInt = (int)S2Angle;

		int rounded0 = round(S0Angle);
		int roundedDzprime = round(D_ZprimeAngle);
		int rounded1 = round(S1Angle);
		int rounded2 = round(S2Angle);

		
			diffs[0]=	_myShoulder->ProcessRawAnglesGetDiffs(   rounded0, roundedDzprime, rounded1, rounded2);
			diffs[1] = _myArm->ProcessRawAnglesGetDiffs( rounded0, roundedDzprime, rounded1, rounded2);
			diffs[2] = _myCalf->ProcessRawAnglesGetDiffs(  rounded0, roundedDzprime, rounded1, rounded2);
			
		 
		#ifdef LOGDEBUG
		/*	if (this->_charId == 'a') {
				Serial.println(" ProcessRawAnglesGetDiffs");
				String Outputstr = "";
				String pl = ",";
				Outputstr = " Legid=" + (char)this->_charId + pl + " " + rounded0 + pl + roundedDzprime + pl + rounded1 + pl + rounded2;
				Serial.print(""); Serial.println(Outputstr);
				}*/
			
		#endif // LOGDEBUG
		}

	//largest displacement servo will run at speeedCoefficient = 1.0  * inputspeed
	// adjust pseeds of other two proportionally to the disp diff vs 1.0 speed
	void LegMatik::CalcOptimalSpeed() {
		int temp_L_Diff = 0;
		int temp_M_Diff = 0;
		int temp_s_Diff = 6000;
		 
	 
		int indx_L = 0;
		int indx_M = 2;
		int indx_s = 1;
		
	 
		for (int i = 0; i < 3; i++) {
			if (diffs[i] >= temp_L_Diff) {
				temp_L_Diff = diffs[i];
				indx_L = i;
				}
			if (diffs[i] < temp_s_Diff) {
				temp_s_Diff = diffs[i];
				indx_s = i;
				}
			}

		 
		if (indx_L == 0 && indx_s == 2 || indx_L == 2 && indx_s == 0) { indx_M = 1; }
		else if (indx_L == 1 && indx_s == 2 || indx_L == 2 && indx_s == 1) { indx_M = 0; }
		 
		float SpeedCoef_fors;
		float SpeedCoef_forM;
		if (temp_L_Diff==0) {
			diffs[indx_L] = diffs[indx_M] = diffs[indx_s] = 0.0;
		  SpeedCoef_forM = 1.0;
		  SpeedCoef_fors = 1.0;
			}
		else {


			SpeedCoef_forM = (float)diffs[indx_M] / (float)diffs[indx_L];
			SpeedCoef_fors = (float)diffs[indx_s] / (float)diffs[indx_L];
		  //if (SpeedCoef_forM < 2.0)SpeedCoef_forM = 2.0;
		  //if (SpeedCoef_fors < 2.0)SpeedCoef_fors = 2.0;
			}

		SpeedPercentCoefs[indx_L] = 100.0;
		SpeedPercentCoefs[indx_s] = SpeedCoef_forM*100.0;
		SpeedPercentCoefs[indx_M] = SpeedCoef_fors*100.0;
	

		this->_myMembers[indx_L]->SpeedCoef = SpeedPercentCoefs[indx_L];
		this->_myMembers[indx_s]->SpeedCoef = SpeedPercentCoefs[indx_s];
		this->_myMembers[indx_M]->SpeedCoef = SpeedPercentCoefs[indx_M];
		//ok need suoer clean
		
	#ifdef LOGDEBUG
		if((char)this->_charId=='a'){
		Serial.print("CalcOptimalSpeed for legid  ");	Serial.println( this->_charId);
	
		Serial.print("L Diff= svoId."); this->_myMembers[indx_L]->PrintMe(); Serial.print( "Diff->"); Serial.println(diffs[indx_L]);
		Serial.print("M Diff= svoId."); this->_myMembers[indx_M]->PrintMe(); Serial.print("Diff->"); Serial.println(diffs[indx_M]);
		Serial.print("s Diff= svoId."); this->_myMembers[indx_s]->PrintMe(); Serial.print("diff->"); Serial.println(diffs[indx_s]);
			}
	#endif // LOGDEBUG

		
		}

#pragma endregion
#pragma region privs


#pragma endregion