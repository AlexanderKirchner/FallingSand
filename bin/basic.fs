#version 330 core

layout(location = 0) out vec4 f_color;

// input: TextureCoord: Koordinaten des Pixels
in vec2 v_texCoord; 

// Textur mit BlockId
uniform sampler2D u_texture;

uniform vec4 u_color;

void main(){
	// PixelId ist in der Textur im Rot-Wert als float (0.0f bis 1.0f) gespeichert. 
	uint id = uint((texture(u_texture, v_texCoord))[0]*255); 
	uint blockId = uint(id/uint(10));  // teilen durch 10 und abrunden;
	
	// Bestimme Farbe des Pixels
	vec4 color = vec4(0.4f, 0.7f, 1.0f, 1.0f);
	
	// Berechne Schattierung
	float shade = float(id-blockId)*0.02;
	/*Ältere Idee um Zufällige Schattierung zu berechnen
	float shade = fract(sin(v_texCoord[0]*12.9898+v_texCoord[1]*78.233) * 43758.5453)*0.2f;
	nicht effizient, da "Zufallswerte" für jeden Frame neue berechnet werden müssen*/
	
	// Sicher nicht die beste Idee, aber es funktioniert
	switch(blockId){
		case 0u: color[0u] = 1.0f-shade;   color[1u] = 1.0f-shade;   color[2u] = 0.0f;         break;   //Sand
		case 1u: color[0] = 0.5f*shade;   color[1] = 0.5f*shade;   color[2] = 1.0f-shade;   break;	//Water
		case 2u: color[0] = 0.5f-shade;   color[1] = 0.5f-shade;   color[2] = 0.5f-shade;   break;	//Stone
		default:  break;
	} 
	
	// "returns" color
	f_color = color;  
}