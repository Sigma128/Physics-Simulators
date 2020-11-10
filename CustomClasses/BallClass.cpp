#include <cmath>

#include "BallClass.h"
#include "VectorClass.h"

//Càlculs físics per actualitzar la posició
void BallClass::updateBall(Vector springForce) {

	positionLast = position;
	position += velocityLast*deltaTime;

	velocityLast = velocity;
	velocity += ( (springForce-velocity*friction)/mass + gravity )*deltaTime;
};