#include <iostream>
#include <tuple>
#include <cmath>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Pendol_Elastic-Render.h"
#include "CustomClasses/VectorClass.h"


//Variables a inicialitzar, la finestra i les textures

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

//Array per organitzar les textures,
//aquest es correspon amb un enum a Game.cpp
SDL_Texture *textures[5];

SDL_Texture *texBackground = nullptr;
SDL_Texture *texArrow = nullptr;
SDL_Texture *texSpring = nullptr;
SDL_Texture *texCircleRed = nullptr;
SDL_Texture *texCircleBlue = nullptr;

enum textureIDs {
	TEX_BACKGROUND,
	TEX_ARROW,
	TEX_SPRING,
	TEX_CIRCLE_RED,
	TEX_CIRCLE_BLUE,
};

//Variables per transformar cordenades
int screenWidth, screenHight;
int windowWidth, windowHeight;
float scaleX, scaleY;

Vector getTextureSize(int textureID) {
	int texW;
	int texH;
	SDL_QueryTexture(textures[textureID], NULL, NULL, &texW, &texH);

	return {texW, texH};
}

Vector windowToScreen(Vector windowCoordinates) {
	Vector screenCoordinates;

	screenCoordinates.x = (windowCoordinates.x - (windowWidth - screenWidth*scaleX) /2 ) / scaleX;
	screenCoordinates.y = (windowCoordinates.y - (windowHeight - screenHight*scaleY) /2 ) / scaleY;

	return screenCoordinates;
}

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

	texBackground = IMG_LoadTexture(renderer, "Images/background.png");
	texArrow	  = IMG_LoadTexture(renderer, "Images/arrow.png");
	texSpring	  = IMG_LoadTexture(renderer, "Images/spring.png");
	texCircleRed  = IMG_LoadTexture(renderer, "Images/circle_red.png");
	texCircleBlue = IMG_LoadTexture(renderer, "Images/circle_blue.png");
	
	//Recordar actualitzar definició de array textures si s'afageixen de noves
	textures[0] = texBackground;
	textures[1] = texArrow;
	textures[2] = texSpring;
	textures[3] = texCircleRed;
	textures[4] = texCircleBlue;
}


//Cas especial per la imatge de fons
void renderBackground() {
	//Destination. On es vol render la imatge
	SDL_Rect dstBackground;
	SDL_QueryTexture(textures[TEX_BACKGROUND], NULL, NULL, &dstBackground.w, &dstBackground.h);
	dstBackground.x = 0;
	dstBackground.y = 0;

	//Escalar i centrar la imatge a la finestra, a resolució 16:9

	dstBackground.w *= scaleX;
	dstBackground.h *= scaleY;

	dstBackground.x = dstBackground.x*scaleX+(windowWidth-screenWidth*scaleX)/2;
	dstBackground.y = dstBackground.y*scaleY+(windowHeight-screenHight*scaleY)/2;

	//dstBackground.x = (windowWidth-dstBackground.w)/2;
	//dstBackground.y = (windowHeight-dstBackground.h)/2

	SDL_RenderCopy(renderer, textures[TEX_BACKGROUND], NULL, &dstBackground);


	//Color a gris
	//Color a vermell (per debug)
	SDL_SetRenderDrawColor(renderer, 205, 205, 205, 255);

	int centerX = windowWidth/2;
	int centerY = windowHeight/2;

	int pixelsPerMeterX = scaleX*100;
	int pixelsPerMeterY = scaleY*100;

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

//Netejar la pantalla, deixant de fons les coordenades cartesianes
void startRender() {
	SDL_RenderClear(renderer);

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

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
void renderTexture(int textureID, Vector position) {
	//Destination. On es vol render la imatge
	SDL_Rect dst;
	SDL_QueryTexture(textures[textureID], NULL, NULL, &dst.w, &dst.h);
	dst.x = position.x - dst.w/2;
	dst.y = position.y - dst.h/2;

	//Escalar la pantalla a la finestra

	dst.w *= scaleX;
	dst.h *= scaleY;

	dst.x = dst.x*scaleX+(windowWidth-screenWidth*scaleX)/2;
	dst.y = dst.y*scaleY+(windowHeight-screenHight*scaleY)/2;

	SDL_RenderCopy(renderer, textures[textureID], NULL, &dst);
}

//Renderitzar un vector com fletxa.
//Punt d'inici en coordenades pixel
//Escala com fracció de la imatge (resolució/100px)
//Opacity [0,255]
void renderVector(int textureID, Vector startPoint, Vector vector) {
	SDL_Rect dst;
	SDL_QueryTexture(textures[textureID], NULL, NULL, &dst.w, &dst.h);
	dst.w *= scaleX;
	dst.h *= scaleY*vector.module()/2;
	SDL_Point topCenter = {dst.w/2, 0};
	dst.x = startPoint.x - dst.w/2;
	dst.y = startPoint.y;
	dst.x = dst.x*scaleX+(windowWidth-screenWidth*scaleX)/2;
	dst.y = dst.y*scaleY+(windowHeight-screenHight*scaleY)/2;
	SDL_RenderCopyEx(renderer, textures[textureID], NULL, &dst, vector.angle()*180/M_PI-90, &topCenter, SDL_FLIP_NONE);
}

//Posar el fotograma a la pantalla
void endRender() {
	//Fer que el fons sigui blanc
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderPresent(renderer);
}

//Alliberar la memòria de les variables
void exitRender() {
	SDL_DestroyTexture(texBackground);
	SDL_DestroyTexture(texArrow);
	SDL_DestroyTexture(texSpring);
	SDL_DestroyTexture(texCircleRed);
	SDL_DestroyTexture(texCircleBlue);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
}