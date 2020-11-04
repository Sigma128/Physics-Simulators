#include <iostream>
//Per std::setprecision()
#include <iomanip>

#include "CampCentral_Physics.h"

#include "CustomClasses/VectorClass.h"


//Constant de la força elèctrica
const long long K = 9000000000;

//Conversió de píxels a metres: 100px = 1m
const int pixelsPerMetre = 100;

int getPixelsPerMeter() { return pixelsPerMetre; };

//Calcular el camp elèctric que una particula fa en un punt en concret
//Donar coordenades en pixel
Vector fieldBetween(ElectricParticle particle, Vector point) {

	if (particle.charge == 0) { return {0, 0}; }

	Vector r = (point-particle.position)/pixelsPerMetre;
	//Evitar divisió entre 0
	if (r.module() == 0) { return {0, 0}; }

	//Només accepta aquesta sintaxi
	return r*K*particle.charge/(r.module()*r.module()*r.module());
}

//Calcular la potència elèctrica que una particula fa en un punt en concret
//Donar coordenades en pixel
float potentialBetween(ElectricParticle particle, Vector point) {

	if (particle.charge == 0) { return 0; }

	Vector r = (point-particle.position)/pixelsPerMetre;
	//Evitar divisió entre 0
	if (r.module() == 0) { return 0; }

	//Només accepta aquesta sintaxi
	return K*particle.charge/(r.module());
}


//Origen de coordenades cartesià,
//en coordenades pixel de SDL2.
//Per passar de pixels a cartesià i viceversa
Vector origin;

void setOrigin(Vector screenCenter) {
	origin = screenCenter;
}

//Passar pixels (coordenades de SDL2) a sistema catesià
//Inversa de cartesianToPixels()
Vector pixelsToCartesian(Vector pixelPosition) {
	Vector cartesianPosition;

	cartesianPosition = (pixelPosition-origin)/pixelsPerMetre;

	//L'eix y de SDL2 apunta cap abaix
	cartesianPosition.y *= -1;

	return cartesianPosition;
}

//Passar sistema cartesià a pixels (coordenades de SDL2).
//Inversa de pixelsToCartesian()
Vector cartesianToPixels(Vector cartesianPosition) {
	Vector pixelPosition;

	//L'eix y de SDL2 apunta cap abaix
	cartesianPosition.y *= -1;

	pixelPosition = cartesianPosition*pixelsPerMetre+origin;

	return pixelPosition;
}


//Array que conté les 5 partícules
//usades en aquest programa: q, Q1, Q2, Q3 i Q4.
//particles[0] és q, la partícula central
//
//Aquest array funciona conjuntament
//amb particleNames[],
//per donar'ls-hi nom a l'hora de std::cout
//
//S'inicialitza amb la posició (x, y, en coordenades cartesianes)
//i càrrega (en Coulombs).
//La escala és 100 pixels/metre per defecte
ElectricParticle particles[5];

void initPhysics() {
	particles[0] = {  cartesianToPixels({ 0,  0}) , 1  };
	particles[1] = {  cartesianToPixels({ 2,  0}) , 2  };
	particles[2] = {  cartesianToPixels({-2,  0}) , 3  };
	particles[3] = {  cartesianToPixels({ 0,  2}) , 4  };
	particles[4] = {  cartesianToPixels({ 0, -2}) , 5  };
}

ElectricParticle* getParticles() { return particles; };
int getParticlesSize() { return sizeof(particles)/sizeof(*particles); };


bool isParticleOnClick(int particleID, Vector mousePos, Vector hitbox) {
	Vector particlePos = particles[particleID].position;

	return (abs(particlePos.x-mousePos.x) < hitbox.x
			and abs(particlePos.y-mousePos.y) < hitbox.y
	);
}


//L'usuari modifica la posició d'una partícula
//Posició cartesiana
void modifyParticlePos(int particleID, Vector newPosition) {
	particles[particleID].position = cartesianToPixels(newPosition);
};

//L'usuari modifica la càrrega d'una partícula
void modifyParticleCharge(int particleID, float newCharge) {
	particles[particleID].charge = newCharge;
};


//Nom de cada partícula
const std::string particleNames[] {"q", "Q1", "Q2", "Q3", "Q4"};

