#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <regex>
//Per donar color a la consola
#include <windows.h>
//#include <chrono>

#include <SDL2/SDL.h>

#include "CustomClasses/VectorClass.h"
#include "CampCentral_Render.h"
#include "CampCentral_Physics.h"


bool doConsoleColor = false;
bool doGreenDefault = false;

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


//Actualitzar la posició del cursor
//
//Aquesta funció és un intermediari necessari ja que
//SDL_GetMouseState() retorna int,
//mentre que Vector és float
Vector getCursorPosition() {
	int x;
	int y;
	SDL_GetMouseState(&x, &y);

	return {x, y};
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

//Imprimir instruccions per fer servir consola
void coutHelpConsole() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Ordres:					Descripcio:
			"ajuda"			Llistar totes les ordres

			"color"			Donar color a la consola
			"descolor"		Desabilitar el color de la consola

			"valors tots"	Llistar tots els valors de totes les carregues
			"valors  q"		Escriure els valors d'una carrega particular
					Q1
					Q2		Exemples de ordres valides:
					Q3		>> valors Q1		>> valors q Q3
			   		Q4		>> valors Q2 Q3		>> valors q Q1 Q4

			"modificar"		Modificar la posicio o carrega d'una particula
			"dibuixar"		Passar a la grafica
	*/
	std::cout << "Ordres:\t\t\tDescripcio:\n"
		<< "\t\"ajuda\"\t\tLlistar totes les ordres\n"
		<< '\n'
		<< "\t\"color\"\tDonar color a la consola\n"
		<< "\t\"descolor\"\tDesabilitar el color de la consola\n"
		<< '\n'
		<< "\t\"valors tots\"\tLlistar tots els valors de totes les carregues\n"
		<< "\t\"valors  q\"\tEscriure els valors d'una carrega particular\n"
		<< "\t\tQ1\n"
		<< "\t\tQ2\tExemples de ordres valides:\n"
		<< "\t\tQ3\t>> Q1\t\t>> q Q3\n"
		<< "\t\tQ4\t>> Q2 Q3\t>> q Q1 Q2 Q3 Q4\n"
		<< '\n'
		<< "\t\"modificar\"\tModificar la posicio o carrega d'una particula\n"
		<< "\t\"dibuixar\"\tPassar a la grafica\n"
		<< '\n';
}

//Imprimir instruccions per modificar variables
void coutHelpModify() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Ordres:
			"ajuda"			Llista les ordres del mode modificar

			"q pos 0 0"		Canviar la posicio de la particula
			 ^     ^ ^--	Posicio en l'eix y
			 |     |----	Posicio en l'eix x
			 |----------	Nom de la particula

			"q charge 0"	Canviar la carrega de la particula
			 ^        ^-	Carrega
			 |----------	Nom de la particula

			Exemples d'ordres per canviar la posicio
			>> q  pos 0 0
			>> Q1 pos 2.5 3.4
			>> Q4 pos 0 -2.7

			Exemples d'ordres per canviar la carrega
			>> q  charge 1
			>> Q1 charge 2.3
			>> Q2 charge -0.3

			"resolucio camp 50"				Canviar cada quants pixels es mesura el camp
					Es recomana 50 o major
			"resolucio potencial 10"		Canviar cada quants pixels es mesura el potencial
					Es recomana 10 o major

			"fi"			Sortir del mode modificar
	
	*/
	std::cout << "Ordres:\n"
		<<"\tajuda\t\tLlista les ordres del mode modificar\n"
		<<'\n'
		<<"\t\"q pos 0 0\"\tCanviar la posicio de la particula\n"
		<<"\t ^     ^ ^--\tPosicio en l'eix y\n"
		<<"\t |     |----\tPosicio en l'eix x\n"
		<<"\t |----------\tNom de la particula\n"
		<<'\n'
		<<"\t\"q charge 0\"\tCanviar la carrega de la particula\n"
		<<"\t ^        ^-\tCarrega\n"
		<<"\t |----------\tNom de la particula\n"
		<<'\n'
		<<"\tExemples d'ordres per canviar la posicio\n"
		<<"\t>> q  pos 0 0\n"
		<<"\t>> Q1 pos 2.5 3.4\n"
		<<"\t>> Q4 pos 0 -2.7\n"
		<<'\n'
		<<"\tExemples d'ordres per canviar la carrega\n"
		<<"\t>> q  charge 1\n"
		<<"\t>> Q1 charge 2.3\n"
		<<"\t>> Q2 charge -0.3\n"
		<<'\n'
		<<"\t\"resolucio camp 50\"\t\tCanviar cada quants pixels es mesura el camp\n"
		<<"\t\tEs recomana 50 o major\n"
		<<"\t\"resolucio potencial 10\"\tCanviar cada quants pixels es mesura el potencial\n"
		<<"\t\tEs recomana 10 o major\n"
		<<'\n'
		<<"\t\"fi\"\t\tSortir del mode modificar\n"
		<<'\n';
}

