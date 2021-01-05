/*
 Name:		Svo.cpp
 Created:	12/26/2020 11:44:31 PM
 Author:	lamri
 Editor:	http://www.visualmicro.com


#include "SvoV2.h"

*/



#if defined(ARDUINO_ARCH_AVR)

#include <avr/interrupt.h>
#include <Arduino.h>

#include "SvoV2.h"

#define MCRO_usToTicks(_us)    (( clockCyclesPerMicrosecond()* _us) / 8)     // converts microseconds to tick (assumes prescale of 8)  // 12 Aug 2009
#define MCRO_ticksToUs(_ticks) (( (unsigned)_ticks * 8)/ clockCyclesPerMicrosecond() ) // converts from ticks back to microseconds

#define LOGDEBUG

#define TRIM_DURATION       2   // compensation ticks to trim adjust for digitalWrite delays // 12 August 2009

//#define NBR_TIMERS        (SVOV2_MAX_SERVOS / SVOV2S_PER_TIMER)

static SvoV2_t svoV2sArra[SVOV2_MAX_SERVOS];
static volatile int8_t channelArra[_Nbr_16timers];

uint8_t svoV2Cnt = 0;


// convenience macros
#define MCRO_SVOV2_INDEX_TO_TIMER(_servo_nbr) ((timer16_Sequence_t)(_servo_nbr / SVOV2S_PER_TIMER)) // returns the timer controlling this sevo
#define MCRO_SVOV2_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SVOV2S_PER_TIMER)       // returns the index of the sevo on this timer
#define MCRO_SVOV2_INDEX(_timer,_channel)  ((_timer*SVOV2S_PER_TIMER) + _channel)     // macro to access sevo index by timer and channel
#define MCRO_SVOV2(_timer,_channel)  (svoV2sArra[MCRO_SVOV2_INDEX(_timer,_channel)])            // macro to access sevo class by timer and channel

#define MCRO_SVOV2_MIN() (SVOV2_MIN_PULSE_WIDTH - this->_minPwm * RESOLUTION)  // minimum value in uS for this sevo
#define MCRO_SVOV2_MAX() (SVOV2_MAX_PULSE_WIDTH - this->_maxPwm * RESOLUTION)  // maximum value in uS for this sevo

/************ static functions common to all instances ***********************/

static inline void handle_interrupts(timer16_Sequence_t argTimer, volatile uint16_t* TCNTn, volatile uint16_t* OCRnA)
    {
    if (channelArra[argTimer] < 0)
        *TCNTn = 0; // channel set to -1 indicated that refresh interval completed so reset the argTimer
    else {
        if (MCRO_SVOV2_INDEX(argTimer, channelArra[argTimer]) < svoV2Cnt && MCRO_SVOV2(argTimer, channelArra[argTimer]).Pin.pinActive == true)
            digitalWrite(MCRO_SVOV2(argTimer, channelArra[argTimer]).Pin.pinNum, LOW); // pulse this channel low if activated
        }

    channelArra[argTimer]++;    // increment to the next channel
    if (MCRO_SVOV2_INDEX(argTimer, channelArra[argTimer]) < svoV2Cnt && channelArra[argTimer] < SVOV2S_PER_TIMER) {


            // Extension for slowmove
        if (MCRO_SVOV2(argTimer, channelArra[argTimer]).speed) {
            // Increment ticks by speed until we reach the target.
            // When the target is reached, speed is set to 0 to disable that code.
            if (MCRO_SVOV2(argTimer, channelArra[argTimer]).target > MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks) {
                MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks += MCRO_SVOV2(argTimer, channelArra[argTimer]).speed;
                if (MCRO_SVOV2(argTimer, channelArra[argTimer]).target <= MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks) {
                    MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks = MCRO_SVOV2(argTimer, channelArra[argTimer]).target;
                    MCRO_SVOV2(argTimer, channelArra[argTimer]).speed = 0;
                    }
                }
            else {
                MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks -= MCRO_SVOV2(argTimer, channelArra[argTimer]).speed;
                if (MCRO_SVOV2(argTimer, channelArra[argTimer]).target >= MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks) {
                    MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks = MCRO_SVOV2(argTimer, channelArra[argTimer]).target;
                    MCRO_SVOV2(argTimer, channelArra[argTimer]).speed = 0;
                    }
                }
            }
            // End of Extension for slowmove

        *OCRnA = *TCNTn + MCRO_SVOV2(argTimer, channelArra[argTimer]).ticks;
        if (MCRO_SVOV2(argTimer, channelArra[argTimer]).Pin.pinActive == true)     // check if activated
            digitalWrite(MCRO_SVOV2(argTimer, channelArra[argTimer]).Pin.pinNum, HIGH); // its an active channel so pulse it high


        }
    else {
      // finished all channels so wait for the refresh period to expire before starting over
        if (((unsigned)*TCNTn) + RESOLUTION < MCRO_usToTicks(SVOV2_REFRESH_INTERVAL))  // allow a few ticks to ensure the next OCR1A not missed
            *OCRnA = (unsigned int)MCRO_usToTicks(SVOV2_REFRESH_INTERVAL);
        else
            *OCRnA = *TCNTn + RESOLUTION;  // at least SVOV2_REFRESH_INTERVAL has elapsed
        channelArra[argTimer] = -1; // this will get incremented at the end of the refresh period to start again at the first channel
        }
    }

