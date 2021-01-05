// SvoV2.h

#ifndef _SVOV2_h
#define _SVOV2_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <inttypes.h>
// Say which 16 bit timers can be used and in what order
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define _Uses_Tmr5
#define _Uses_Tmr1
#define _Uses_Tmr3
#define _Uses_Tmr4
typedef enum { _timer5, _timer1, _timer3, _timer4, _Nbr_16timers } timer16_Sequence_t;

#elif defined(__AVR_ATmega32U4__)
#define _Uses_Tmr1
typedef enum { _timer1, _Nbr_16timers } timer16_Sequence_t;

#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define _Uses_Tmr3
#define _Uses_Tmr1
typedef enum { _timer3, _timer1, _Nbr_16timers } timer16_Sequence_t;

#elif defined(__AVR_ATmega128__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega2561__)
#define _Uses_Tmr3
#define _Uses_Tmr1
typedef enum { _timer3, _timer1, _Nbr_16timers } timer16_Sequence_t;

#else  // everything else
#define _Uses_Tmr1
typedef enum { _timer1, _Nbr_16timers } timer16_Sequence_t;
#endif

//======================================================================================

#define SVOV2_VERSION           1    
#define SVOV2_MIN_PULSE_WIDTH       512
#define SVOV2_MAX_PULSE_WIDTH      2534
#define SVOV2_DEFAULT_PULSE_WIDTH  1500
#define SVOV2_REFRESH_INTERVAL    20000    

#define SVOV2S_PER_TIMER       12      
#define SVOV2_MAX_SERVOS   (_Nbr_16timers  * SVOV2S_PER_TIMER)

#define INVALID_SVOV2         255    
#define RESOLUTION         4   

#if !defined(ARDUINO_ARCH_STM32F4)

typedef struct {
	uint8_t pinNum : 6;// a pin number from 0 to 63
	uint8_t pinActive : 1;// true if this channel is enabled, pin not pulsed if false
	} SvoV2Pin_t;

typedef struct {
	SvoV2Pin_t Pin;
	volatile unsigned int ticks;
	unsigned int value;
	unsigned int target;			// Extension for move
	uint8_t speed;					// Extension for move
	} SvoV2_t;

class SvoV2 {
	public:
	SvoV2();
	SvoV2(bool argIsUsingOffset);
	uint8_t AttachSelf();

	uint8_t attachV1(int pin);
	uint8_t attachV1(int pin, int min, int max);
	//int ConvertDegreesToPulsw(int argDegrees);
	void Detach();
	void Write(int value);
	//void Write(int value, int rate);
	void WriteMicroseconds(int value);
	void Speedmove(int value, uint8_t speed);
	int Read();
	int ReadMicroseconds();
	bool Attached();
	//void InitPositions();

	bool IsMoving();
	void Stop();
	void ZeroMe();

	void PrintMe();

	void SpeedMoveFromOffset(int value, uint8_t speed);
	//int AssignPin(int argId);
	void SetupById(int argId);
	int GetGlobalZero();
	int GetId();
	private:
	uint8_t svoV2Index;               // index into the channel data for this sevo
	int _minPwm;                       // minimum is this value times 4 added to SvoV2_MIN_PULSE_WIDTH    
	int _maxPwm;                       // maximum is this value times 4 added to SvoV2_MAX_PULSE_WIDTH  
	bool _isForward;
	int _myPin;
	bool _useOffsetAngle;
	//int _midAngle;
	//int _midUs;
	//int _curPos;
	//int _lastPos;
	//int _nextPos;

	//char _name[3];
	int _GlobalZeroAngle;// will be hardcoded per id
	int _GlobalMax;
	int _GlobalMin;
	int _id;// 

	//int _GlobalZeroPWM; // will be hardcoded per id

	//int _GlobalMaxOutPMW;
	//int _GlobalMaxINPMW;

	//int offsettedAngle(int argAngle);
	//int OffsettedUs(int argUS);//not needed , delete me 
	};

#endif
#endif

