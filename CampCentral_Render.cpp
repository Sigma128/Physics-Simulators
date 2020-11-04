#include <iostream>
#include <tuple>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "CampCentral_Render.h"
#include "CustomClasses/VectorClass.h"


//Variables a inicialitzar, la finestra i les textures

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *texArrow = nullptr;
SDL_Texture *texCircleRed = nullptr;
SDL_Texture *texCircleBlue = nullptr;
SDL_Texture *texBackground = nullptr;

//Array per organitzar les textures,
//aquest es correspon amb un enum a Game.cpp
SDL_Texture *textures[4];

//Variables per transformar cordenades
int screenWidth, screenHight;
int windowWidth, windowHeight;
float scaleX, scaleY;

Vector getTextureSize(int texID) {
	int texW;
	int texH;
	SDL_QueryTexture(textures[texID], NULL, NULL, &texW, &texH);

	return {texW, texH};
}

//Transformar sistema de coordenades de la finestra
//al de la pantalla
Vector windowToScreen(Vector windowCoordinates) {
	Vector screenCoordinates;

	screenCoordinates.x = (windowCoordinates.x - (windowWidth - screenWidth*scaleX) /2 ) / scaleX;
	screenCoordinates.y = (windowCoordinates.y - (windowHeight - screenHight*scaleY) /2 ) / scaleY;

	return screenCoordinates;
}

//Transformar sistema de coordenades de la pantalla
//al de la finestra
Vector screenToWindow(Vector screenCoordinates) {
	Vector windowCoordinates;

	windowCoordinates.x = screenCoordinates.x*scaleX + (windowWidth - screenWidth*scaleX) /2;
	windowCoordinates.y = screenCoordinates.y*scaleY + (windowHeight - screenHight*scaleY) /2;

	return windowCoordinates;
}

Vector getScreenCenter() { return {screenWidth/2, screenHight/2}; }

//Pixel coordinates
//Precision in pixels/unit
Vector snapToGrid(Vector pos, int precision) {
	Vector result;
	result.x = round( (pos.x-screenWidth/2) / precision )*precision + screenWidth/2;
	result.y = round( (pos.y-screenHight/2) / precision )*precision + screenHight/2;

	return result;
}

//Give resolution in pixels/meter
std::tuple<Vector, int, int> getCartesianArray(int resolution) {
	return std::make_tuple(
		snapToGrid( {0,0} , resolution ),
		ceil(screenWidth/resolution),
		ceil(screenHight/resolution)
	);
}


//Inicialitzar finestra i textures
void initRender() {

	//Detectar la resolució de la pantalla

	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_DisplayMode DM;
	if (SDL_GetDesktopDisplayMode(0, &DM) == 0) {
		screenWidth = DM.w;
		screenHight = DM.h;
	} else {
		SDL_Log("Could not get display mode for video display #%s", SDL_GetError());
		screenWidth = 1366;
		screenHight = 768;
	}

	window = SDL_CreateWindow(
		"Calculadora Camp Central",
		screenWidth/2,
		screenHight/4,
		screenWidth/2, screenHight/2,
		SDL_WINDOW_RESIZABLE
	);

	SDL_SetWindowMinimumSize(window, screenWidth/10, screenHight/10);

	renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	texArrow	  = IMG_LoadTexture(renderer, "Images/triangle_vector.png");
	texCircleRed  = IMG_LoadTexture(renderer, "Images/circle_red.png");
	texCircleBlue = IMG_LoadTexture(renderer, "Images/circle_blue.png");
	texBackground = IMG_LoadTexture(renderer, "Images/CartesianBackground.png");
	
	textures[0] = texArrow;
	textures[1] = texCircleRed;
	textures[2] = texCircleBlue;
	textures[3] = texBackground;
	//Actualitzar definició de textures[4] si s'afageixen noves textures
}


//Cas especial per la imatge de fons
void renderBackground() {
	//Destination. On es vol render la imatge
	SDL_Rect dstBackground;
	SDL_QueryTexture(textures[3], NULL, NULL, &dstBackground.w, &dstBackground.h);
	dstBackground.x = 0;
	dstBackground.y = 0;

	//Escalar i centrar la imatge a la finestra, a resolució 16:9

	dstBackground.w *= scaleX;
	dstBackground.h *= scaleY;

	dstBackground.x = dstBackground.x*scaleX+(windowWidth-screenWidth*scaleX)/2;
	dstBackground.y = dstBackground.y*scaleY+(windowHeight-screenHight*scaleY)/2;

	//dstBackground.x = (windowWidth-dstBackground.w)/2;
	//dstBackground.y = (windowHeight-dstBackground.h)/2

	SDL_RenderCopy(renderer, textures[3], NULL, &dstBackground);


	//Color a gris
	//Color a vermell (per debug)
	SDL_SetRenderDrawColor(renderer, 205, 205, 205, 255);

	int centerX = windowWidth/2;
	int centerY = windowHeight/2;

	int pixelsPerMeterX = scaleX*100; //Hauria de ser 100, però floats
	int pixelsPerMeterY = scaleY*100; //Hauria de ser 100, però floats

	//Eixos
	SDL_RenderDrawLine(renderer, centerX, 0, centerX, windowHeight);
	SDL_RenderDrawLine(renderer, 0, centerY, windowWidth, centerY);

	//Graella
	for (int i = 0; i < floor(centerX/pixelsPerMeterX); i++) {
		SDL_RenderDrawLine(renderer, centerX+pixelsPerMeterX*i, 0, centerX+pixelsPerMeterX*i, windowHeight);
		SDL_RenderDrawLine(renderer, centerX-pixelsPerMeterX*i, 0, centerX-pixelsPerMeterX*i, windowHeight);
	}

	for (int i = 0; i < floor(centerY/pixelsPerMeterY); i++) {
		SDL_RenderDrawLine(renderer, 0, centerY+pixelsPerMeterY*i, windowWidth, centerY+pixelsPerMeterY*i);
		SDL_RenderDrawLine(renderer, 0, centerY-pixelsPerMeterY*i, windowWidth, centerY-pixelsPerMeterY*i);
	}
}