#ifndef WIRING // Wiring pre-defines signal handlers so don't define any if compiling for the Wiring platform
// Interrupt handlers for Arduino
#if defined(_useTmr1)
SIGNAL(TIMER1_COMPA_vect)
    {
    handle_interrupts(_timer1, &TCNT1, &OCR1A);
    }
#endif

#if defined(_useTmr3)
SIGNAL(TIMER3_COMPA_vect)
    {
    handle_interrupts(_timer3, &TCNT3, &OCR3A);
    }
#endif

#if defined(_useTmr4)
SIGNAL(TIMER4_COMPA_vect)
    {
    handle_interrupts(_timer4, &TCNT4, &OCR4A);
    }
#endif

#if defined(_useTmr5)
SIGNAL(TIMER5_COMPA_vect)
    {
    handle_interrupts(_timer5, &TCNT5, &OCR5A);
    }
#endif

#elif defined WIRING
// Interrupt handlers for Wiring
#if defined(_useTmr1)
void Timer1Service()
    {
    handle_interrupts(_timer1, &TCNT1, &OCR1A);
    }
#endif
#if defined(_useTmr3)
void Timer3Service()
    {
    handle_interrupts(_timer3, &TCNT3, &OCR3A);
    }
#endif
#endif


static void initISR(timer16_Sequence_t timer)
    {
#if defined (_useTmr1)
    if (timer == _timer1) {
        TCCR1A = 0;             // normal counting mode
        TCCR1B = _BV(CS11);     // set prescaler of 8
        TCNT1 = 0;              // clear the timer count
    #if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)
        TIFR |= _BV(OCF1A);      // clear any pending interrupts;
        TIMSK |= _BV(OCIE1A);  // enable the output compare interrupt
    #else
        // here if not ATmega8 or ATmega128
        TIFR1 |= _BV(OCF1A);     // clear any pending interrupts;
        TIMSK1 |= _BV(OCIE1A); // enable the output compare interrupt
    #endif
    #if defined(WIRING)
        timerAttach(TIMER1OUTCOMPAREA_INT, Timer1Service);
    #endif
        }
#endif

#if defined (_useTmr3)
    if (timer == _timer3) {
        TCCR3A = 0;             // normal counting mode
        TCCR3B = _BV(CS31);     // set prescaler of 8
        TCNT3 = 0;              // clear the timer count
    #if defined(__AVR_ATmega128__)
        TIFR |= _BV(OCF3A);     // clear any pending interrupts;
        ETIMSK |= _BV(OCIE3A);  // enable the output compare interrupt
    #else
        TIFR3 = _BV(OCF3A);     // clear any pending interrupts;
        TIMSK3 = _BV(OCIE3A); // enable the output compare interrupt
    #endif
    #if defined(WIRING)
        timerAttach(TIMER3OUTCOMPAREA_INT, Timer3Service);  // for Wiring platform only
    #endif
        }
#endif

#if defined (_useTmr4)
    if (timer == _timer4) {
        TCCR4A = 0;             // normal counting mode
        TCCR4B = _BV(CS41);     // set prescaler of 8
        TCNT4 = 0;              // clear the timer count
        TIFR4 = _BV(OCF4A);     // clear any pending interrupts;
        TIMSK4 = _BV(OCIE4A); // enable the output compare interrupt
        }
#endif

#if defined (_useTmr5)
    if (timer == _timer5) {
        TCCR5A = 0;             // normal counting mode
        TCCR5B = _BV(CS51);     // set prescaler of 8
        TCNT5 = 0;              // clear the timer count
        TIFR5 = _BV(OCF5A);     // clear any pending interrupts;
        TIMSK5 = _BV(OCIE5A); // enable the output compare interrupt
        }
#endif
    }

