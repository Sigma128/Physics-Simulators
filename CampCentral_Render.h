#ifndef RENDER_H
#define RENDER_H

#include "CustomClasses/VectorClass.h"

Vector getScreenCenter();

Vector getTextureSize(int texID);
//Transformar sistema de coordenades de la finestra
//al de la pantalla
Vector windowToScreen(Vector coordinates);
//Transformar sistema de coordenades de la pantalla
//al de la finestra
Vector screenToWindow(Vector screenCoordinates);
Vector snapToGrid(Vector pos, int precision);
std::tuple<Vector, int, int> getCartesianArray(int resolution);

//Inicialitzar les variables
void initRender();

//Netejar la pantalla i deixar un fons blanc
void startRender();

//Render una particula com un cercle,
//vermell si positiu, blau si negatiu
void renderParticle(Vector position, float charge);

//Renderitzar un vector com fletxa.
//Donar punt d'inici i vector
//Coordenades pixel
//Opacity [0,255]
void renderVector(Vector startPoint, Vector vector, float vectorScale, float opacity);

//Renderitzar un cuadrat del potencial electric.
//Donar punt d'inici i color
//Coordenades pixel
//Opacity [0,255]
void renderPotential(Vector startPoint, int pixelsPerMeter, float potentialOpacity);

//Posar el fotograma a la pantalla
void endRender();

//Alliberar la memòria de les variables
void exitRender();

#endif