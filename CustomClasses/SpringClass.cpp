#define _USE_MATH_DEFINES
#include <cmath>

#include "SpringClass.h"
#include "VectorClass.h"

//Càlculs físics per actualitzar la molla
void SpringClass::updateSpring(Vector cursorPosition, Vector ballPosition) {
	spring = ballPosition-cursorPosition;
	forceLength = spring.module();
	forceAngle = spring.angle();

	lengthChange = forceLength-lengthInitial;
	force = {
		-k*std::cos(forceAngle)*lengthChange,
		-k*std::sin(forceAngle)*lengthChange
	};
};