static void finISR(timer16_Sequence_t timer)
    {
        //disable use of the given timer
#if defined WIRING   // Wiring
    if (timer == _timer1) {
    #if defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
        TIMSK1 &= ~_BV(OCIE1A);  // disable timer 1 output compare interrupt
    #else
        TIMSK &= ~_BV(OCIE1A);  // disable timer 1 output compare interrupt
    #endif
        timerDetach(TIMER1OUTCOMPAREA_INT);
        }
    else if (timer == _timer3) {
    #if defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
        TIMSK3 &= ~_BV(OCIE3A);    // disable the timer3 output compare A interrupt
    #else
        ETIMSK &= ~_BV(OCIE3A);    // disable the timer3 output compare A interrupt
    #endif
        timerDetach(TIMER3OUTCOMPAREA_INT);
        }
#else
  //For arduino - in future: call here to a currently undefined function to reset the timer
    (void)timer;  // squash "unused parameter 'timer' [-Wunused-parameter]" warning
#endif
    }

static boolean isTimerActive(timer16_Sequence_t timer)
    {
      // returns true if any sevo  is active on this timer
    for (uint8_t channel = 0; channel < SVOV2S_PER_TIMER; channel++) {
        if (MCRO_SVOV2(timer, channel).Pin.pinActive == true)
            return true;
        }
    return false;
    }


    /****************** end of static functions ******************************/
#pragma region PUB
SvoV2::SvoV2()
    {
    if (svoV2Cnt < SVOV2_MAX_SERVOS) {
        this->svoV2Index = svoV2Cnt++;                    // assign a sevo index to this instance
        svoV2sArra[this->svoV2Index].ticks = MCRO_usToTicks(SVOV2_DEFAULT_PULSE_WIDTH);
    #ifdef  LOGDEBUG
        Serial.println("new servo");// store default values  - 12 Aug 2009
    #endif // LOG
        }
    else
        this->svoV2Index = INVALID_SVOV2;  // too many svoV2sArra
    }

uint8_t SvoV2::attachV1(int pin)
    {
    return this->attachV1(pin, SVOV2_MIN_PULSE_WIDTH, SVOV2_MAX_PULSE_WIDTH);
    }

