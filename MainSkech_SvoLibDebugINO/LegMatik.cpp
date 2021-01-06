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
void LegMatik::MoveToBySpeed(int argT, int argD, int argH, int argspeed, bool argmove) {
	XYZ_inputConversion(argT, argD, argH, argmove, argspeed);
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

	void LegMatik::XYZ_inputConversion(int argT, int argD, int argH, bool argDoMove, int argspeed) {
		
		float Zprime = GetZprime(argH, argT);
		float S0Angle = GetRawAngle(argT, Zprime);

		float z = GetZprime(argD, Zprime);
		float S1AnglePlus = GetRawAngle(argD, Zprime);
		float S1Angle = getANgleSSS(ARMLEN, CALFLEN, z);

	#ifdef LOGDEBUG
		String debstr = "";
		int s1AngleInt_beta = (int)S1Angle;
		int rounded1_beta = round(S1Angle);

		int S1AnglePlus_beta = (int)S1AnglePlus;
		int roundedS1AnglePlus_beta = round(S1AnglePlus);
		debstr = "(" + String(roundedS1AnglePlus_beta) + " - " + String(rounded1_beta) + ")=";
		//Serial.print("("); Serial.print(roundedS1AnglePlus_beta);  Serial.print("-"); Serial.print(rounded1_beta); Serial.print(","); Serial.print(" ");
	#endif // LOGDEBUG


		S1Angle = S1AnglePlus - S1Angle; //normalize for Arms
		float S2Angle = getANgleSSS(CALFLEN, z, ARMLEN);		//old norm for cal was 180 - getAn... );



		// INT CONVERSION HERE 

		int s0AngleInt = (int)S0Angle;
		int s1AngleInt = (int)S1Angle;
		int s2AngleInt = (int)S2Angle;

		int rounded0 = round(S0Angle);
		int rounded1 = round(S1Angle);
		int rounded2 = round(S2Angle);

		if (argDoMove) {
			_myShoulder->Speedmove(rounded0, argspeed);
			_myArm->Speedmove(rounded1, argspeed);
			_myCalf->Speedmove(rounded2, argspeed);
			}
		else
			{
		#ifdef LOGDEBUG
			String Outputstr = "";
			String pl = ",";
			Outputstr = pl + " "+ rounded0 + pl + debstr+ rounded1 + pl + rounded2 + (char)this->_charId;
			Serial.print(""); Serial.println(Outputstr);
		#endif // LOGDEBUG
			}
		
		}
#pragma endregion
#pragma region privs


#pragma endregion