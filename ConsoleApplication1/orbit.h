#pragma once
#include <math.h>

class COrbit {
public:
	float theta;
	float phi;
	float radius;
	COrbit() { theta = 0.0; phi = 0.0; radius = 7.0; }
	float X() { return radius * sin(phi)*cos(theta); }
	float Y() { return radius * sin(phi)*sin(theta); }
	float Z() { return radius * cos(theta); }
	
};