uint8_t SvoV2::attachV1(int pin, int min, int max)
    {
    if (this->svoV2Index < SVOV2_MAX_SERVOS) {
        pinMode(pin, OUTPUT);                                   // set servo pin to output
        svoV2sArra[this->svoV2Index].Pin.pinNum = pin;
        // todo min/max check: abs(min - MIN_PULSE_WIDTH) /4 < 128 
        this->_minPwm = (SVOV2_MIN_PULSE_WIDTH - min) / RESOLUTION; //resolution of min/max is 4 uS
        this->_maxPwm = (SVOV2_MAX_PULSE_WIDTH - max) / RESOLUTION;
        // initialize the timer if it has not already been initialized 
        timer16_Sequence_t timer = MCRO_SVOV2_INDEX_TO_TIMER(svoV2Index);
        if (isTimerActive(timer) == false)
            initISR(timer);
        svoV2sArra[this->svoV2Index].Pin.pinActive = true;  // this must be set after the check for isTimerActive
        }
    return this->svoV2Index;
    }


uint8_t SvoV2::Attach(int argId)
    {
    int pin;
    if (this->svoV2Index < SVOV2_MAX_SERVOS) {

        pin = this->_myPin;


        pinMode(pin, OUTPUT);                                   // set sevo pin to output
        svoV2sArra[this->svoV2Index].Pin.pinNum = pin;
        // todo _minPwm/_maxPwm check: abs(_minPwm - SVOV2_MIN_PULSE_WIDTH) /4 < 128
        //this->_minPwm = 0; //resolution of _minPwm/_maxPwm is 4 uS
        //this->_maxPwm = 0;

        this->_minPwm = (SVOV2_MIN_PULSE_WIDTH - 512) / RESOLUTION; //resolution of _minPwm/_maxPwm is 4 uS
        this->_maxPwm = (SVOV2_MAX_PULSE_WIDTH - 2534) / RESOLUTION;
        // initialize the timer if it has not already been initialized
        timer16_Sequence_t timer = MCRO_SVOV2_INDEX_TO_TIMER(svoV2Index);
        if (isTimerActive(timer) == false)
            initISR(timer);
        svoV2sArra[this->svoV2Index].Pin.pinActive = true;  // this must be set after the check for isTimerActive

        }
    return this->svoV2Index;
    }


//void SvoV2::InitPositions() {
//    _curPos = this->ReadMicroseconds();
//    _lastPos = _curPos;
//    _nextPos = _curPos;
//    }
void SvoV2::Detach()
    {
    svoV2sArra[this->svoV2Index].Pin.pinActive = false;
    timer16_Sequence_t timer = MCRO_SVOV2_INDEX_TO_TIMER(svoV2Index);
    if (isTimerActive(timer) == false) {
        finISR(timer);
        }
    }

