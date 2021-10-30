BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src

all: $(BIN_DIR)/out $(BIN_DIR)/SDL2.dll $(BIN_DIR)/glew32.dll $(BIN_DIR)/basic.fs $(BIN_DIR)/basic.vs

$(BIN_DIR)/out: $(OBJ_DIR)/Shader.o $(SRC_DIR)/Main.c $(OBJ_DIR)/Sandbox.o $(BIN_DIR)
	gcc -o $(BIN_DIR)/out $(SRC_DIR)/Main.c $(OBJ_DIR)/Shader.o $(OBJ_DIR)/Sandbox.o -Ilibraries/glew-2.1.0/include -Ilibraries/SDL2-2.0.12/include     -Llibraries/glew-2.1.0/lib/Release/x64 -Llibraries/SDL2-2.0.12/lib/x64 -lopengl32 -lglew32 -lSDL2 -Wall -Wpedantic -Wextra -lm

$(OBJ_DIR)/Sandbox.o: $(SRC_DIR)/Sandbox.c $(SRC_DIR)/Sandbox.h $(SRC_DIR)/Defines.h $(OBJ_DIR)
	gcc -c $(SRC_DIR)/Sandbox.c -o $(OBJ_DIR)/Sandbox.o -Wall -Wpedantic -Wextra

$(OBJ_DIR)/Shader.o: $(SRC_DIR)/Shader.c $(SRC_DIR)/Shader.h $(OBJ_DIR)
	gcc -c $(SRC_DIR)/Shader.c -o $(OBJ_DIR)/Shader.o -Ilibraries/glew-2.1.0/include -Wall -Wpedantic -Wextra
	
$(OBJ_DIR): 
	mkdir $(OBJ_DIR)
	
$(BIN_DIR): 
	mkdir $(BIN_DIR)
	
$(BIN_DIR)/basic.vs: $(BIN_DIR)
	cp $(SRC_DIR)/basic.vs $(BIN_DIR)
	
$(BIN_DIR)/basic.fs: $(BIN_DIR)
	cp $(SRC_DIR)/basic.fs $(BIN_DIR)

$(BIN_DIR)/SDL2.dll: $(BIN_DIR)
	cp libraries\SDL2-2.0.12\lib\x64\SDL2.dll $(BIN_DIR)

$(BIN_DIR)/glew32.dll: $(BIN_DIR)
	cp libraries\glew-2.1.0\bin\Release\x64\glew32.dll $(BIN_DIR)/

clean:
	rm -f -r obj bin