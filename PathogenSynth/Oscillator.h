#pragma once

#include <stdint.h>
#include "Wavetable.h"


class Oscillator
{
public:

	enum eChannelIndex
	{
		LEFT_CHANNEL,
		RIGHT_CHANNEL,
	};

	enum eLoopModes
	{
		FORWARD, //Start -> End 
		REVERSE, //End -> Start -> End
		PING_PONG, //Start -> End -> Start
		ONE_SHOT, //Start -> End;

		ONE_SHOT_FORWARD, //Start -> End; Loop -> End;
		ONE_SHOT_PING_PONG, //Start -> End; Loop -> End, End -> Loop;
	};

	enum eLoopPhases
	{
		INC,
		DEC,
		ONESHOT_FINISHED,
	};

	enum eTrigger
	{
		TRIGGERED = 1,
	};

	enum eSmoothDirection
	{
		RISING,
		FALLING,
	};

	enum eLoopIndex
	{
		START,
		LOOP,
		END,
		NUM_LOOP_POINTS
	};

	Oscillator(Wavetable* wt);
	virtual ~Oscillator();

	
	void trigger(uint8_t velocity);

	uint32_t getLoopPoint(/*eLoopIndex*/ uint8_t index);
	void updateLoopPoints(uint32_t start, uint32_t loop, uint32_t end);

	//Updates Oscillator parameters with given wave
	void updateWavetable(Wavetable* wave);

	//Updates the Oscillator parameters with existing wavetable
	void updateWavetable(void);

	//Finds suitable zero crossings for both the endPoint and the LoopPoint
	uint8_t smoothLoopPoints(uint8_t /*eSmoothDirection*/ dir);

	//Returns the next sample to be played
	double getSample(uint8_t chIndex);

	//Returns the sample value at phase 'ph'
	double getSample(uint8_t chIndex, uint32_t ph);

	uint32_t getCurrentPhase(void) { return phase; }

	void updatePhase();

	uint32_t FindZeroCrossing(uint8_t dir, uint32_t start, uint32_t end);
	bool isAtZeroCrossing(uint32_t sampleIdx);
	
	Wavetable* getWavetable(void) { return wt;  }


protected:

	Wavetable* wt;
	

	uint8_t retrigFlag;
	uint8_t triggerHandled;

	int32_t phase;
	int32_t phaseIncrement;

	uint8_t loopMode; //eLoopModes
	uint8_t loopPhase; //eLoopPhases

	//Used for one shots
	int32_t LoopPoint[NUM_LOOP_POINTS];


};

