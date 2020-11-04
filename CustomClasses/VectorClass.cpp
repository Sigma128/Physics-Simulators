#define _USE_MATH_DEFINES
#include <cmath>

#include "VectorClass.h"

//Angle del vector
//segons la convenció trigonomètrica.
//Retorna graus
//
//L'eix y de SDL2 apunta cap a baix,
//per això es tracta la y com si fós negativa
float Vector::angle() {
	//Evitar dividir entre 0
	if (x == 0) {
		//El vector val 0
		//(segurament s'ha inicialitzat però no s'ha usat)
		if (y == 0) {
			return 0;
		//El vector apunta cap a dalt
		} else if (y < 0) {
			return 90;
		//El vector apunta cap a baix
		} else if (y > 0) {
			return 270;
		}
	//(90º, 270º)
	} else if (x < 0) {
		return 180+std::atan(-y/x)*180/M_PI;
	//(-90º, 90º)
	} else {
		return std::atan(-y/x)*180/M_PI;
	}

	return 0;
}

//Angle del vector
//usat per renderitzar.
//Retorna graus
//
//L'eix y apunta cap a baix,
//l'angle 0 és a sota i
//va en sentit horari;
//bastant diferent a la convenció en trigonometria.
//Per això, es rota el sistema de coordenades
//-90º (x/y en comptes de y/x), 
//i es canvia de sentit (angle = -angle)
//en el moment de renderitzar (a Game_Render.cpp)
float Vector::renderAngle() {
	//Evitar dividir entre 0
	if (y == 0) {
		//El vector val 0
		//(segurament s'ha inicialitzat però no s'ha usat)
		if (x == 0) {
			return 0;
		//El vector apunta cap a la dreta
		} else if (x > 0) {
			return 90;
		//El vector apunta cap a l'esquerra
		} else if (x < 0) {
			return 270;
		}
	//" (90º, 270º) "
	} else if (y < 0) {
		return 180+std::atan(x/y)*180/M_PI;
	//" (-90º, 90º) "
	} else {
		return std::atan(x/y)*180/M_PI;
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