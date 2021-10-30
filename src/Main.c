/*
FALLING SAND GAME

Version: v1.0

Autor: Alexander Kirchner 
Matrikel Nummer: 2232159
Note benötigt: Ja

Entwicklungsplattform: Windows 64-bit, 
	Unix: Nicht sicher; Compiled in Linux-Subsystem, Grafik bisher nicht getestet. (anders Makefile verwenden)

Benötigte Bibliotheken:
	SDL 2.0.12 (Erstllen des Fensters)
	OpenGL/Glew 2.1.0 (Grafik)

	Windows: https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip (direkt in Hauptordner legen)
			 https://sourceforge.net/projects/glew/files/glew/2.1.0/
		Im Hauptornder (da wo auch das Makefile ist) muss ein Unterordner "libaries" erstellt werden, in den die beiden libary-Ordner ("glew-2.1.0" und "SDL2-2.0.12") kopiert werden müssen. 
	
	Unix (Ubuntu): sudo apt install libsdl2-dev libglew-dev

Compilen: Das für das entsprechende Betriebsystem benötigte Makefile umbenennen in "Makefile"(Dateiendung .win oder .unix entferen) und in der Konsole mit /make ausführen.
Falls dabei Probleme auftreten sollen liegen bereits fertig ausführbare Dateien im bin Ordner.

Kurzbeschreibung:
Zellulärer Automat der fallenden Sand und Wasser animiert. 
Dabei wird eine Array an Pixeln regelmäßig nach bestimmten und relativ einfachen Regeln verändert. In jedem Rechenschritt wird für alle Pixel eine Methode aufgerufen, die den Pixel entsprechend bewegt. Beispielsweise wird ein Sandpixel nach unten bewegt, wenn sich darunter ein Luftpixel befindet. 
Der Benutzer kann mit der Maus (beliebige Maustaste) Wasser, Sand und Steinpixel in der Welt plazieren. 
-> Mit dem Mausrad kann der zu plazierende Pixel ausgewählt werden.
-> Mit den Tasten 0-9 kann die Menge der Pixel ausgewählt werden, die gesetzt werden sollen.
-> Mit der Taste 'x' kann der aktuelle Inhalt der Sandbox gelöscht werden.

Umsetzung:
Als Grafikengine wurde OpenGL verwendet. Dadurch kann der Grafikkarte direkt ein Array aus Pixeln übergeben werden. 
Es wird direkt das Array an "id"-s übergeben. Der Datentyp pixel (typedef für uint8_t) enthält eine Zahl zwischen 0 und 255. Da nicht mehr als 20 Blöcke implementiert werden sollen, ist in dieser Id zusätzlich zur Information um welchen Block es sich handelt, auch die Schattierung (shade) des Blocks gespeichert (Damit bspw. Sand verschiedene Farbtöne hat und das fließen von Sand tatsächlich nach einer Bewegung aussieht). Um zwischen verschiedenen Blöcke unterscheiden zu können gibt es zusätzlich die "BlockId" (z.B. Sand 0, Wasser 1, Luft 255,...). Für id<200 wird die Blockid durch teilen durch 10 und abrunden erreicht. Die id's 0-9 repräsentieren also Sand in verschiedenen Schattierungen. 
Zusätzlich gibt es die "SelectId", in der alle mit der Maus plazierbaren Blöcke der Reihe nach geordnet sind, damit man diese Mit der Maus einfacher auswählen kann. Hier eine übersicht über die verschiedene Ids:

	 Pixel |     id     |   blockId   |  selectId 
	-------+------------+-------------+------------
	 Sand  |    0 - 9   |      0      |     0 
	-------+------------+-------------+------------
	 Wasser|   10 - 19  |      1      |     1
	-------+------------+-------------+------------
	 Stone |   20 - 29  |      2      |     2
	-------+------------+-------------+------------
	 Air   |     255    |     255     |     3

In der Grafikkarte wird im Fragment-Shader (wird für jeden Pixel einmal aufgerufen) mit der Id jeweils die Farbe des Pixels bestimmt.  
Die Implentierung der Grafik (vorallem Vertexshader) basiert ursprünglich auf einem Youtube Tutorial zu OpenGL in C++ (https://www.youtube.com/watch?v=yRYHly3bl2Q&list=PLStQc0GqppuWBDuNWnkQ8rzmyx35AINyt) 

Übersicht: 
	Main.c: Initialisierung und Hauptschleife
	Shader.c/.h: Einlesen und Compilen der Shader Dateien (runtime), Ausgabe von Fehlermeldungen; Initialisierung der Shader
	basic.vs/.fs: Quellcode des Fragment- und Vertexshaders
	Sandbox.c/.h: Ausführen der Physik; Methoden zum verändern des Pixelarrays und spawnen von neuen Pixeln; Konvertierungen zwischen verschiedene Ids. 
	Defines.h: Hinzufügen von in mehreren Dateien benötigten Bibliotheken; Implementierung eines einfachen zweidimensionalen Integer Vektors Vec2i; struct Vertex
*/


