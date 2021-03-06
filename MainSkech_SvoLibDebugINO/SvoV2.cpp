/*
 Name:		Svo.cpp
 Created:	12/26/2020 11:44:31 PM
 Author:	lamri
 Editor:	http://www.visualmicro.com


#include "SvoV2.h"

*/
// #define LOGDEBUG
#if defined(ARDUINO_ARCH_AVR)
#include <avr/interrupt.h>
#include <Arduino.h>

#include "SvoV2.h"
#pragma region HarUino

#define MCRO_usToTicks(_us)    (( clockCyclesPerMicrosecond()* _us) / 8)     // converts microseconds to tick (assumes prescale of 8)  // 12 Aug 2009
#define MCRO_ticksToUs(_ticks) (( (unsigned)_ticks * 8)/ clockCyclesPerMicrosecond() ) // converts from ticks back to microseconds



#define TRIM_DURATION       2   // compensation ticks to trim adjust for digitalWrite delays // 12 August 2009

//#define NBR_TIMERS        (SVOV2_MAX_SERVOS / SVOV2S_PER_TIMER)

static SvoV2_t StaticSvoV2sArra[SVOV2_MAX_SERVOS];
static volatile int8_t channelArra[_Nbr_16timers];

uint8_t svoV2Cnt = 0;


// convenience macros
#define MCRO_SVOV2_INDEX_TO_TIMER(_servo_nbr) ((timer16_Sequence_t)(_servo_nbr / SVOV2S_PER_TIMER)) // returns the timer controlling this sevo
#define MCRO_SVOV2_INDEX_TO_CHANNEL(_servo_nbr) (_servo_nbr % SVOV2S_PER_TIMER)       // returns the index of the sevo on this timer
#define MCRO_SVOV2_INDEX(_timer,_channel)  ((_timer*SVOV2S_PER_TIMER) + _channel)     // macro to access sevo index by timer and channel
#define MCRO_SVOV2(_timer,_channel)  (StaticSvoV2sArra[MCRO_SVOV2_INDEX(_timer,_channel)])            // macro to access sevo class by timer and channel

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
#if defined(_Uses_Tmr1)
SIGNAL(TIMER1_COMPA_vect)
    {
    handle_interrupts(_timer1, &TCNT1, &OCR1A);
    }
#endif

#if defined(_Uses_Tmr3)
SIGNAL(TIMER3_COMPA_vect)
    {
    handle_interrupts(_timer3, &TCNT3, &OCR3A);
    }
#endif

#if defined(_Uses_Tmr4)
SIGNAL(TIMER4_COMPA_vect)
    {
    handle_interrupts(_timer4, &TCNT4, &OCR4A);
    }
#endif

#if defined(_Uses_Tmr5)
SIGNAL(TIMER5_COMPA_vect)
    {
    handle_interrupts(_timer5, &TCNT5, &OCR5A);
    }
#endif

#elif defined WIRING
// Interrupt handlers for Wiring
#if defined(_Uses_Tmr1)
void Timer1Service()
    {
    handle_interrupts(_timer1, &TCNT1, &OCR1A);
    }
#endif
#if defined(_Uses_Tmr3)
void Timer3Service()
    {
    handle_interrupts(_timer3, &TCNT3, &OCR3A);
    }
#endif
#endif


