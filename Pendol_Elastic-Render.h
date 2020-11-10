#ifndef RENDER_H
#define RENDER_H

#include <tuple>

#include "CustomClasses/VectorClass.h"

Vector getScreenCenter();

Vector getTextureSize(int texID);
Vector windowToScreen(Vector coordinates);
Vector screenToWindow(Vector screenCoordinates);
Vector snapToGrid(Vector pos, int precision);
std::tuple<Vector, int, int> getCartesianArray(int resolution);

//Inicialitzar les variables
void initRender();

//Netejar la pantalla i deixar un fons blanc
void startRender();

//Render una particula com un cercle,
//vermell si positiu, blau si negatiu
void renderTexture(int textureID, Vector position);

//Renderitzar un vector com fletxa.
//Donar punt d'inici i vector
//Coordenades pixel
//Opacity [0,255]
void renderVector(int textureID, Vector startPoint, Vector vector);

//Posar el fotograma a la pantalla
void endRender();

//Alliberar la memòria de les variables
void exitRender();

#endif