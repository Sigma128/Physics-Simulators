#ifndef SPRINGCLASS_H
#define SPRINGCLASS_H

#include "VectorClass.h"

class SpringClass {
private:
	float lengthChange = 0;
public:
	//Vector que representa la molla
	Vector spring = {0, -1};
	//Força que aplica la molla
	Vector force = {0, 0};

	float lengthInitial = 1;
	float k = 5;

	//Mòdul de la força
	//Es defineix com una variable individual
	//perquè s'usa vàries vegades, i així
	//s'estalvia processador
	float forceLength = 0;

	//Angle de la força
	//Es defineix com una variable individual
	//perquè s'usa vàries vegades, i així
	//s'estalvia processador
	float forceAngle = 0;

	//Constructor
	SpringClass(float x=0, float y=0) {
		spring.x = x;
		spring.y = y;
	}

	//Càlculs físics per actualitzar la molla
	void updateSpring(Vector redPosition, Vector ballPosition);
};

#endif