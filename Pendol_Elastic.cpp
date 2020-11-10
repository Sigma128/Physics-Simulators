#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <regex>
//Per donar color a la consola
#include <windows.h>
#include <chrono>

#include "SDL2/SDL.h"

#include "Pendol_Elastic-Render.h"

#include "CustomClasses/VectorClass.h"
#include "CustomClasses/SpringClass.h"
#include "CustomClasses/BallClass.h"


bool doConsoleColor = false;
bool doGreenDefault = false;

enum ConsoleColors {
	CONCOL_BLACK,
	CONCOL_BLUE,
	CONCOL_GREEN,
	CONCOL_CYAN,
	CONCOL_RED,
	CONCOL_MAGENTA,
	CONCOL_YELLOW,
	CONCOL_WHITE,
	CONCOL_LIGHT_BLACK,
	CONCOL_LIGHT_BLUE,
	CONCOL_LIGHT_GREEN,
	CONCOL_LIGHT_CYAN,
	CONCOL_LIGHT_RED,
	CONCOL_LIGHT_MAGENTA,
	CONCOL_LIGHT_YELLOW,
	CONCOL_LIGHT_WHITE
};

void setConsoleColor(int foreground=8, int background=0) {
	//Colors disponibles:
	//0  Negre
	//1  Blau
	//2  Verd
	//3  Cyan
	//4  Vermell
	//5  Magenta
	//6  Groc
	//7  Blanc
	//8  Negre	(Claret)
	//9  Blau	(Claret)
	//10 Verd	(Claret)
	//11 Cyan	(Claret)
	//12 Vermell (Claret)
	//13 Magenta (Claret)
	//14 Groc	(Claret)
	//15 Blanc   (Claret)

	//El color de la consola és determinat per un número,
	//que es calcula:
	//colorFront + colorFons*16

	if (doConsoleColor)	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		if (doGreenDefault and foreground == 8) {
			SetConsoleTextAttribute(hConsole, 2+background*16);
		} else if (doGreenDefault and foreground == 7) {
			SetConsoleTextAttribute(hConsole, 10+background*16);
		} else {
			SetConsoleTextAttribute(hConsole, foreground+background*16);
		}
	}
}


//Actualitzar la posició del cursor
//coordenades pixel
//
//Aquesta funció és un intermediari necessari ja que
//SDL_GetMouseState() retorna int,
//mentre que Vector és float
Vector getCursorPosition() {
	int x;
	int y;
	SDL_GetMouseState(&x, &y);

	return windowToScreen( {x, y} );
}