void SvoV2::Write(int value)
    {
    byte channel = this->svoV2Index;
    svoV2sArra[channel].value = value;
    if (value < SVOV2_MIN_PULSE_WIDTH)
        {  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
        if (value < 0) value = 0;
        if (value > 270) value = 270;

        value = map(value, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
    #ifdef  LOGDEBUG
        Serial.print("min ");    Serial.print(_minPwm);    Serial.print("  ");    Serial.print("max "); Serial.println(_maxPwm);
        Serial.print("MIN ");    Serial.print(MCRO_SVOV2_MIN());    Serial.print("  ");    Serial.print("AMX "); Serial.println(MCRO_SVOV2_MAX());
        Serial.print("wroteANGLE =");  Serial.println(value);

    #endif // LOG


        }
    this->WriteMicroseconds(value);
    }

int  SvoV2::ConvertDegreesToPulsw(int argDegrees) {

    if (argDegrees < SVOV2_MIN_PULSE_WIDTH)
        {  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
        if (argDegrees < 0) argDegrees = 0;
        if (argDegrees > 270) argDegrees = 270;
        argDegrees = map(argDegrees, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
        }
    else
        if (argDegrees > SVOV2_MAX_PULSE_WIDTH) {
            argDegrees = 270;
            }

    return argDegrees;
    }
//
//
//void SvoV2::Write(int value, int rate)
//    {
//    if (value < SVOV2_MIN_PULSE_WIDTH)
//        {  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
//        if (value < 0) value = 0;
//        if (value > 270) value = 270;
//        value = map(value, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
//        }
//    this->WriteMicroseconds(value);
//    }


void SvoV2::WriteMicroseconds(int value)
    {
#ifdef LOGDEBUG
    Serial.print("wroteMs =");  Serial.println(value);
#endif
      // calculate and store the values for the given channel
    byte channel = this->svoV2Index;
    svoV2sArra[channel].value = value;
    if ((channel < SVOV2_MAX_SERVOS))   // ensure channel is valid
        {
        if (value < MCRO_SVOV2_MIN())          // ensure pulse width is valid
            value = MCRO_SVOV2_MIN();
        else if (value > MCRO_SVOV2_MAX())
            value = MCRO_SVOV2_MAX();

        value = value - TRIM_DURATION;
        value = MCRO_usToTicks(value);  // convert to ticks after compensating for interrupt overhead - 12 Aug 2009

        uint8_t oldSREG = SREG;
        cli();
        svoV2sArra[channel].ticks = value;
        // Disable slowmove logic.
        svoV2sArra[channel].speed = 0;
        // End of Extension for slowmove
        SREG = oldSREG;


        }

    }

// Extension for slowmove
/*

          speed=0 - Full speed, identical to write
          speed=1 - Minimum speed
          speed=255 - Maximum speed
*/

void SvoV2::SpeedMoveFromOffset(int value, uint8_t speed) {

    int newVAl = 0;
    if (this->_id == 0 || this->_id == 9) {
        newVAl = this->_GlobalZeroAngle + value;
    #ifdef  LOGDEBUG
        Serial.println(" i am 0 ");
    #endif // LOG
        }
    else
        if (this->_id == 3 || this->_id == 6) {
            newVAl = this->_GlobalZeroAngle - value;
            }
        else
            if (this->_id % 3 == 2) {

                newVAl = this->_GlobalZeroAngle + value;
            #ifdef  LOGDEBUG
                Serial.println(" calf");
            #endif // LOG
                }

            else
                // if (this->_id % 3 == 1)
                {

                if (this->_isForward) {
                    newVAl = this->_GlobalZeroAngle + value;
                #ifdef  LOGDEBUG
                    Serial.println(" NOT calf forward");
                #endif // LOG

                    }
                else
                    {
                    newVAl = this->_GlobalZeroAngle - value;
                #ifdef  LOGDEBUG
                    Serial.println(" NOT calf BAckwards");
                #endif // LOG
                    }


                }

#ifdef LOGDEBUG     
    Serial.print("my zero ="); Serial.print(this->_GlobalZeroAngle);
    Serial.print(" newVAl = "); Serial.println(newVAl);

#endif
    this->Speedmove(newVAl, speed);
//    }
//else 
//    //handle iput in microseconds
//    {
//    this->Speedmove(this->offsettedAngle(value), speed);
//    }

    }


void SvoV2::Speedmove(int value, uint8_t speed) {
    // This fuction is a copy of write and witeMicroseconds but value will be saved
    // in target instead of in ticks in the servo structure and speed will be save
    // there too.

    if (speed) {
        if (value < SVOV2_MIN_PULSE_WIDTH) {
        // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
            if (value < 0) value = 0;
            if (value > 270) value = 270;
            value = map(value, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
            }
            // calculate and store the values for the given channel
        byte channel = this->svoV2Index;
        if ((channel >= 0) && (channel < SVOV2_MAX_SERVOS)) {   // ensure channel is valid
            if (value < MCRO_SVOV2_MIN())          // ensure pulse width is valid
                value = MCRO_SVOV2_MIN();
            else if (value > MCRO_SVOV2_MAX())
                value = MCRO_SVOV2_MAX();

            value = value - TRIM_DURATION;
            value = MCRO_usToTicks(value);  // convert to ticks after compensating for interrupt overhead - 12 Aug 2009

            // Set speed and direction
            uint8_t oldSREG = SREG;
            cli();
            svoV2sArra[channel].target = value;
            svoV2sArra[channel].speed = speed;
            SREG = oldSREG;
            }
        }
    else {
        Write(value);
        }
    }
    // End of Extension for slowmove

int SvoV2::Read() // return the value as degrees
    {
    return  map(this->ReadMicroseconds() + 1, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX(), 0, 270);
    }

int SvoV2::ReadMicroseconds()
    {
    unsigned int pulsewidth;
    if (this->svoV2Index != INVALID_SVOV2)
        pulsewidth = MCRO_ticksToUs(svoV2sArra[this->svoV2Index].ticks) + TRIM_DURATION;   // 12 aug 2009
    else
        pulsewidth = 0;

    return pulsewidth;
    }

bool SvoV2::Attached()
    {
    return svoV2sArra[this->svoV2Index].Pin.pinActive;
    }

bool SvoV2::IsMoving() {
    byte channel = this->svoV2Index;
    int value = svoV2sArra[channel].value;

    if (value < SVOV2_MIN_PULSE_WIDTH) {
        if (this->Read() != value) {
            return true;
            }
        }
    else {
        if (this->ReadMicroseconds() != value) {
            return true;
            }
        }
    return false;
    }
void SvoV2::Stop() {
    this->Write(this->Read());
    }


void  SvoV2::ZeroMe() {
    this->Speedmove(this->_GlobalZeroAngle, 20);

    }


int SvoV2::GetGlobalZero() {
    return this->_GlobalZeroAngle;
    }

int SvoV2::GetId() {
    return this->_id;
    }

#pragma endregion
#pragma region PRIV
//int SvoV2::offsettedAngle(int argAngle) {
//    int ConvertedAngle = 0;
//  
//    if (this->_isForward == true) {
//        ConvertedAngle = this->_midAngle + argAngle;
//        }
//    else
//        if (this->_isForward == false) {
//            ConvertedAngle = this->_midAngle - argAngle;
//            }
//
//    return ConvertedAngle;
//    }



//int SvoV2::OffsettedUs(int argUS) {
//     
//    int ConvertedUS = 0;
//    if (this->_isForward == true) {
//        ConvertedUS = _midUs + argUS;
//        }
//    else
//        if (this->_isForward == false) {
//            ConvertedUS = _midUs - argUS;
//            }
//
//    return ConvertedUS;
//    }
#pragma endregion

#pragma region Locals
//local
void SvoV2::SetupById(int argId) {

    switch (argId) {

               // the shoulder group. 
               // can be displaced 50 degrees out , and 40 in.
               // works well to minimize legs touching eachother under the body.
               // while having a larger out angle for more reach
            case 0:
                this->_name = "FL0";
                // pin =22;
                this->_isForward = true; //positive Transvers should open the shoulder OUT away from body
                this->_GlobalZeroAngle = TwoHundred;
            #ifdef LOGDEBUG
                Serial.print(" global zwrofor id0="); Serial.println(_GlobalZeroAngle);
            #endif
                this->_GlobalMaxOutAngle = 250;// can go to  260;
                this->_GlobalMaxINAngle = 160;// can go down to 150; before physical damage
                break;
            case 3:
                this->_name = "FR0";
                // pin =23;
                this->_isForward = false; //positive Transvers should open the shoulder OUT away from body
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 250;// can go to  250 only !!!
                this->_GlobalMaxINAngle = 160;// can go down to 140; before  phd lol
                break;
            case 6:
                this->_name = "BL0";
                // pin =28;
                this->_isForward = false; //positive Transvers should open the shoulder OUT away from body
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 250;
                this->_GlobalMaxINAngle = 160;
                break;
            case 9:
                this->_name = "BR0";
                // pin =29;
                this->_isForward = true; //positive Transvers should open the shoulder OUT away from body
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 250;
                this->_GlobalMaxINAngle = 140;
                break;
//============================================================================
                    //the arms group , should have a 48 degree displacement from center to minimize phd while keeping symetry ..


            case 1:
                this->_name = "FL1";
                // pin =24;
                this->_isForward = true; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 248;
                this->_GlobalMaxINAngle = 152;
                break;
            case 4:
                this->_name = "FR1";
                // pin =25;
                this->_isForward = true; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 248;
                this->_GlobalMaxINAngle = 152;
                break;
            case 7:
                this->_name = "BL1";
                // pin =30;
                this->_isForward = false; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 248;
                this->_GlobalMaxINAngle = 152;
                break;
            case 10:
                this->_name = "BR1";
                // pin =31;
                this->_isForward = false; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = 200;
                this->_GlobalMaxOutAngle = 260;
                this->_GlobalMaxINAngle = 150;
                break;
//================================================================================
                    //calves .. easy mode , servos are already placed correcty
            case 2:
                this->_name = "FL2";
                // pin =26;
                this->_isForward = true; //180 irl = 135 here   
                this->_GlobalZeroAngle = 110;
                this->_GlobalMaxOutAngle = 270;
                this->_GlobalMaxINAngle = 135;
                break;
            case 5:
                this->_name = "FR2";
                // pin =27;
                this->_isForward = true; //180 irl = 135 here   
                this->_GlobalZeroAngle = 110;
                this->_GlobalMaxOutAngle = 270;
                this->_GlobalMaxINAngle = 135;
                break;
            case 8:
                this->_name = "BL2";
                // pin =32;
                this->_isForward = true; //180 irl = 135 here   
                this->_GlobalZeroAngle = 110;
                this->_GlobalMaxOutAngle = 270;
                this->_GlobalMaxINAngle = 135;
                break;
            case 11:
                this->_name = "BR2";
                // pin =33;
                this->_isForward = true; //180 irl = 135 here   
                this->_GlobalZeroAngle = 110;
                this->_GlobalMaxOutAngle = 270;
                this->_GlobalMaxINAngle = 135;
                break;

                argId = constrain(argId, 0, 11);
                this->_id = argId;

                this->_GlobalZeroPWM = map(_GlobalZeroAngle, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
                this->_GlobalMaxOutPMW = map(_GlobalMaxOutAngle, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
                this->_GlobalMaxINPMW = map(_GlobalMaxINAngle, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
        }//xSwitch
    }

int SvoV2::AssignPin(int argId) {
    int tempin = 0;

    switch (argId) {

              // the shoulder group. 
              // can be displaced 50 degrees out , and 40 in.
              // works well to minimize legs touching eachother under the body.
              // while having a larger out angle for more reach
            case 0:
                tempin = 22;
            #ifdef LOGDEBUG
                Serial.println(" assigend servo 0 pin 22");
            #endif
                break;
            case 3:
                tempin = 23;
                break;
            case 6:
                tempin = 28;
                break;
            case 9:
                tempin = 29;
                break;
//============================================================================
                    //the arms group , should have a 48 degree displacement from center to minimize phd while keeping symetry ..


            case 1:
                tempin = 24;
            #ifdef LOGDEBUG
                Serial.println(" assigend servo 1 pin 2");
            #endif
                break;
            case 4:
                tempin = 25;
                break;
            case 7:
                tempin = 30;
                break;
            case 10:
                tempin = 31;
                break;
//================================================================================
                    //calves .. easy mode , servos are already placed correcty
            case 2:
                tempin = 26;
                break;
            case 5:
                tempin = 27;
                break;
            case 8:
                tempin = 32;
                break;
            case 11:
                tempin = 33;

                break;
        }//xSwitch
    return tempin;
    }
#pragma endregion
#endif // ARDUINO_ARCH_AVR



