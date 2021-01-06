// SvoV2.h  24bytes ber object

#ifndef _SVOV2_h
#define _SVOV2_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#pragma region Timers
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

#pragma endregion

#pragma region DFFINES
#define SVOV2_VERSION 1    
#define SVOV2_MIN_PULSE_WIDTH 512
#define SVOV2_MAX_PULSE_WIDTH 2534
#define SVOV2_DEFAULT_PULSE_WIDTH 1500
#define SVOV2_REFRESH_INTERVAL 20000    
#define SVOV2S_PER_TIMER 12      
#define SVOV2_MAX_SERVOS (_Nbr_16timers  * SVOV2S_PER_TIMER)
#define INVALID_SVOV2 255    
#define RESOLUTION 4   
#pragma endregion

#if !defined(ARDUINO_ARCH_STM32F4)

#pragma region SVOstructs
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
#pragma endregion
class SvoV2 {
	public:
	SvoV2( );
	uint8_t AttachSelf();
	void Detach();
	void Speedmove(int value, uint8_t speed);
	bool Attached();
	bool IsMoving();
	void Stop();
	void ZeroMe();
	void SitMe();
	void PrintMe();

	private:
	void SetupById(int argId);
	void Write(int value);
	void WriteMicroseconds(int value);
	int OffsettedAngle(int argAngle);
	int Read();
	int ReadMicroseconds();

	uint8_t svoV2Index; // index into the channel data for this sevo
	int _minPwm;		// minimum is this value times 4 added to SvoV2_MIN_PULSE_WIDTH    
	int _maxPwm;		// maximum is this value times 4 added to SvoV2_MAX_PULSE_WIDTH  
	bool _isForward;
	uint8_t _myPin;
	int _currPwm;
	int _nextPwm;
	int _Adjustedspeed; //leg will adjust this to maintain even servo mtion. most likely a sealing to not exced
	// leg is given xyz . 
	//it calculates a0 a1 a2
	//

	uint8_t _id;
	int _GlobalZeroAngle;// will be hardcoded per id
	int _GlobalMax;// will be hardcoded per id depends on mhysical constraints
	int _GlobalMin;// will be hardcoded per id depends on mhysical constraints
	const int shoulderZero = 200;
	const int shoulderMin = 160;
	const int shoulderMax = 250;
	const int armZero = 200;
	const int armMin = 152;
	const int armMax = 248;
	const int calfZero = 112;
	const int calfMin = 110;
	const int calfMax = 270;

	int EmergencyUm;
	};

#endif
#endif

