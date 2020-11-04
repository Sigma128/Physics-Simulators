#ifndef PHYSICS_H
#define PHYSICS_H

#include "CustomClasses/VectorClass.h"

void setOrigin(Vector screenCenter);
void initPhysics();

int getPixelsPerMeter();

//Objecte que representa una partícula.
//Conté la seva posició i la seva càrrega elèctrica
struct ElectricParticle {
	Vector position;
	float charge;
};
//Calcular el camp elèctric que una particula fa en un punt en concret
//Donar coordenades en pixel
Vector fieldBetween(ElectricParticle particle, Vector point);
//Calcular la potència elèctrica que una particula fa en un punt en concret
//Donar coordenades en pixel
float potentialBetween(ElectricParticle particle, Vector point);

Vector pixelsToCartesian(Vector pixelPosition);
Vector cartesianToPixels(Vector cartesianPosition);

bool isParticleOnClick(int particleID, Vector mousePos, Vector hitbox);

//L'usuari modifica la posició d'una partícula
void modifyParticlePos(int particleID, Vector newPosition);
//L'usuari modifica la càrrega d'una partícula
void modifyParticleCharge(int particleID, float newCharge);

//Tots els valors calculats d'una partícula
void coutParticleData(int particleID);

//Tots els valors calculats de totes les partícules
void coutAllParticleData();

ElectricParticle* getParticles();
int getParticlesSize();

#endif