//Tots els valors calculats d'una partícula
void coutParticleData(int particleID) {
	using namespace std;

	cout << "Particula: " << particleNames[particleID] << '\n';

	//Posició de cada partícula
	cout << "\t-Posicio (r): "
		<< "x: " << pixelsToCartesian(particles[particleID].position).x << "  "
		<< "y: " << pixelsToCartesian(particles[particleID].position).y << "  m\n";


	//Usar notació científica amb 2 decimals
	//a partir d'aquí
	int originalPrecision = cout.precision();
	cout << setprecision(2) << scientific;
	

	//Càrrega de cada partícula
	cout << "\t-Carrega (Q): "
		<< particles[particleID].charge << "C\n";


	//Camp i potencial de les partícules Q1 Q2 Q3 Q4,
	//(totes excepte q)
	if (particleID != 0) {
		Vector field = fieldBetween(particles[particleID], particles[0].position);
		float potential = potentialBetween(particles[particleID], particles[0].position);


		//Camp elèctric
		cout << "\t-Camp electric (E): \n";
		cout << "\t\t-Forma cartesiana:\t"
			<< field.x << "i + "
			<< field.y << "j  N/C\n";
		cout << "\t\t-Forma polar:\t\t"
			<< "modul: " << field.module() << " N/C  "
			//Només l'angle és fixed
			//Després es retorna a scientific
			<< fixed
			<< "angle: " << field.angle() //Radians a graus
			<< scientific << " graus\n";

		//Força elèctrica
		Vector force = field*particles[0].charge;
		cout << "\t-Forca (F): \n";
		cout << "\t\t-Forma cartesiana:\t"
			<< force.x << "i + "
			<< force.y << "j  N\n";
		cout << "\t\t-Forma polar:\t\t"
			<< "modul: " << force.module() << " N    "
			//Només l'angle és fixed
			//Després es retorna a scientific
			<< fixed
			<< "angle: " << force.angle() //Radians a graus
			<< scientific << " graus\n";

		//Potencial elèctric
		cout << "\t-Potencial (V):\t"
			<< potential << " V\n";

		//Energia elèctrica
		cout << "\t-Energia (U):\t"
			<< potential*particles[0].charge << " J\n";
	}

	//Retornar precisió al predeterminat
	cout << setprecision(originalPrecision) << defaultfloat << '\n';
}

//Tots els valors calculats de totes les partícules
void coutAllParticleData() {
	using namespace std;

	//Anar sumant el camp i el potencial cada loop
	//per calcular el total

	Vector totalField;
	float totalPotential;

	//Valors de cada partícula individual
	for (int i = 0; i < sizeof(particles)/sizeof(*particles); i++) {
		coutParticleData(i);

		if (i != 0) {
			totalField += fieldBetween(particles[i], particles[0].position);
			totalPotential += potentialBetween(particles[i], particles[0].position);
		}
	}

	//Usar notació científica amb 2 decimals
	//a partir d'aquí
	int originalPrecision = cout.precision();
	cout << setprecision(2) << scientific;


	cout << "\nTotal:\n";

	//Camp elèctric total
	cout << "Camp electric (E):\n";
	cout << "\t-Forma cartesiana:\t"
		<< totalField.x << "i + "
		<< totalField.y << "j  N/C\n";
	cout << "\t-Forma polar:\t\t"
		<< "modul: " << totalField.module() << " N/C  "
		//Només l'angle és fixed
		//Després es retorna a scientific
		<< fixed
		<< "angle: " << totalField.angle() //Radians a graus
		<< scientific << " graus\n";
	
	//Força elèctrica total
	Vector totalForce = totalField*particles[0].charge;
		cout << "Forca (F):\n";
		cout << "\t-Forma cartesiana:\t"
			<< totalForce.x << "i + "
			<< totalForce.y << "j  N\n";
		cout << "\t-Forma polar:\t\t"
			<< "modul: " << totalForce.module() << " N    "
			//Només l'angle és fixed
			//Després es retorna a scientific
			<< fixed
			<< "angle: " << totalForce.angle() //Radians a graus
			<< scientific << " graus\n";
	
	//Potencial elèctric total
	cout << "Potencial (V):\t" << totalPotential << " V\n";

	//Energia elèctrica total
	cout << "Energia (U):\t" << totalPotential*particles[0].charge << " J\n";


	//Retornar precisió al predeterminat
	cout << setprecision(originalPrecision) << defaultfloat << "\n\n";
}