//Imprimir instruccions per fer servir finestra gràfica
void coutHelpRender() {
	/* Escriu el següent missatge:
	\t		\t		\t		\t		\t		\t		\t		\t
	Accions disponibles:
			Arrosega els cercles per canviar la posicio

			Fes servir la roda del ratoli sobre els cercles per canviar la carrega

			Nomes es poden canviar els valors en unitats senceres
			Si vols valors decimals, fes servir la consola

			Prem C per generar el camp de forca electrica

			Prem P per tornar a la consola
	
	*/
	std::cout << "Accions disponibles:\n"
		<<"\tArrosega els cercles per canviar la posicio\n"
		<<'\n'
		<<"\tFes servir la roda del ratoli sobre els cercles per canviar la carrega\n"
		<<'\n'
		<<"\tNomes es poden canviar els valors en unitats senceres\n"
		<<"\tSi vols valors decimals, fes servir la consola\n"
		<<'\n'
		<<"\tPrem C per generar el camp de forca electrica\n"
		<<'\n'
		<<"\tPrem V per generar el camp de potencial electric\n"
		<<'\n'
		<<"\tPrem P per tornar a la consola\n"
		<<'\n';
}



int main(int argc, char *argv[]) {
	//Aquestes tres funcions han d'anar en aquest ordre
	//Inicialitzar les variables a CampCentral_Render.cpp
	initRender();
	setOrigin( getScreenCenter() );
	//Inicialitzar les partícules a CampCentral_Physics.cpp
	initPhysics();

	//Flags que s'han de conservar entre fotogrames

	bool running = true;
	bool doConsole = true;
	bool modifying = false;

	bool isMouseDown = false;
	bool wasMouseDownLastFrame = false;
	int lastParticleOnClick = -1;

	//Resolució de camp i potencial en px/m
	int fieldResolution = 50;
	int potentialResolution = 10;


	//Flags per renderitzar
	bool doDrawField = false;
	bool doDrawPotential = false;
	std::vector<std::vector<Vector>> electricField;
	std::vector<std::vector<float>> electricPotential;
	float fieldHightestModule;
	float potentialAbsoluteValue;

	//Imprimir instruccions per fer servir consola,
	//el qual s’activa primer, per defecte
	coutHelpConsole();

	//Main loop
	SDL_Event event;
	while (running)	{
		//Codi per iniciar un cronòmetre
		//per si cal evaluar l'eficiència del codi
		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		//Flags que es restableixen cada fotograma

		//Flag per indicar si hi ha input amb la roda del ratolí
		int mouseWheel = 0;
		//Flag per recalcular el camp
		bool doRenderField = false;
		//Flag per recalcular el potencial
		bool doRenderPotential = false;

		//Registre de l'input
		while (SDL_PollEvent(&event)) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				//L'usuari fa click esquerre
				if (event.type == SDL_MOUSEBUTTONDOWN) {
					isMouseDown = true;
				}
				//L'usuari deixa de fer click esquerre
				else if (event.type == SDL_MOUSEBUTTONUP) {
					isMouseDown = false;
					wasMouseDownLastFrame = true;
				}
				//No hi ha input al ratolí
				else {
					wasMouseDownLastFrame = false;
				}
			}		
			//Hi ha input a la roda del ratolí
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
			//El jugador ha près alguna tecla
			else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
					//Tecla "C" premuda:
					//Renderitzar el camp
					case SDL_SCANCODE_C:
						doDrawField = !doDrawField;
						doRenderField = true;
						break;
					
					//Tecla "V" premuda:
					//Renderitzar el potencial
					case SDL_SCANCODE_V:
						doDrawPotential = !doDrawPotential;
						doRenderPotential = true;
						break;

					//Tecla "P" premuda:
					//Pausar, és a dir, deixar de renderitzar i
					//passar a la consola
					case SDL_SCANCODE_P:
						doConsole = true;
						break;

					default:
						break;
				}
			}
			//El jugador tanca la finestra amb
			//la "x" a la part superior dreta:
			//Tancar el programa
			else if (event.type == SDL_QUIT) {
				running = false;
			}
		}
		
		//Nom de cada partícula
		//Actua com un Enum per _Physics.cpp
		const std::string particleNames[] {"q", "Q1", "Q2", "Q3", "Q4"};

		//L'usuari interactua amb el programa a través de la consola
		if (doConsole and !modifying) {
			//Demanar a l'usuari que introdueixi una ordre
			setConsoleColor();			
			std::string command{};
			std::cout << "Esperant ordre:\n";
			setConsoleColor(CONCOL_WHITE);
			std::getline(std::cin, command);
			setConsoleColor();
			std::cout << '\n';

			//L'usuari vol saber quines ordres existeixen
			if (command == "ajuda") {
				//Imprimir instruccions per fer servir consola
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
			//L'usuari vol saber els valors de totes els partícules
			else if (command == "valors tots") {
				setConsoleColor(CONCOL_CYAN);
				coutAllParticleData();
				setConsoleColor();
			}
			//L'usuari vol saber el valor d'una partícula en concret
			else if (command.find("valors") != std::string::npos) {
				//Buscar si el nom de la partícula
				//apareix en la odre
				setConsoleColor(CONCOL_CYAN);
				for (int i = 0; i < sizeof(particleNames)/sizeof(*particleNames); i++) {
					//Si és així, imprimir els valors de la partícula
					if (command.find(particleNames[i]) != std::string::npos) {
						coutParticleData(i);
					}
				}
				setConsoleColor();
			}
			//L'usuari vol modificar la posició o càrrega d'alguna partícula
			else if (command == "modificar") {
				modifying = true;
				coutHelpModify();
			}
			//L’usuari vol desactivar la consola i
			//passar a la finestra gràfica
			else if (command == "dibuixar") {
				doConsole = false;
				coutHelpRender();
			}
			//No es detecta cap ordre introduïda
			else {
				setConsoleColor(CONCOL_LIGHT_RED);
				std::cout << "No es reconeix la ordre\n"
					<< "Ordre escrita: " << command << "\n\n";
				setConsoleColor();
			}
		}

		//"Mode modificar" activat, per modificar les propietats
		else if (doConsole and modifying) {
			//Demanar a l'usuari que introdueixi
			//una ordre per modificar propietats

			std::string modifyCommand{};
			std::cout << "Esperant ordre per modificar:\n";
			setConsoleColor(CONCOL_WHITE);
			std::getline(std::cin, modifyCommand);
			setConsoleColor();
			std::cout << '\n';

			//L'usuari ha acabat de modificar
			if (modifyCommand == "fi") {
				modifying = false;
				std::cout << '\n';
			}
			//L’usuari vol saber com modificar propietats
			else if (modifyCommand == "ajuda") {
				//Imprimir instruccions per fer servir
				//la finestra gràfica
				coutHelpModify();
			}
			//L’usuari vol modificar la resolució
			//de la renderització dels camps
			else if (modifyCommand.find("resolucio") != std::string::npos) {

				if (modifyCommand.find("camp") != std::string::npos) {
					int resolucio = findNthNumberInString(modifyCommand, 0);
					if (resolucio != 0){
						fieldResolution = resolucio;
					}

					setConsoleColor(CONCOL_BLUE);
					std::cout << "Resolucio de camp canviada a "
						<< fieldResolution << "\n\n";
					setConsoleColor();
				}

				else if (modifyCommand.find("potencial") != std::string::npos) {
					int resolucio = findNthNumberInString(modifyCommand, 0);
					if (resolucio != 0){
						potentialResolution = resolucio;
					}

					setConsoleColor(CONCOL_BLUE);
					std::cout << "Resolucio de potencial canviada a "
						<< potentialResolution << "\n\n";
					setConsoleColor();
				}
			}
			//L’usuari vol modificar les propietats de una partícula
			else {
				//Buscar noms de particules
				int particleToModify = -1;
				for (int i = 0; i < sizeof(particleNames)/sizeof(*particleNames); i++) {
					if (modifyCommand.find(particleNames[i]) != std::string::npos) {
						particleToModify = i;
						break;
					}
				}

				//L’usuari vol modificar la posició de una partícula
				if ( (particleToModify != -1) and (modifyCommand.find("pos") != std::string::npos) ) {
					//Coordenades cartesianes
					//Modificar posició

					Vector newPosition;

					//L'usuari escriu 1
					//Agafar 1r i 2n nombre
					if (particleToModify == 0) {
						newPosition.x = findNthNumberInString(modifyCommand, 0);
						newPosition.y = findNthNumberInString(modifyCommand, 1);
					}

					//particleToModify != 0
					//L'usuari escriu una de Q1 Q2 Q3 Q4
					//El número en "Q1 Q2 Q3 Q4" és el primer nombre
					//Per tant, agafar 2n i 3r nombre
					else {
						newPosition.x = findNthNumberInString(modifyCommand, 1);
						newPosition.y = findNthNumberInString(modifyCommand, 2);
					}

					modifyParticlePos(particleToModify, newPosition);
					setConsoleColor(CONCOL_BLUE);
					std::cout << "Posicio de " << particleNames[particleToModify]
						<< " canviada a " << newPosition.x << "  " << newPosition.y << "\n\n";
					setConsoleColor();
				}
				//L’usuari vol modificar la posició de una partícula
				else if ( (particleToModify != -1) and (modifyCommand.find("charge") != std::string::npos) ) {
					float newCharge;

					//L'usuari escriu q
					//Agafar 1r nombre
					if (particleToModify == 0) {
						newCharge = findNthNumberInString(modifyCommand, 0);

					//L'usuari escriu una de Q1 Q2 Q3 Q4
					//El número en "Q1 Q2 Q3 Q4" és el primer nombre
					//Per tant, agafar 2n nombre
					} else {
						//El número en "Q1 Q2 Q3 Q4" és el primer nombre
						//Per tant, la càrrega és el segon nombre
						newCharge = findNthNumberInString(modifyCommand, 1);
					}

					modifyParticleCharge(particleToModify, newCharge);
					setConsoleColor(CONCOL_BLUE);
					std::cout << "Carrega de " << particleNames[particleToModify]
						<< " canviada a " << newCharge << "\n\n";
					setConsoleColor();
				}
				//Si no es troba cap nom de particula
				//o cap ordre després de la partícula
				else {
					setConsoleColor(CONCOL_LIGHT_RED);
					std::cout << "No es reconeix la ordre\n"
						<< "Ordre escrita: " << modifyCommand << "\n\n";
					setConsoleColor();
				}
			}
		}
		
		//!doConsole
		//La finestra gràfica està activa
		else {
			//Obtenir els valors de les partícules de CampCentral_Physics.cpp
			ElectricParticle *particles = getParticles();

			//Es detecten inputs amb el ratolí
			if (isMouseDown or mouseWheel != 0) {
				Vector cursorPosition = windowToScreen( getCursorPosition() );
				Vector hitbox = {100, 100};

				//Mirar si el ratolí està a sobre d’alguna partícula
				int particleOnClick = -1;
				for (int i = 0; i < getParticlesSize(); i++) {
					if ( isParticleOnClick(i, cursorPosition, hitbox) ) {
						particleOnClick = i;
					}
				}

				//No s’està arrastrant cap partícula.
				//Mirar si hi ha input en la roda del ratolí
				if (particleOnClick != -1) {
					//S'està arrastrant una partícula.
					//Indicar gràficament que s'està arrastrant la partícula,
					//i anar actualitzant la seva posició
					if (isMouseDown) {
						particles[particleOnClick].position = snapToGrid(cursorPosition, getPixelsPerMeter());
						lastParticleOnClick = particleOnClick;
					}

					//mouseWheel != 0
					//No s’està arrastrant cap partícula,
					//hi ha input en la roda del ratolí
					else {
						//Scroll up
						if (mouseWheel == 1) {
							particles[particleOnClick].charge += 1;
						}
						//Scroll down
						else {
							particles[particleOnClick].charge += -1;
						}

						std::cout << "Charge " << particleNames[particleOnClick] << " -> "
							<< particles[particleOnClick].charge << std::endl;

						//Recalcular camp i potencial
						doRenderField = true;
						doRenderPotential = true;
					}
				}
			}
			//No es detecten inputs amb el ratolí.
			//Revisar si és que l’usuari ha deixat anar la partícula
			else if (wasMouseDownLastFrame and lastParticleOnClick != -1) {
				Vector newPosition = pixelsToCartesian( particles[lastParticleOnClick].position );

				//Notificar l'usuari que s'ha modificat la posició
				std::cout << "Pos " << particleNames[lastParticleOnClick] << " -> "
					<< newPosition.x << "  "
					<< newPosition.y << std::endl;

				//Recalcular camp i potencial
				doRenderField = true;
				doRenderPotential = true;

				lastParticleOnClick = -1;
			}

			//Es comença a renderitzar

			//Netejar la pantalla, creant un nou fotograma i
			//deixant només la imatge de fons
			startRender();

			//L'usuari ha premut la tecla "V"
			//per activar el potencial
			if (doDrawPotential) {
				if (electricPotential.empty()) {
					doRenderPotential = true;
				}

				std::tuple<Vector, int, int> cartesianArray = getCartesianArray( potentialResolution );
				Vector cornerUpLeft = std::get<0>(cartesianArray);
				int maxI = std::get<1>(cartesianArray);
				int maxJ = std::get<2>(cartesianArray);

				//Cal recalcular el potencial
				//(Per estalviar CPU, només es calcula
				//en la primera vegada que es necessita,
				//després només es consulten els valors)
				if (doRenderPotential) {
					electricPotential.erase(electricPotential.begin(), electricPotential.end());
					potentialAbsoluteValue = 0;

					for (int i = 0; i < maxI; i++) {
					std::vector<float> row;
					electricPotential.push_back(row);
					for (int j = 0; j < maxJ; j++) {
						Vector point{
							cornerUpLeft.x + potentialResolution*i ,
							cornerUpLeft.y + potentialResolution*j
						};
						float totalPotential = 0;
						for (int k = 0; k < getParticlesSize(); k++) {
							//No és q
							if (k != 0) {
								totalPotential += potentialBetween(particles[k], point);
							}
						}

						electricPotential[i].push_back(totalPotential);

						if ( abs(totalPotential) > potentialAbsoluteValue) {
							potentialAbsoluteValue = abs(totalPotential);
						}
					}
					}

					//Reset per evitar recalcular quan no es necessita
					doRenderPotential = false;
				}

				//Consultar els valors i 
				//renderitzar potencial en cada punt de la pantalla
				//(és a dir, acolorir cada punt)

				for (int i = 0; i < electricPotential.size(); i++) {
				for (int j = 0; j < electricPotential[i].size(); j++) {
					Vector point{
						cornerUpLeft.x + potentialResolution*i,
						cornerUpLeft.y + potentialResolution*j
					};

					float potentialOpacity;

					float ratioToHighestPotential = abs(electricPotential[i][j]) / potentialAbsoluteValue;

					//sqrt per allanar una mica la escala exponencial del potencial
					if (electricPotential[i][j] < 0) {
						potentialOpacity = 127*
							(1 - ( sqrtf( ratioToHighestPotential ) ) );
					}
					else {
						potentialOpacity = 127*
							(1 + ( sqrtf( ratioToHighestPotential ) ) );
					}

					//failsafe
					if (potentialOpacity < 0) {
						potentialOpacity = 0;
					}
					else if (potentialOpacity > 255) {
						potentialOpacity = 255;
					}

					//1.2*potentialResolution perquè sinó apareixen patrons de Moiré a certes resolucions,
					//en forma de separacions entre els quadrats, que resulten en una graella blanca
					renderPotential( screenToWindow( point ) , 1.2f*potentialResolution, potentialOpacity);
				}
				}
			}

			//L'usuari ha premut la tecla "C"
			//per activar el camp de força
			if (doDrawField) {
				if (electricField.empty()) {
					doRenderField = true;
				}

				std::tuple<Vector, int, int> cartesianArray = getCartesianArray( fieldResolution );
				Vector cornerUpLeft = std::get<0>(cartesianArray);
				int maxI = std::get<1>(cartesianArray);
				int maxJ = std::get<2>(cartesianArray);

				//Cal recalcular el camp de força
				//(Per estalviar CPU, només es calcula
				//en la primera vegada que es necessita,
				//després només es consulten els valors)
				if (doRenderField) {
					electricField.erase(electricField.begin(), electricField.end());
					fieldHightestModule = 0;

					for (int i = 0; i < maxI; i++) {
					std::vector<Vector> row;
					electricField.push_back(row);
					for (int j = 0; j < maxJ; j++) {
						Vector point{
							cornerUpLeft.x + fieldResolution*i,
							cornerUpLeft.y + fieldResolution*j
						};
						Vector totalField;
						for (int k = 0; k < getParticlesSize(); k++) {
							//No és q
							if (k != 0) {
								totalField += fieldBetween(particles[k], point);
							}
						}

						electricField[i].push_back(totalField);

						if (totalField.module() > fieldHightestModule) {
							fieldHightestModule = totalField.module();
						}
					}
					}

					//Reset per evitar recalcular quan no es necessita
					doRenderField = false;
				}
				
				//Consultar els valors i 
				//renderitzar camp en cada punt de la pantalla
				//(és a dir, dibuixar una fletxa en cada punt)

				for (int i = 0; i < electricField.size(); i++) {
				for (int j = 0; j < electricField[i].size(); j++) {
					Vector point{
						cornerUpLeft.x + fieldResolution*i,
						cornerUpLeft.y + fieldResolution*j
					};

					float opacity = (electricField[i][j].module() / fieldHightestModule) *255;

					renderVector( screenToWindow( point ) , electricField[i][j],
								float(fieldResolution)/getPixelsPerMeter(), opacity);
				}
				}
			}

			//Render partícules
			for (int i = 0; i < getParticlesSize(); i++) {
				if (particles[i].charge != 0) {
					renderParticle(particles[i].position, particles[i].charge);
				}
			}
			
			//Posar el fotograma a la pantalla
			endRender();

			//Codi per parar el cronòmetre
			//per si cal evaluar l'eficiència del codi
			//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			//std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		}
	}

	//Executar el següent codi quan es tanca el joc:

	//Alliberar la memòria de les variables a Game_Render.cpp
	exitRender();

	//Tancar SDL
	SDL_Quit();

	return 0;
}