// Glew and OpenGL
#define GLEW_STATIC // static link (without .dll file)
#include <GL/glew.h>

// SDL (Erstellen des Fensters)
#define SDL_MAIN_HANDLED  // dynamic link => add .dll file (libraries\SDL2-2.0.12\lib\[x86 or x64]\SDL2.dll) in bin-folder

#if defined(__WIN64)
	#include <SDL.h> 
#else // Unix
	#include <SDL2/SDL.h>
#endif

#define PRINT_FPS 0 // 0: Nein, 1: Ja

#include "Defines.h"
#include "Shader.h"
#include "Sandbox.h"


int main() {
	SDL_Window* window;
	SDL_Init(SDL_INIT_EVERYTHING);

	// initialisieren des Frame Buffers
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);  // Für jede Farbe 8 bit
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Title, Position, size, flags
	window = SDL_CreateWindow("Falling Sand", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_X,WINDOW_Y, SDL_WINDOW_OPENGL);  
	SDL_GL_CreateContext(window);  // returns SDL_GLContext glContext

	//initialize OpenGL
	GLenum err = glewInit();  // initialize function pointers
	if (err != GLEW_OK) { printf("Err: %s\n", glewGetErrorString(err)); exit(0); }
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));

	const GLubyte* renderer = glGetString(GL_RENDERER);
	printf("Renderer: %s\n", renderer);

	//create Texture
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Verhalten bei Verkleinerung der Texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Vergrößerung der Texture (Magnification) gl_nearest/linear: Interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE, 0);
	
	//create Vertices: Spannt eine Fläche über das gesamte Fenster auf, die als Grafikebene verwendet wird
	Vertex vertices[] = {
		(Vertex){ .x = -1.0f, .y = -1.0f, .z = 0.0f,    .u = 0.0f, .v = 0.0f   }, // .r = 0.0f, .b = 0.0f, .g = 0.0f, .a = 1.0f
		(Vertex){ .x = -1.0f, .y = 1.0f, .z = 0.0f,    .u = 0.0f, .v = 1.0f,  },
		(Vertex){ .x = 1.0f, .y = -1.0f, .z = 0.0f,    .u = 1.0f, .v = 0.0f,  },
		(Vertex){ .x = 1.0f, .y = 1.0f, .z = 0.0f,    .u = 1.0f, .v = 1.0f,  }
	};
	uint32_t numVertices = 4;
	

	GLuint bufferId;
	GLuint vao;
	glGenVertexArrays(1, &(vao));  // speichert buffer and layout
	glBindVertexArray(vao);

	glGenBuffers(1, &(bufferId));
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW); 

	glEnableVertexAttribArray(0); // change vao
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, x));  // num Buffers, 3 coordinates, floats, ... , size, offsetof(struct Vertex, x) = 0
		
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
	
	glBindVertexArray(0);


	// Erstellt Vertex und Fragment-Shader
	GLuint shaderId = createShader("basic.vs", "basic.fs");
	bindShader(shaderId);

	int colorUniformLocation = glGetUniformLocation(shaderId, "u_color");
	if (colorUniformLocation != -1) {
		glUniform4f(colorUniformLocation, 1.0f, 0.0f, 1.0f, 1.0f);
	}

	int textureUniformLocation = glGetUniformLocation(shaderId, "u_texture");
	if (textureUniformLocation != -1) glUniform1i(colorUniformLocation, 0);  // texture slot 0

	#if PRINT_FPS==1 // FPS ausgeben
	// Timer:
	uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
	uint64_t lastCounter = SDL_GetPerformanceCounter();
	float delta = 0.0f;
	#endif
	
	// Variablen für die Benutzersteuerung
	Vec2i lastMousePos = {-1,-1};
	// Aktuell ausgewählter Block
	uint8_t currentSelectId = 0;
	selectPrintName(currentSelectId);
	// Menge der neuen Pixel die erstellt werden sollen (0-9), wobei 0 bedeutet 1 Pixel
	uint8_t spawnAmount = 4;
	// nimmt Wert 1 an, wenn das Programm geschlossen und die Schleife abgebrochen werden soll.
	_Bool close = 0;
	
	// Setup Sandbox
	pixel pixels[WINDOW_Y][WINDOW_X];
	initSandbox(pixels);
	
	
	
	// Hauptschleife: Wird ausgeführt solange das Programm läuft
	while (!close) {
		
		runPhysics(pixels);

		/* Grafik ausgeben:
		Da immer die gleiche Textur verwendet wird muss diese nicht aktiviert und gebindet werden. 
		Sonst: glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureId);
		 */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, WINDOW_X, WINDOW_Y, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

		glBindVertexArray(vao); // bind Vertexbuffer
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices); // Alternativen: POINTS, LINES, LINE_STRIP, LINE_LOOP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN

		SDL_GL_SwapWindow(window); //double Buffering

		/* Check and handle events*/
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT ) close = 1;// Programm Schließen
			else if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_ESCAPE)close = 1;
				if(event.key.keysym.sym >= 	SDLK_0 && event.key.keysym.sym <= SDLK_9 )spawnAmount = event.key.keysym.sym-SDLK_0;
				if(event.key.keysym.sym == SDLK_x)initSandbox(pixels);
			}else if(event.type == SDL_MOUSEWHEEL){ // Ausgewählten Block verändern
				currentSelectId-=event.wheel.y;
				currentSelectId %= getSelectAmount();
				selectPrintName(currentSelectId);
			}
		}
		
		// Position der Maus bestimmen. Falls linke Maustaste gedrückt wird der aktuell ausgewählte Block gespawn.
		Vec2i mouse;
		if(SDL_GetMouseState(&mouse.x, &mouse.y)){
			Vec2i pos = {mouse.x,WINDOW_Y-mouse.y};
			spawn(pos,lastMousePos, currentSelectId, spawnAmount, pixels);
			lastMousePos = pos;
		}else lastMousePos.x = -1;
		
		#if PRINT_FPS==1 // FPS ausgeben
			uint64_t endCounter = SDL_GetPerformanceCounter();
			uint64_t counterElapsed = SDL_GetPerformanceCounter() - lastCounter;
			delta = ((float)counterElapsed) / (float)perfCounterFrequency;
			lastCounter = endCounter;	
			uint32_t fps = (uint32_t)(((float)perfCounterFrequency) / ((float)counterElapsed));
			printf("Fps: %d\n", fps); 
		#endif
		
	}

	// Ende des Programms: Speicher freigeben
	glDeleteTextures(1, &textureId);
	freeShader(shaderId);
	glDeleteBuffers(1, &bufferId);
	
	return 0;
}