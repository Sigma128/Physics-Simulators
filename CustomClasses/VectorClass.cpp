#define _USE_MATH_DEFINES
#include <cmath>

#include "VectorClass.h"

//Angle del vector
//negativa a la convenció trigonomètrica.
//Retorna radians
//
//L'eix y de SDL2 apunta cap a baix
float Vector::angle() {
	//Evitar dividir entre 0
	if (x == 0) {
		//El vector apunta cap a dalt
		if (y > 0) {
			return M_PI_2;
		//El vector apunta cap a baix
		}
		else if (y < 0) {
			return M_PI_2*3;
		}
		//y == 0. El vector val 0
		//(segurament s'ha inicialitzat però no s'ha usat)
		else {
			return 0;
		}
	}
	//(90º, 270º)
	else if (x < 0) {
		return M_PI+std::atan(y/x);
	}
	//(-90º, 90º)
	else {
		return std::atan(y/x);
	}
	return 0;
}


//Codi per poder encadenar operacions

Vector operator +(Vector left, const Vector &right) {
	left += right;
	return left;
}

Vector operator -(Vector left, const Vector &right) {
	left -= right;
	return left;
}

Vector operator *(Vector left, const float &right) {
	left *= right;
	return left;
}

Vector operator /(Vector left, const float &right) {
	left /= right;
	return left;
}