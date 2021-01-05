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
	LegMatik(SvoV2*  argArra, uint8_t argShoulderIndex, uint8_t argArmIndex, uint8_t argCalfIndex);
	void TestMyServos();
	void PrintAngles(int argT, int argD, int argH);
	void MoveToBySpeed(int argT, int argD, int argH, int argspeed);
#pragma endregion

#pragma region priv
	private:
	SvoV2* _myShoulder;
	SvoV2* _myArm;
	SvoV2* _myCalf;

	float GetRawAngle(float argOposit, float argAdjascent);

	double GetZprime(float arhHeight, float argTransverse);

	float getANgleSSS(float argArm, float argOpositEdge, float argthird);

	String XYZ_inputConversion(int argT, int argD, int argH, bool argDoMove, int argspeed);

	float sqrt88x(const float x);

#pragma endregion
	};

#endif