static void initISR(timer16_Sequence_t timer)
    {
#if defined (_Uses_Tmr1)
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

#if defined (_Uses_Tmr3)
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

#if defined (_Uses_Tmr4)
    if (timer == _timer4) {
        TCCR4A = 0;             // normal counting mode
        TCCR4B = _BV(CS41);     // set prescaler of 8
        TCNT4 = 0;              // clear the timer count
        TIFR4 = _BV(OCF4A);     // clear any pending interrupts;
        TIMSK4 = _BV(OCIE4A); // enable the output compare interrupt
        }
#endif

#if defined (_Uses_Tmr5)
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

#pragma endregion
    /****************** end of static functions ******************************/
#pragma region PUB
SvoV2::SvoV2(  )
    {
    if (svoV2Cnt < SVOV2_MAX_SERVOS) {
        this->svoV2Index = svoV2Cnt++;// assign a sevo index to this instance
        StaticSvoV2sArra[this->svoV2Index].ticks = MCRO_usToTicks(SVOV2_DEFAULT_PULSE_WIDTH);
        SetupById(this->svoV2Index);
        }
    else
        this->svoV2Index = INVALID_SVOV2;  // too many svoV2sArra
    }
uint8_t SvoV2::AttachSelf( )
    {
    if (this->svoV2Index < SVOV2_MAX_SERVOS) {

        pinMode(this->_myPin, OUTPUT);                                   // set sevo pin to output
        StaticSvoV2sArra[this->svoV2Index].Pin.pinNum = this->_myPin;
        this->_minPwm = (SVOV2_MIN_PULSE_WIDTH - 512) / RESOLUTION; //resolution of _minPwm/_maxPwm is 4 uS
        this->_maxPwm = (SVOV2_MAX_PULSE_WIDTH - 2534) / RESOLUTION;
        // initialize the timer if it has not already been initialized
        timer16_Sequence_t timer = MCRO_SVOV2_INDEX_TO_TIMER(svoV2Index);
        if (isTimerActive(timer) == false)
            initISR(timer);
        StaticSvoV2sArra[this->svoV2Index].Pin.pinActive = true;  // this must be set after the check for isTimerActive

        }
    return this->svoV2Index;
    }
void SvoV2::Detach()
    {
    StaticSvoV2sArra[this->svoV2Index].Pin.pinActive = false;
    timer16_Sequence_t timer = MCRO_SVOV2_INDEX_TO_TIMER(svoV2Index);
    if (isTimerActive(timer) == false) {
        finISR(timer);
        }
    }
void SvoV2::PrintMe() {
    
    Serial.print("  _id|"); Serial.print(this->_id);
    Serial.print("  Pin|"); Serial.print(StaticSvoV2sArra[this->_id].Pin.pinNum);
    Serial.print("  zero|"); Serial.print( this->_GlobalZeroAngle);
    Serial.println("");

    }
//speed=0 - Full speed, identical to write ,speed=1 - Minimum speed , speed=255 - Maximum speed
void SvoV2::Speedmove(int value, uint8_t speed) {
    // This fuction is a copy of write and witeMicroseconds but value will be saved
    // in target instead of in ticks in the servo structure and speed will be save
    // there too.

    int valueOffsetted = OffsettedAngle(value);
#ifdef LOGDEBUG
    Serial.print("val=");   Serial.print(value); Serial.print("ofst="); Serial.println(valueOffsetted);
#endif
    if (speed) {
        if (valueOffsetted < SVOV2_MIN_PULSE_WIDTH) {
        // treat valueOffsetteds less than 544 as angles in degrees (valid valueOffsetteds in microseconds are handled as microseconds)
           
           /* if (valueOffsetted < 0) valueOffsetted = 0;
            if (valueOffsetted > 270) valueOffsetted = 270;*/
            valueOffsetted = constrain(valueOffsetted, this->_GlobalMin, this->_GlobalMax);
            valueOffsetted = map(valueOffsetted, 0, 270, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX());
           
           
            }
            // calculate and store the valueOffsetteds for the given channel
        byte channel = this->svoV2Index;
        if ((channel >= 0) && (channel < SVOV2_MAX_SERVOS)) {   // ensure channel is valid
            if (valueOffsetted < MCRO_SVOV2_MIN())          // ensure pulse width is valid
                valueOffsetted = MCRO_SVOV2_MIN();
            else if (valueOffsetted > MCRO_SVOV2_MAX())
                valueOffsetted = MCRO_SVOV2_MAX();

            valueOffsetted = valueOffsetted - TRIM_DURATION;
            valueOffsetted = MCRO_usToTicks(valueOffsetted);  // convert to ticks after compensating for interrupt overhead - 12 Aug 2009

            // Set speed and direction
            uint8_t oldSREG = SREG;
            cli();
            StaticSvoV2sArra[channel].target = valueOffsetted;
            StaticSvoV2sArra[channel].speed = speed;
            SREG = oldSREG;
            }
        }
    else {
        Write(valueOffsetted);
        }
    }
bool SvoV2::Attached()
    {
    return StaticSvoV2sArra[this->svoV2Index].Pin.pinActive;
    }
bool SvoV2::IsMoving() {
    byte channel = this->svoV2Index;
    int value = StaticSvoV2sArra[channel].value;

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
     
    if (this->_id % 3 == 2) this->Speedmove(180, 0);
    else
        this->Speedmove(0, 0);
    }

void  SvoV2::SitMe() {
    if(this->_id%3==0) this->Speedmove(0, 0);
    else
        if (this->_id % 3 == 1) this->Speedmove(-48, 0);
        else
    this->Speedmove(40, 0);
    }
#pragma endregion
#pragma region PRIV
void SvoV2::SetupById(int argId) {
    argId = constrain(argId, 0, 11);

    switch (argId) {

               // the this->shoulder group. 
               // can be displaced 50 degrees out , and 40 in.
               // works well to minimize legs touching eachother under the body.
               // while having a larger out angle for more reach
            case 0:
                this->_myPin = 22;
                this->_isForward = true; //positive Transvers this->should open the this->shoulder OUT away from body
                this->_GlobalZeroAngle = this->shoulderZero;
                this->_GlobalMax = this->shoulderMax;// can go to  260;
                this->_GlobalMin = this->shoulderMin;// can go down to 150; before physical damage
                break;
            case 3:
                this->_myPin = 23;
                this->_isForward = false; //positive Transvers this->should open the this->shoulder OUT away from body
                this->_GlobalZeroAngle = this->shoulderZero;
                this->_GlobalMax = 250;// can go to  250 only !!!
                this->_GlobalMin = this->shoulderMin;// can go down to 140; before  phd lol
                break;
            case 6:
                this->_myPin = 28;
                this->_isForward = false; //positive Transvers this->should open the this->shoulder OUT away from body
                this->_GlobalZeroAngle = this->shoulderZero;
                this->_GlobalMax = 250;
                this->_GlobalMin = this->shoulderMin;
                break;
            case 9:
                this->_myPin = 29;
                this->_isForward = true; //positive Transvers this->should open the this->shoulder OUT away from body
                this->_GlobalZeroAngle = this->shoulderZero;
                this->_GlobalMax = 250;
                this->_GlobalMin = 140;
                break;
//============================================================================
                    //the arms group , this->should have a 48 degree displacement from center to minimize phd while keeping symetry ..


            case 1:
                this->_myPin = 24;
                this->_isForward = true; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = this->armZero;
                this->_GlobalMax = this->armMax;
                this->_GlobalMin = this->armMin;
                break;
            case 4:
                this->_myPin = 25;
                this->_isForward = false; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = this->armZero;
                this->_GlobalMax = this->armMax;
                this->_GlobalMin = this->armMin;
                break;
            case 7:
                this->_myPin = 30;
                this->_isForward = false; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = this->armZero;
                this->_GlobalMax = this->armMax;
                this->_GlobalMin = this->armMin;
                break;
            case 10:
                this->_myPin = 31;
                this->_isForward = true; //a 0 angle makes leg extended,  
                this->_GlobalZeroAngle = this->armZero;
                this->_GlobalMax = this->armMax;
                this->_GlobalMin = this->armMin;
                break;
//================================================================================
                    //calves .. easy mode , servos are already placed correcty
            case 2:
                this->_myPin = 26;
                this->_isForward = true; //180 irl = this->calfMin here   
                this->_GlobalZeroAngle = this->calfZero;
                this->_GlobalMax = this->calfMax;
                this->_GlobalMin = this->calfMin;
                break;
            case 5:
                this->_myPin = 27;
                this->_isForward = true; //180 irl = this->calfMin here   
                this->_GlobalZeroAngle = this->calfZero;
                this->_GlobalMax = this->calfMax;
                this->_GlobalMin = this->calfMin;
                break;
            case 8:
                this->_myPin = 32;
                this->_isForward = true; //180 irl = this->calfMin here   
                this->_GlobalZeroAngle = this->calfZero;
                this->_GlobalMax = this->calfMax;
                this->_GlobalMin = this->calfMin;
                break;
            case 11:
                this->_myPin = 33;
                this->_isForward = true; //180 irl = this->calfMin here   
                this->_GlobalZeroAngle = this->calfZero;
                this->_GlobalMax = this->calfMax;
                this->_GlobalMin = this->calfMin;
                break;



        }//xSwitch

    this->_id = argId;
    }
int SvoV2::OffsettedAngle(int argAngle) {
    int ConvertedAngle = 0;

    if (this->_id % 3 == 2) {
        //180 input -> 110
        //40 input ->270
        //160 input = 180-160 =40 +110 =150
        argAngle = constrain(argAngle, 40, 180);
        if (_isForward)
            {
           
            ConvertedAngle = 180 - argAngle + this->_GlobalMin;
            }
        else {
            ConvertedAngle = this->_GlobalMin-( 180 - argAngle );
            }
        
        }
    else
    if ( _isForward  ) 
        {
        ConvertedAngle = this->_GlobalZeroAngle + argAngle;
        }
    else
       // if (this->_isForward == false)
            {
            ConvertedAngle = this->_GlobalZeroAngle - argAngle;
            }
   ConvertedAngle = constrain(ConvertedAngle, this->_GlobalMin, this->_GlobalMax);
#ifdef LOGDEBUG
   Serial.print("conv");  Serial.println(ConvertedAngle);
#endif // LOGDEBUG
    return ConvertedAngle;
    }
int SvoV2::Read() // return the value as degrees
    {
    return  map(this->ReadMicroseconds() + 1, MCRO_SVOV2_MIN(), MCRO_SVOV2_MAX(), 0, 270);
    }
int SvoV2::ReadMicroseconds()
    {
    unsigned int pulsewidth;
    if (this->svoV2Index != INVALID_SVOV2)
        pulsewidth = MCRO_ticksToUs(StaticSvoV2sArra[this->svoV2Index].ticks) + TRIM_DURATION;   // 12 aug 2009
    else
        pulsewidth = 0;

    return pulsewidth;
    }
void SvoV2::WriteMicroseconds(int value)
    {
      // calculate and store the values for the given channel
    byte channel = this->svoV2Index;
    StaticSvoV2sArra[channel].value = value;
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
        StaticSvoV2sArra[channel].ticks = value;
        // Disable slowmove logic.
        StaticSvoV2sArra[channel].speed = 0;
        // End of Extension for slowmove
        SREG = oldSREG;


        }

    }
void SvoV2::Write(int value)
    {
    byte channel = this->svoV2Index;
    StaticSvoV2sArra[channel].value = value;
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
#pragma endregion
#endif // ARDUINO_ARCH_AVR