//Detectar el nombre n (0-2) dintre d'un std::string
//Serveix per positius, negatius i decimals.
//Els nombres negatius s'han d'esciure amb guió.
//Els nombres decimals s'han d'esciure amb punt. (no coma,)
float findNthNumberInString(std::string input, int nthNumber) {
	using namespace std;

	//No modificar el regex. Ni tans sols afegir espais
	const string regexStrings[3] = {
		"[^\\d]*?(-?\\d+(\\.\\d+)?).*",
	/*   [^\\d]*						Ignorar caracters alfabètics (a-z, que no són nombres 0-9)
			  (-?\\d+(\\.\\d+)?)		Detectar primer nombre
			   -?							-Buscar signe negatiu
				 \\d+						-Detectar Nombre
					 (\\.\\d+)?			-Buscar decimals
						\\.					-Punt decimal
						   \\d+				-Nombre decimal
								  .*	Ignorar la resta del string*/

		"[^\\d]*\\d+(\\.\\d+)?[^\\d]+?(-?\\d+(\\.\\d+)?).*",
	/*   [^\\d]*											Ignorar caracters alfabètics (1)
			  \\d+(\\.\\d+)?								Ignorar primer nombre
				  (\\.\\d+)?									-Ignorar decimals, si hi ha
							[^\\d]+?						Ignorar caracters alfabètics (2)
									(-?\\d+(\\.\\d+)?)		Detectar segon nombre
													  .*	Ignorar la resta del string*/

		"[^\\d]*\\d+[^\\d]+\\d+(\\.\\d+)?[^\\d]+?(-?\\d+(\\.\\d+)?).*"
	/*   [^\\d]*														Ignorar caracters alfabètics (1)
				\\d+													Ignorar primer nombre
					[^\\d]+												Ignorar caracters alfabètics (2)
						   \\d+(\\.\\d+)?								Ignorar segon nombre
							   (\\.\\d+)?									-Ignorar decimals, si hi ha
										 [^\\d]+?						Ignorar caracters alfabètics (3)
												 (-?\\d+(\\.\\d+)?)		Detectar tercer nombre
																   .*	Ignorar la resta del string*/
	};


	//Detectar si existeix el nombre n
	//dintre de l'input
	if (
		regex_search(
			input,
			regex(regexStrings[nthNumber])
			)
		) {

		//Si existeix,
		//i n==1 (s'ha demanat el primer nombre),
		//retornar aquest nombre
		if (nthNumber == 0) {
			string output = regex_replace(
				input,
				regex(regexStrings[nthNumber]),
				"$1"
			);

			//Passar output de string a float
			return stof(output);
		}

		//Si es troba,
		//i n!=0 (s'ha demanat el segon o tercer nombre);
		//retornar el segon capture group,
		//ja que el primer detecta si el segon nombre és decimal.
		//(Tecnicismes de regex)
		else {
			string output = regex_replace(
				input,
				regex(regexStrings[nthNumber]),
				"$2"
			);

			//Passar output de string a float
			return stof(output);
		}
	}
	
	//No es reconeix el nombre
	setConsoleColor(CONCOL_LIGHT_RED);
	std::cout << "ERROR: nombre " << nthNumber+1 << " no reconegut\n";
	setConsoleColor();
	return 0;
}


void coutHelpConsole() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Ordres:					Descripcio:
			"ajuda"			Llistar totes les ordres

			"color"			Donar color a la consola
			"descolor"		Desabilitar el color de la consola

			"valors"	Llistar totes les propietats

			"modificar"		Modificar propietats
			"dibuixar"		Passar a la finestra grafica
	*/
	std::cout << "Ordres:\t\t\tDescripcio:\n"
		<< "\t\"ajuda\"\t\tLlistar totes les ordres\n"
		<< '\n'
		<< "\t\"color\"\tDonar color a la consola\n"
		<< "\t\"descolor\"\tDesabilitar el color de la consola\n"
		<< '\n'
		<< "\t\"propietats\"\tLlistar totes les propietats\n"
		<< '\n'
		<< "\t\"modificar\"\tModificar propietats\n"
		<< "\t\"play\"\tPassar a la finestra grafica\n"
		<< '\n';
}

void coutHelpModify() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Ordres:
			"ajuda"			Llista les ordres del mode modificar
			
			"k"				Modificar constant elastica

			"lon"			Modificar longitud inicial de la molla

			"extrem"		Modificar vector de l'extrem fix de la molla

			"massa"			Modificar massa
			
			"b"				Modificar coeficient de friccio

			"pos"			Modificar vector de la posicio inicial de la bola

			"gravetat"		Modificar gravetat
			
			"temps"			Modificar delta temps
			
			"mollalliure"	Activar o desactivar mode "molla lliure"
			
			"fi"			Sortir del mode modificar
	
	*/
	std::cout << "Ordres:\n"
		<<"\t\"ajuda\"\t\tLlista les ordres del mode modificar\n"
		<<'\n'
		<<"\t\"k\"\t\tModificar constant elastica\n"
		<<'\n'
		<<"\t\"lon\"\t\tModificar longitud inicial de la molla\n"
		<<'\n'
		<<"\t\"extrem\"\t\tModificar vector de l'extrem fix de la molla\n"
		<<'\n'
		<<"\t\"massa\"\t\tModificar massa de la bola\n"
		<<'\n'
		<<"\t\"b\"\t\tModificar coeficient de friccio\n"
		<<'\n'
		<<"\t\"pos\"\t\tModificar vector de la posicio inicial de la bola\n"
		<<'\n'
		<<"\t\"gravetat\"\tModificar gravetat\n"
		<<'\n'
		<<"\t\"temps\"\t\tModificar delta temps, en segons\n"
		<<"\t\t\t\tEs recomana major a 0.02\n"
		<<"\t\"mollalliure\"\tModificar si la molla te un extrem fix o no\n"
		<<'\n'
		<<"\t\"fi\"\t\tSortir del mode modificar\n"
		<<'\n';
}

