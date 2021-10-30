#include "Sandbox.h"

#include <math.h>

/*Vertauscht zwei Pixel*/
void pixSwap(pixel* a, pixel* b){
	pixel c = *a;
	*a = *b;
	*b = c;
}

/* Berechnungen für einen Sandpixel */
void compute(unsigned int x, unsigned int y, pixel pixels[][WINDOW_X]){
	Vec2i pos = {x,y};
	pixel*this = getPixel(pos, pixels);
	int thisId = getBlockId(this);
	
	//Falls kein Sand => nichts tun.
	if(thisId == SAND_ID){
	
		/*betrachtet die drei Blöcke darunter:
		   a       aktiver Block
		ul u ur   	unten rechts usw.*/
		pos.x--; pos.y--;
		pixel* underLeft = getPixel(pos, pixels);
		int underLeftId = getBlockId(underLeft);
		pos.x++;
		pixel* under = getPixel(pos, pixels);
		int underId = getBlockId(under);
		pos.x++;
		pixel* underRight = getPixel(pos, pixels);
		int underRightId = getBlockId(underRight);
		
		// Wenn darunter Luft oder Wasser => fallen
		if(underId == AIR_ID) pixSwap(under, this);
		// Wenn linker unterer Block frei => fallen nach links unten
		// außer links und rechts unten frei => zufällig nach links oder Rechts fallen.
		// einfacher Zufall über shade-Farbe des Blocks%2
		else if(  (underLeftId==AIR_ID || underLeftId == WATER_ID)  &&  
				( (underRightId!=AIR_ID && underRightId != WATER_ID) || (*this+x)%2 )   ) 
			pixSwap(underLeft, this);
		// Wenn rechter unterer Block frei => fallen nach rechts unten
		else if( underRightId==AIR_ID || underRightId == WATER_ID) pixSwap(underRight, this);
	}	
	else if(thisId == WATER_ID){
		/*betrachtet die Blöcke links und rechts und die drei Blöcke darunter:
			l a r       links aktiver Block rechts
		   ul u ur   	unten rechts usw.*/
		pos.x--; 
		pixel* left = getPixel(pos, pixels);
		int leftId = getBlockId(left);
		pos.y--;
		pixel* underLeft = getPixel(pos, pixels);
		int underLeftId = getBlockId(underLeft);
		pos.x++;
		pixel* under = getPixel(pos, pixels);
		int underId = getBlockId(under);
		pos.x++;
		pixel* underRight = getPixel(pos, pixels);
		int underRightId = getBlockId(underRight);
		pos.y++;
		pixel* right = getPixel(pos, pixels);
		int rightId = getBlockId(right);
	
		// Wenn darunter Luft => fallen
		if(underId == AIR_ID) pixSwap(under, this);
	
		// Wenn linker unterer Block frei => fallen nach links unten
		// außer links und rechts unten frei => zufällig nach links oder Rechts fallen.
		// einfacher Zufall über shade-Farbe des Blocks%2
		else if(  underLeftId==AIR_ID  &&  
				( underRightId!=AIR_ID || *this%2 ) ) 
			pixSwap(underLeft, this);
		// Wenn rechter unterer Block frei => fallen nach rechts unten
		else if( underRightId==AIR_ID ) pixSwap(underRight, this);
	
		// gleiches für die Blöcke links und rechts
		else if(  leftId==AIR_ID  &&  
				( rightId!=AIR_ID || *this%2 ) ) 
			pixSwap(left, this);
		// Wenn rechter unterer Block frei => fallen nach rechts unten
		else if( rightId==AIR_ID ) pixSwap(right, this);
		
	}
}


/* "Hauptmethode": Wird in der Schleife in der Mainmethode aufgerufen
   Pixel werden verschoben, dass die Entsprechenden Animationen entstehen */
void runPhysics(pixel pixels[][WINDOW_X]){
	for (unsigned int y = 0; y < WINDOW_Y; y++) {
		for (unsigned int x = 0; x < WINDOW_X; x++) {
			/* Wird von unten nach oben und rechts nach links für jedes Pixel aufgerufen:
				------->---Ende
				------->----->^
				START-->----->^
			Dadurch kommt es links und rechts zu unterschiedlichem Verhalten, da auf einer Seite die Blöcke bereits gefallen sind und auf der anderen noch nicht.
			Daher wird die x Koordinate jedes zweite Mal (y%2) invertiert. Damit erhält man diesen "Weg":
				------->---Ende
				^------<-------
				START-->----->^
			*/
			int xNew = x;
			if(y%2)xNew=WINDOW_X-x;
			compute(xNew,y,pixels);
		}
	}
}

/* Gibt den Pointer auf den Pixel an der entsprechenden Position zurück.
   Wenn außerhalb der Sandbox, dann NULL */
pixel* getPixel(Vec2i pos, pixel pixels[][WINDOW_X]){
	if(pos.x<0 || pos.x >=WINDOW_X || pos.y <0 || pos.y>=WINDOW_Y)return NULL;
	
	return &pixels[pos.y][pos.x];
}

