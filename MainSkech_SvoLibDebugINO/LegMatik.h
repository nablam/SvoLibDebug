// LegMatik.h

#ifndef _LEGMATIK_h
#define _LEGMATIK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "SvoV2.h"

#define LEGDEBUG
#define ARMLEN 12
#define CALFLEN 12

class LegMatik {
#pragma region PUBS
	public:
	//LegMatik();
	LegMatik(SvoV2*  argArra, uint8_t argShoulderIndex, uint8_t argArmIndex, uint8_t argCalfIndex, char argCharId);
	void XYZ_inputConversion(int argT, int argD, int argH );
	void CalcOptimalSpeed();
	void MoveTo_curReqval_BySpeedoeff(uint8_t argInputSpeed, bool argdomove);
	//void TestMyServos();
	//void PrintAngles(int argT, int argD, int argH);

	
	char _charId;
#pragma endregion

#pragma region priv
	private:
	SvoV2* _myShoulder;
	SvoV2* _myArm;
	SvoV2* _myCalf;
	SvoV2* _myMembers[3] = { _myShoulder ,_myArm,_myCalf };
	int diffs[3];
	float SpeedPercentCoefs[3];
	

	float GetRawAngle(float argOposit, float argAdjascent);

	double GetZprime(float arhHeight, float argTransverse);

	float getANgleSSS(float argArm, float argOpositEdge, float argthird);

	float sqrt88x(const float x);

	
#pragma endregion
	};

#endif

