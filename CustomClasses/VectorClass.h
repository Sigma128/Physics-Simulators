#ifndef VECTOR_CLASS_H
#define VECTOR_CLASS_H

#define _USE_MATH_DEFINES
#include <cmath>

//Aquesta classe es comporta com un vector físic,
//s'utilitza en els càlculs físics.
//
//Es poden fer operacions amb vectors
//utilitzant els operadors estàndard (+,-,*,/), i
//càlcul del mòdul i de l'angle
class Vector {
public:
	//Component horitzontal del vector
	float x;
	//Component vertical del vector
	float y;

	//Constructor predeterminat
	Vector() {
		x = 0;
		y = 0;
	}
	
	//Constructor per float
	Vector(float xInit, float yInit) {
		x = xInit;
		y = yInit;
	}

	//Constructor per int
	Vector(int xInit, int yInit) {
		x = xInit;
		y = yInit;
	}

	//Copy constructor, o signe "=" de tota la vida
	Vector(const Vector& vector) {
		x = vector.x;
		y = vector.y;
	}

	//Suma de vectors
	Vector& operator +=(const Vector& vector) {
		x += vector.x;
		y += vector.y;
		return *this;
	}

	//Resta de vectors
	Vector& operator -=(const Vector& vector) {
		x -= vector.x;
		y -= vector.y;
		return *this;
	}

	//Vector multiplicat per escalar
	Vector& operator *=(const float& scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	//Vector dividit entre escalar
	Vector& operator /=(const float& scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}

	//Mòdul del vector (pitàgores dels components)
	float module() { return std::sqrt(pow(x, 2) + pow(y, 2)); }
	
	//Angle del vector
	//segons la convenció trigonomètrica.
	//Retorna radians
	//
	//L'eix y de SDL2 apunta cap a baix
	float angle();
};

//Codi per poder encadenar operacions

Vector operator +(Vector left, const Vector &right);
Vector operator -(Vector left, const Vector &right);
Vector operator *(Vector left, const float &right);
Vector operator /(Vector left, const float &right);

#endif