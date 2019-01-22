/*
* HackflightSimRangefinder.cpp: Class implementation for Rangefinder class in HackflightSim
*
* Copyright (C) 2018 Simon D. Levy
*
* MIT License
*/

#include "HackflightSimRangefinder.h"

HackflightSimRangefinder::HackflightSimRangefinder(APawn * pawn) : HackflightSimSensor(pawn)
{
}

void HackflightSimRangefinder::modifyState(state_t & state, float time)
{
	float altitude = getAltitude() - _groundAltitude;

	FVector euler = getEulerAngles();

	// Hypoteneuse = adjacent / cosine
	state.altitude = altitude / (cos(euler.X) * cos(euler.Y));
}

bool HackflightSimRangefinder::ready(float time)
{
	(void)time;

    if (!_ready) { // XXX should use an init() method
        _groundAltitude = getAltitude();
        _ready = true;
    }

	return true;
}

float HackflightSimRangefinder::getAltitude(void)
{
	return _pawn->GetActorLocation().Z / 100; // cm => m
}
