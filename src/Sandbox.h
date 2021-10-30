#pragma once

#include "Defines.h"

//Anzahl der verschienden Blöcke, die mit der Maus ausgewählt werden können.
#define SELECT_AMOUNT 4

typedef uint8_t pixel;

void runPhysics(pixel pixels[][WINDOW_X]);

pixel* getPixel(Vec2i pos, pixel pixels[][WINDOW_X]);

int getBlockId(pixel* pix);

_Bool setPixel(Vec2i pos, uint8_t id, pixel pixels[][WINDOW_X]);

void spawn(Vec2i pos, Vec2i lastMousePos, uint8_t currentSelectId, uint8_t spawnAmount, pixel pixels[][WINDOW_X]);

void selectPrintName(uint8_t selectId);

uint8_t getSelectAmount();

uint8_t selectConvertToBlockId(uint8_t selectId);

void initSandbox(pixel pixels[][WINDOW_X]);