void coutHelpRender() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Accions disponibles:
			Arrosega els cercles per canviar la posicio

			Fes servir la roda del ratoli sobre els cercles per canviar la carrega

			Nomes es poden canviar els valors en unitats senceres
			Si vols valors decimals, fes servir la consola

			Prem P per tornar a la consola
	
	*/
	std::cout << "Accions disponibles:\n"
		<<'\n'
		<<"\tPrem R per retornar la bola a la posicio inicial\n"
		<<'\n'
		<<"\tPrem barra espaiadora per tornar a la consola\n"
		<<'\n';
}



int main(int argc, char *argv[]) {
	//Aquestes tres funcions han d'anar en aquest ordre
	//Inicialitzar les variables a Game_Render.cpp
	initRender();

	//Variables que s'han de conservar entre fotogrames

	bool running = true;
	bool doConsole = true;
	bool modifying = false;

	bool isMouseDown = false;
	bool wasMouseDownLastFrame = false;
	int lastParticleOnClick = -1;

	bool doFreeSpring = false;

	int pxPerMeter = 100;

	Vector springFixedPoint = {7, 4};
	Vector ballInitPoint = {3, 4};

	//Vector que representa la molla
	SpringClass spring = {0, -1};

	//Massa al final de la molla,
	//que s'anomena "bola" per evitar confusió
	//amb la magnitud física
	BallClass ball = {3, 4};


	coutHelpConsole();

	//Main loop
	SDL_Event event;
	while (running)	{
		//Codi per iniciar un cronòmetre.
		//Es fa servir més tard per regular els fotogrames per segon
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		//Variables que es restableixen cada fotograma

		int mouseWheel = 0;
		Vector cursorPosition = {7, 4};

		//Registre de l'input
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_MOUSEBUTTONUP and event.button.button == SDL_BUTTON_LEFT) {
				//El jugador deixa de fer click esquerre:
				isMouseDown = false;
				wasMouseDownLastFrame = true;
			}
			else {
				wasMouseDownLastFrame = false;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN and event.button.button == SDL_BUTTON_LEFT) {
				//L'usuari fa click esquerre:
				isMouseDown = true;
			}


			else if (event.type == SDL_MOUSEWHEEL) {
				//Scroll up
				if (event.wheel.y > 0) {
					mouseWheel = 1;
				}
				//Scroll down
				else {
					mouseWheel = -1;
				}
			}

			//Detectar si el jugador ha près alguna tecla
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
					//Es prem la tecla "R":
					//Restablèixer variables
					case SDL_SCANCODE_R:
						ball.position = ballInitPoint;
						ball.positionLast = ballInitPoint;
						ball.velocity = {0, 0};
						ball.velocityLast = {0, 0};
						break;
					
					//Es prem la tecla "P":
					//Pausar, és a dir, deixar de renderitzar i
					//passar a la consola
					case SDL_SCANCODE_SPACE:
					case SDL_SCANCODE_P:
					case SDL_SCANCODE_ESCAPE:
						doConsole = true;
						break;

					default:
						break;
				}
			}

			//El jugador tanca la finestra amb
			//la "x" a la part superior dreta:
			//Tancar el joc
			else if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		//L'usuari interactua amb el programa a través de la consola
		if (doConsole and !modifying) {
			using namespace std;

			setConsoleColor();			
			string command{};
			cout << "Esperant ordre:\n";
			setConsoleColor(CONCOL_WHITE);
			getline(cin, command);
			setConsoleColor();
			cout << '\n';

			//L'usuari vol saber quines ordres existeixen
			if (command == "ajuda") {
				coutHelpConsole();
			}
			//Una broma
			else if (command == "hack") {
				doGreenDefault = true;
				setConsoleColor();
			}
			//Una broma
			else if (command == "unhack") {
				doGreenDefault = false;
				setConsoleColor();
			}
			else if (command == "color") {
				doConsoleColor = true;
			}
			else if (command == "descolor") {
				doConsoleColor = false;
				setConsoleColor(CONCOL_WHITE);
			}
			else if (command == "propietats") {
				//L'usuari vol saber les propietats de la molla i la bola
				setConsoleColor(CONCOL_CYAN);

				std::cout << "Extrem fix de la molla: "
					<< -7+springFixedPoint.x << " " << -4+springFixedPoint.y << " m\n"
					<< "Constant elastica (k): " << spring.k << '\n'
					<< "Longitud inicial de la molla: " << spring.lengthInitial << " m\n"
					<< '\n'
					<< "Posicio inicial de la bola: "
					<< -7+ballInitPoint.x << " " << -4+ballInitPoint.y << " m\n"
					<< "Massa de la bola: " << ball.mass << " kg\n"
					<< "Coeficient de friccio (b): " << ball.friction << '\n'
					<< "Gravetat: " << ball.gravity.x << " " << ball.gravity.y << " m/s2\n"
					<< "Increment de temps: " << ball.deltaTime << "s \n"
					<< '\n';

				setConsoleColor();
			}
			//L'usuari vol modificar la posició o càrrega d'alguna partícula
			else if (command == "modificar") {
				modifying = true;
				coutHelpModify();
			}
			//Desactivar la consola i passar a renderitzar
			else if (command == "play") {
				doConsole = false;
				coutHelpRender();
			}
			else {
				setConsoleColor(CONCOL_LIGHT_RED);
				cout << "No es reconeix la ordre\n"
					<< "Ordre escrita: " << command << "\n\n";
				setConsoleColor();
			}
		}

		else if (doConsole and modifying) {
			using namespace std;

			//Espai perquè l'usuari modifiqui tot el necessari
			if (modifying) {
				string modifyCommand{};
				cout << "Esperant ordre per modificar:\n";
				setConsoleColor(CONCOL_WHITE);
				getline(cin, modifyCommand);
				setConsoleColor();
				cout << '\n';				

				if (modifyCommand == "fi") {
					modifying = false;
					cout << '\n';
				}
				else if (modifyCommand == "ajuda") {
					coutHelpModify();
				}
				//Constant elàstica
				else if (modifyCommand.find("k") != std::string::npos) {
					spring.k = findNthNumberInString(modifyCommand, 0);
					std::cout << "Constant elastica (k) canviada a " << spring.k << '\n';
				}
				//Longitud inicial de la molla
				else if (modifyCommand.find("lon") != std::string::npos) {
					spring.lengthInitial = findNthNumberInString(modifyCommand, 0);
					std::cout << "Longitud inicial canviada a " << spring.lengthInitial << '\n';
				}
				//Massa de la bola
				else if (modifyCommand.find("massa") != std::string::npos) {
					ball.mass = findNthNumberInString(modifyCommand, 0);
					std::cout << "Massa canviada a " << ball.mass << " kg\n";
				}
				//Constant de fricció de la bola
				else if (modifyCommand.find("b") != std::string::npos) {
					ball.friction = findNthNumberInString(modifyCommand, 0);
					std::cout << "Coeficient de friccio (b) canviada a " << ball.friction << '\n';
				}
				//Vector gravetat
				else if (modifyCommand.find("gravetat") != std::string::npos) {
					ball.gravity.y = findNthNumberInString(modifyCommand, 0);
					std::cout << "Gravetat canviada a " << ball.gravity.y << " m/s2\n";
				}
				//deltaTemps
				else if (modifyCommand.find("temps") != std::string::npos) {
					ball.deltaTime = findNthNumberInString(modifyCommand, 0);
					std::cout << "Increment de canviada a " << ball.deltaTime << " s\n";
				}
				//Extrem fix de la molla
				else if (modifyCommand.find("extrem") != std::string::npos) {
					springFixedPoint.x = 7+findNthNumberInString(modifyCommand, 0);
					springFixedPoint.y = 4-findNthNumberInString(modifyCommand, 1);
					std::cout << "Extrem fix de la molla canviada a " << -7+springFixedPoint.x
						<< " " << 4-springFixedPoint.y << " m \n";
				}
				//Posició inicial de la bola
				else if (modifyCommand.find("pos") != std::string::npos) {
					ballInitPoint.x = 7+findNthNumberInString(modifyCommand, 0);
					ballInitPoint.y = 4-findNthNumberInString(modifyCommand, 1);
					std::cout << "Posicio inicial de la bola canviada a " << -7+ballInitPoint.x
						<< " " << 4-ballInitPoint.y << " m \n";
				}
				//Mode "molla lliure"
				else if (modifyCommand.find("mollalliure") != std::string::npos) {
					doFreeSpring = !doFreeSpring;
					std::cout << "mode \"molla lliure\" canviada a " << doFreeSpring << '\n';
				}
				else {
					setConsoleColor(CONCOL_LIGHT_RED);
					cout << "No es reconeix la ordre\n"
						<< "Ordre escrita: " << modifyCommand << "\n\n";
					setConsoleColor();
				}
			}
		}
		

		//!doConsole
		//Render
		else {
			if (isMouseDown or mouseWheel != 0) {
				cursorPosition = getCursorPosition()/pxPerMeter;
				Vector hitbox = {100, 100};

				if ( abs(cursorPosition.x-cursorPosition.x) < hitbox.x ) {
					if (isMouseDown) {
						ball.position = cursorPosition;
						ball.positionLast = cursorPosition;
						ball.velocity = {0, 0};
						ball.velocityLast = {0, 0};
					}

					//mouseWheel != 0
					else {
						if (mouseWheel == 1) {
							ball.mass += 1;
						}
						else {
							ball.mass += -1;
						}

						std::cout << "Massa " << ball.mass << '\n';
					}
				}
			}
			/*Què fer quan es deixa anar  else if (wasMouseDownLastFrame) {
				std::cout << "Posicio -> " << ball.position.x << " " << ball.position.y << '\n';
			}*/

			//Actualitzar les variables de la molla amb
			//la posició del cursor i
			//la posició actual de la bola
			if (doFreeSpring) {
				cursorPosition = getCursorPosition()/pxPerMeter;
				spring.updateSpring( cursorPosition , ball.position);
			}
			else {
				spring.updateSpring(springFixedPoint, ball.position);
			}

			//Actualitzar les variables de la bola amb
			//la força que la molla aplica sobre ella
			ball.updateBall(spring.force);

			enum textureIDs {
				TEX_BACKGROUND,
				TEX_ARROW,
				TEX_SPRING,
				TEX_CIRCLE_RED,
				TEX_CIRCLE_BLUE,
			};

			//Netejar la pantalla, deixant només la imatge de fons
			startRender();

			if (doFreeSpring) {
				//Renderitzar la molla
				renderVector(TEX_SPRING, cursorPosition*pxPerMeter, spring.spring);
				//Renderitzar el cursor
				renderTexture(TEX_CIRCLE_RED, cursorPosition*pxPerMeter);
			}
			else {
				//Renderitzar la molla
				renderVector(TEX_SPRING, springFixedPoint*pxPerMeter, spring.spring);
				//Renderitzar el cursor
				renderTexture(TEX_CIRCLE_RED, springFixedPoint*pxPerMeter);
			}
						
			//Renderitzar la bola
			renderTexture(TEX_CIRCLE_BLUE, ball.position*pxPerMeter);

			//Posar el fotograma a la pantalla
			endRender();


			//Codi per regular els fotogrames per segon,
			//de forma que la simulació es reprodueix quasi a temps real

			int millisecondsPerFrame = 1000*ball.deltaTime;

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			int timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
			if (millisecondsPerFrame > 0) { Sleep(millisecondsPerFrame); }

			//Per si cal comprobar fotogrames per segon
			//std::chrono::steady_clock::time_point fpsEnd = std::chrono::steady_clock::now();
			//std::cout << 1000/std::chrono::duration_cast<std::chrono::milliseconds>(fpsEnd - begin).count() << '\n';
		}
	}

	//Executar el següent codi quan es tanca el joc:

	//Alliberar la memòria de les variables a Game_Render.cpp
	exitRender();

	//Tancar SDL
	SDL_Quit();

	return 0;
}