//Netejar la pantalla, deixant un fons blanc
void startRender() {
	SDL_RenderClear(renderer);

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);


	//Quan la pantalla és justament 16:9 no s'ajusta bé per la precisió

	float ratio = screenWidth*1.0f/screenHight;

	//Pantalla ample
	if (windowWidth/windowHeight >= ratio) {
		scaleX = windowHeight*ratio/screenWidth;
		scaleY = windowHeight*1.0f/screenHight;
		//1.0f perquè no arrodoneixi
		//TODO: Provar a treure 1.0f
	
	//Pantalla alta
	} else {
		scaleX = windowWidth*1.0f/screenWidth;
		scaleY = windowWidth/(screenHight*ratio);
		//1.0f perquè no arrodoneixi
		//TODO: Provar a treure 1.0f
	}

	//Render imatge de fons
	renderBackground();
}

//Renderitzar la textura especificada
//en la posició especificada
//centrada
void renderParticle(Vector position, float charge) {
	//Destination. On es vol render la imatge
	SDL_Rect dst;

	if (charge >= 0) {
		//TEX_RED_RING
		SDL_QueryTexture(textures[1], NULL, NULL, &dst.w, &dst.h);
	} else {
		//TEX_BLUE_RING
		SDL_QueryTexture(textures[2], NULL, NULL, &dst.w, &dst.h);
	}

	dst.w *= sqrt(abs(charge))/4;
	dst.h *= sqrt(abs(charge))/4;

	dst.x = position.x - dst.w/2;
	dst.y = position.y - dst.h/2;

	//Escalar la pantalla a la finestra

	dst.w *= scaleX;
	dst.h *= scaleY;

	dst.x = dst.x*scaleX+(windowWidth-screenWidth*scaleX)/2;
	dst.y = dst.y*scaleY+(windowHeight-screenHight*scaleY)/2;

	if (charge >= 0) {
		//TEX_RED_RING
		SDL_RenderCopy(renderer, textures[1], NULL, &dst);
	} else {
		//TEX_BLUE_RING
		SDL_RenderCopy(renderer, textures[2], NULL, &dst);
	}
}

//Renderitzar un vector com fletxa.
//Punt d'inici en coordenades pixel
//Escala com fracció de la imatge (resolució/100px)
//Opacity [0,255]
void renderVector(Vector startPoint, Vector vector, float vectorScale, float opacity) {
	SDL_Rect dst;
	SDL_QueryTexture(textures[0], NULL, NULL, &dst.w, &dst.h);
	dst.w *= scaleX*vectorScale;
	dst.h *= scaleY*vectorScale;
	SDL_Point topCenter = {dst.w/2, 0};
	dst.x = startPoint.x - dst.w/2;
	dst.y = startPoint.y;
	SDL_SetTextureAlphaMod(textures[0], opacity);
	SDL_RenderCopyEx(renderer, textures[0], NULL, &dst, -vector.renderAngle(), &topCenter, SDL_FLIP_NONE);
}

//Renderitzar un cuadrat del potencial electric
//Gradient de blau(0) a vermell(255)
//Donar punt d'inici i color
//Coordenades pixel
//Opacity [0,255]
void renderPotential(Vector startPoint, int pixelsPerMeter, float potentialOpacity) {
	SDL_Rect destination;
	destination.w = pixelsPerMeter*scaleX;
	destination.h = pixelsPerMeter*scaleY;
	destination.x = startPoint.x - destination.w/2;
	destination.y = startPoint.y - destination.h/2;

	int red		= potentialOpacity <= 127.5 ?						 0 : 2*potentialOpacity-255;
	int green	= potentialOpacity <= 127.5 ?		2*potentialOpacity : 510-2*potentialOpacity;
	int blue	= potentialOpacity <= 127.5 ?	255-2*potentialOpacity : 0;

	SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
	SDL_RenderFillRect(renderer, &destination);
}

//Posar el fotograma a la pantalla
void endRender() {
	//Fer que el fons sigui blanc
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderPresent(renderer);
}

//Alliberar la memòria de les variables
void exitRender() {	
	SDL_DestroyTexture(texArrow);
	SDL_DestroyTexture(texCircleRed);
	SDL_DestroyTexture(texCircleBlue);
	SDL_DestroyTexture(texBackground);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
}