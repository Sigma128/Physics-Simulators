#ifndef BALLCLASS_H
#define BALLCLASS_H

#include "VectorClass.h"

//Classe que representa la massa
//al final de la molla, que s'anomena "bola"
//per evitar confusió
//amb la massa física (la del pes)
//
//No es considera la força de la gravetat, i
//la fricció es calcula com si estigués dintre
//d'un fluid
class BallClass {
public:
	//Posició actual de la bola
	Vector position = {7, 5};

	float mass = 0.5;

	//Coeficient de fricció amb l'aire
	float friction = 1;

	float deltaTime = 0.02;

	Vector gravity = {0.0f, 9.8f};	//SDL té l'eix y al revés


	Vector positionLast = {7, 5};
	Vector velocity = {0, 0};
	Vector velocityLast = {0, 0};

	//Constructor
	BallClass(float x=0, float y=0) {
		position.x = x;
		position.y = y;
	}


	//Càlculs físics per actualitzar la posició
	void updateBall(Vector springForce);
};

#endif