/* Gibt die BlockId des Pixels zurück oder -1 falls undefiniert zurück */
int getBlockId(pixel* pix){
	if(pix == NULL)return -1;
	else if(*pix>200)return *pix;
	else return *pix/10;
}

/* Setzt an der entsprechenden Position einen neuen Pixel */
_Bool setPixel(Vec2i pos, uint8_t id, pixel pixels[][WINDOW_X]){
	pixel* pix = getPixel(pos, pixels);
	if(!pix)return 0;
	*pix = id;
	return 1;
}

/* gibt eine zufällig generierte id zum jeweiligen Block zurück */
uint8_t convertBlockIdToId(uint8_t id){
	// Zufällige Verteilung des Farbtons ("shade" im Shader)
	if(id < 200){
		id = id*10+rand()%10;
	}
	return id;
}


/* Spawnt zufällig verteilt um die Position Pixel. Wird zum Beispiel die Verteilung des Sands verwendet */
void spawnSpread(Vec2i pos, uint8_t id, uint8_t amount, pixel pixels[][WINDOW_X]){
	// Alle Blöcke dieses Aufrufs haben die gleiche Shade. Dadurch entstehen in den Sandpyramiden interessantere Strukturen.
	id=convertBlockIdToId(id);
	int d0 = 2*amount+1;  // Verteilungsdurchmesser
	for(int i = 0; i<amount; i++){
		int r = rand()%(d0*d0*amount);  // Zufallszahl
		int r1 = r%amount; // Verteilungsradius dieses Pixels
		int d1 = (r1*2 + 1); // Verteilungsdurchmesser dieses Pixels
		int c = (r/amount)%(d1*d1); // Ort des Pixels
		Vec2i posN = {  pos.x-r1+(c/d1) , pos.y-r1+(c%d1) };
		setPixel(posN, id ,pixels);
	}
}

/* Spawnt "feste"/zusammenhängende Blöcke. Falls lastMousePos.x ungleich -1 wird zwischen lastMousePos und pos interpoliert */
void spawnSolid(Vec2i pos, Vec2i lastMousePos,  uint8_t blockId, uint8_t amount, pixel pixels[][WINDOW_X]){
	/* Zeichnet improvisierten Kreis mit festem Durchmesser 5;
	    _ x x x _
	    x x x x x 
	    x x x x x 
	    x x x x x
	    _ x x x _
	*/
	int r = rand();
	uint8_t id = convertBlockIdToId(blockId);
	for(int i = -amount; i<amount+1; i++){
		for(int j = -amount; j<amount+1; j++){
			if((r*i)%8 == 0) id = convertBlockIdToId(blockId);
			if( sqrt(pow(i,2)+pow(j,2))<=amount ){
				Vec2i posN = {  pos.x+i , pos.y+j};
				setPixel(posN, id, pixels);
			}
		}
	}
	// Die Methode wird rekursiv aufgerufen um zwischen zwei Punkten zu interpolieren. 
	// Wenn die Koordinaten lastMousePos und pos übereinstimmen wird die Rekursion abgebrochen.
	if(lastMousePos.x == -1 || (lastMousePos.x == pos.x && lastMousePos.y == pos.y) )return; 
	
	int deltaX = lastMousePos.x - pos.x;
	int deltaY = lastMousePos.y - pos.y;
	if(deltaX*deltaX > deltaY*deltaY){
		if(deltaX>0) pos.x++;
		else pos.x--;
	}else{
		if(deltaY>0) pos.y++;
		else pos.y--;
	}
	spawnSolid(pos, lastMousePos, blockId, amount, pixels);
}

/* Spawnt den aktuell ausgewählten Block an der entsprechenden Stelle */
void spawn(Vec2i pos,Vec2i lastMousePos,  uint8_t currentSelectId, uint8_t spawnAmount, pixel pixels[][WINDOW_X]){
	uint8_t id = selectConvertToBlockId(currentSelectId);
	
	spawnAmount++;
	if(id == SAND_ID  || id == WATER_ID) spawnSpread(pos, id, spawnAmount, pixels);
	else if(id == STONE_ID || id == AIR_ID)   spawnSolid(pos, lastMousePos, id, spawnAmount, pixels);
}

// Methoden zum auswählen des Pixels mit dem Mausrad
const char selectNames[SELECT_AMOUNT][10]={"Sand", "Water", "Stone", "Air"};
const uint8_t selectBlockId[SELECT_AMOUNT] = {0,1,2,255};

uint8_t selectConvertToBlockId(uint8_t selectId){
	selectId %= SELECT_AMOUNT;
	return selectBlockId[selectId];
}
void selectPrintName(uint8_t selectId){
	selectId %= SELECT_AMOUNT;
	printf("Selected Block: %s (change with mouse wheel)\n", selectNames[selectId]);
}
uint8_t getSelectAmount(){
	return SELECT_AMOUNT;
}

/* Initialisieren der Sandbox; Setzt an allen Pixels Luft */
void initSandbox(pixel pixels[][WINDOW_X]){
	for (int i = 0; i < (WINDOW_X * WINDOW_Y); i++) {
		pixels[i/WINDOW_X][i%WINDOW_X] = AIR_ID;
	}
}