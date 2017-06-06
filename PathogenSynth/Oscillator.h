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

		NUM_LOOP_MODES,
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

	enum
	{
		RESAMPLE_FACTOR = 256,
	};

	Oscillator(Wavetable* wt);
	virtual ~Oscillator();

	
	void trigger(double pitch, uint8_t velocity);

	void updatePhaseInc(void);

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

	uint32_t getCurrentPhase(void) { return (uint32_t)phase; }

	void updatePhase();

	uint32_t FindZeroCrossing(uint8_t dir, uint32_t start, uint32_t end);
	bool isAtZeroCrossing(uint32_t sampleIdx);
	
	Wavetable* getWavetable(void) { return wt;  }

	void setCoarseTune(int32_t t) { coarseTune = t; }
	void setFineTune(int32_t t) { fineTune = t;  }
	void setLoopMode(uint32_t m) { loopMode = m; }

protected:

	//Given a midi note, calculate the new phase inc value
	double getPhaseInc(double pitch);

	void resetInterpolator(float ph, float phInc);
	double getInterpolatedSample(uint8_t ch);
	void updateInterpolation(float lastPhase, float newPhase);


	uint32_t masterTune;

	int32_t midiNote;
	int32_t fineTune;
	int32_t coarseTune;

	Wavetable* wt;
	
	uint8_t retrigFlag;
	uint8_t triggerHandled;

	float phase;
	float phaseIncrement;

	float interpInc;


	int32_t upsampleFactor;
	int32_t downsampleFactor;

	int32_t errorAcc;

	double lerpAccLeft;
	double lerpAccRight;
	uint32_t lerpDivisor;

	uint8_t loopMode; //eLoopModes
	uint8_t loopPhase; //eLoopPhases

	//Used for one shots
	int32_t LoopPoint[NUM_LOOP_POINTS];


};

