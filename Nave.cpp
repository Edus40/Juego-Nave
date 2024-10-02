#include <iostream>
#include <allegro.h>
#include <cmath>

using namespace std;

void rotar(float &x, float &y, float cx, float cy, float da);
void pintar_nave(float cx, float cy, BITMAP *buffer);
void mover_nave(float &cx, float &cy, float &vx, float &vy);
void aceleracion(float da, float &vx, float &vy);
void pintar_motor(float da, float cx, float cy, BITMAP *buffer);
void medidor_combustible(bool gastar_combustible, float &combustible, BITMAP *buffer);
void pintar_nivel(int num_nivel, BITMAP *buffer);
void explocion(float cx, float cy, BITMAP *buffer, int num_nivel);
bool game_over(float &cx, float &cy, float &vx, float &vy, float &combustible, BITMAP *buffer, int &num_nivel);
bool aterrizar(float cx, float cy, float vx, float vy, BITMAP *buffer, int num_nivel);
bool choque_triangulo(float x1, float y1, float x2, float y2, float p1x, float p1y, float p2x, float p2y, string tipo_triangulo);
bool choque_nave(int num_nivel, float cx, float cy);
void avanzar_nivel(float &cx, float &cy, float &vx, float &vy, int &num_nivel, float &combustible, BITMAP *buffer);
void reiniciar_nivel(float &cx, float &cy, float &vx, float &vy, float &combustible);

int main() {
	
	allegro_init();
	install_keyboard();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 740, 500, 0, 0);
	BITMAP *buffer = create_bitmap(740,500);
	
	float cx, cy;
	float vx, vy;
	cx = 710; cy =100;
	vy = -2;
	float combustible = 100; // 100 significa tanque de combustible lleno.
	int num_nivel = 1;

	while (!key[KEY_ESC] && !game_over(cx, cy, vx, vy, combustible, buffer, num_nivel)) {
		bool gastar_combustible = false;
		clear_to_color(buffer, 0X000000);
		pintar_nivel(num_nivel, buffer);
		mover_nave(cx, cy, vx, vy);
		
		if (key[KEY_UP] || key[KEY_W] && combustible > 0){
			aceleracion(0, vx, vy); // Cuando el angulo = 0, la aceleración será hacia arriba.
			pintar_motor(0, cx, cy, buffer);
			gastar_combustible = true;
		}
		if (key[KEY_RIGHT] || key[KEY_D] && combustible > 0){
			aceleracion(-90, vx, vy);
			pintar_motor(-90, cx, cy, buffer);
			gastar_combustible = true;
		}
		if (key[KEY_LEFT] || key[KEY_A] && combustible > 0){
			aceleracion(90, vx, vy);
			pintar_motor(90, cx, cy, buffer);
			gastar_combustible = true;
		}
		
		medidor_combustible(gastar_combustible, combustible, buffer);
		pintar_nave(cx, cy, buffer);
		blit(buffer, screen, 0, 0, 0, 0, 740, 500);
		avanzar_nivel(cx, cy, vx, vy, num_nivel, combustible, buffer);
		rest(20);

	}
	return 0;
}
END_OF_MAIN()

void reiniciar_nivel(float &cx, float &cy, float &vx, float &vy, float &combustible){
	cx = 710; cy = 100;
	vx = 0, vy = -2;
	combustible = 100;
}

void avanzar_nivel(float &cx, float &cy, float &vx, float &vy, int &num_nivel, float &combustible, BITMAP *buffer){
	if(aterrizar(cx, cy, vx, vy, buffer, num_nivel) == true){
		cx = 710; cy = 100;
		vy = -2; vx = 0; 
		if(num_nivel != 2) num_nivel++;
		while(!key[KEY_E]){
			textout_centre_ex(screen, font, "PULSA (E) PARA AVANZAR AL SIGUIENTE NIVEL", 370, 250, 0XFCFC05, 0X000000);
			rest(10);
		}
		combustible = 100;
	}
}

bool choque_nave(int num_nivel, float cx, float cy){
	
	// Pata izquierda.
	float r1x = cx-20, r1y = cy;
	float r2x = cx-10, r2y = cy+20;
	// Pata derecha.
	float p1x = cx+10, p1y = cy;
	float p2x = cx+20, p2y = cy+20;
	// Cuerpo nave.
	float q1x = cx-10, q1y = cy-15;
	float q2x = cx+10, q2y = cy;
	
	if(num_nivel == 2){
		if(choque_triangulo(110, 100, 300, 500, r1x, r1y, r2x, r2y, "abajo") == true || choque_triangulo(110, 100, 300, 500, p1x, p1y, p2x, p2y, "abajo") == true || choque_triangulo(110, 100, 300, 500, q1x, q1y, q2x, q2y, "abajo") == true) return true;	
		else if (choque_triangulo(500, 500, 600, 300, r1x, r1y, r2x, r2y, "abajo") == true || choque_triangulo(500, 500, 600, 300, p1x, p1y, p2x, p2y, "abajo") == true || choque_triangulo(500, 500, 600, 300, q1x, q1y, q2x, q2y, "abajo") == true) return true;
		else if (choque_triangulo(600, 300, 800, 500, r1x, r1y, r2x, r2y, "abajo") == true || choque_triangulo(600, 300, 800, 500, p1x, p1y, p2x, p2y, "abajo") == true || choque_triangulo(600, 300, 800, 500, q1x, q1y, q2x, q2y, "abajo") == true) return true;
		else if (choque_triangulo(200, 0, 400, 350, r1x, r1y, r2x, r2y, "arriba") == true || choque_triangulo(200, 0, 400, 350, p1x, p1y, p2x, p2y, "arriba") == true || choque_triangulo(200, 0, 400, 350, q1x, q1y, q2x, q2y, "arriba") == true) return true;
	}
	return false;
}

bool choque_triangulo(float x1, float y1, float x2, float y2, float p1x, float p1y, float p2x, float p2y, string tipo_triangulo){
	float m = (y2-y1)/(x2-x1);
	// m>0 triangulo derecho, si m<0 triangulo izquierdo.
	float b = y1-m*x1;
	// ecuación de una recta es y=mx+b
	if(tipo_triangulo == "abajo"){
		if(m>0){
			if(x1 <= p1x && p1x <= x2){
			if(p2y >= m*p1x+b) return true;
	        }
		    else{
		    	if(p1x <= x1 && x1 <= p2x){
				if(y1 <= p2y) return true;
			    }	
		    }
	    }    
	    else if(m<0){
	    	if(x1 <= p2x && p2x <= x2){
			if(p2y >= m*p2x+b) return true;
		    }
		    else{
		    	if(p1x <= x2 && x2 <= p2x){
				if(y2 <= p2y) return true;
			    }	
		    }
	    }
    }
    
    if(tipo_triangulo == "arriba"){
    	if(m>0){
			if(x1 <= p2x && p2x <= x2){
			if(p1y <= m*p2x+b) return true;
	        }
		    else{
		    	if(p1x <= x2 && x2 <= p2x){
				if(p1y <= y2) return true;
			    }	
		    }
	    }    
	    else if(m<0){
	    	if(x1 <= p1x && p1x <= x2){
	    		if(p1y <= m*p1x+b) return true;
		    }
		    else{
		    	if(p1x <= x1 && x1 <= p2x){
				if(p1y <= y1) return true;
			    }	
		    }
	    }	
	}
	return false;
}

bool aterrizar(float cx, float cy, float vx, float vy, BITMAP *buffer, int num_nivel){
	if(cy+20 >= 450 && cx-20 >= 10 && cx+20 <= 100){
		if(vy <= 1.5){
			return true;
		} 
		else {
		}	
    } 
    return false;
}

bool game_over(float &cx, float &cy, float &vx, float &vy, float &combustible, BITMAP *buffer, int &num_nivel){
	if(cx+20 >= 740 || cx-20 <= 0 || cy-15 <= 0 || cy+20 >= 500){
		explocion(cx, cy, buffer, num_nivel);
		reiniciar_nivel(cx, cy, vx, vy, combustible);
		//return true;
	}
	else if(choque_nave(num_nivel, cx, cy) == true){
		explocion(cx, cy, buffer, num_nivel);
		reiniciar_nivel(cx, cy, vx, vy, combustible);
		//return true;
	}
	else if(cy+20 >= 450 && cx-20 >= 10 && cx+20 <= 100 && vy > 1.5){
		explocion(cx, cy, buffer, num_nivel);
		reiniciar_nivel(cx, cy, vx, vy, combustible);	
	}
	else if(cx-20 <= 100 && 100 <= cx+20 && cy+20>=450){
		explocion(cx, cy, buffer, num_nivel);
		reiniciar_nivel(cx, cy, vx, vy, combustible);	
	}
	return false;
}

void explocion(float cx, float cy, BITMAP *buffer, int num_nivel){
	float x[12] = {cx-10, cx+10, cx, cx, cx+15, cx-15, cx+5, cx-10, cx+10, cx-5, cx-10, cx+10};
	float y[12] = {cy, cy, cy-15, cy+15, cy-15, cy+15, cy+5, cy-10, cy-10, cy+10, cy, cy+10};
	
	float dx[6] = {7, 6, 3, -7, -7, 0};
	float dy[6] = {2, -7, -7, -6, 0, 7};
	
	clear(screen);
	
	do{
		clear(buffer);
		pintar_nivel(num_nivel, buffer);
		int j = 0;
		for(int i=0; i<=10; i+=2){
			line(buffer, x[i], y[i], x[i+1], y[i+1], 0XFCFC05);
			rotar(x[i+1], y[i+1], x[i], y[i], 15);
			
			x[i] += dx[j]; // movimiento del pivote.
			y[i] += dy[j];
			x[i+1] += dx[j]; //movimiento del que orbita.
			y[i+1] += dy[j];
			j++;
		}

		textout_centre_ex(buffer, font, "PULSA (Q) PARA INTENTARLO DE NUEVO", 370, 250, 0XFCFC05, 0X000000);
		textout_centre_ex(buffer, font, "PULSA (ESC) PARA SALIR DEL JUEGO", 370, 260, 0XFCFC05, 0X000000);
		blit(buffer, screen, 0, 0, 0, 0, 740, 500);
		rest(20);
	}while(!key[KEY_ESC] && !key[KEY_Q]);
}

void pintar_nivel(int num_nivel, BITMAP *buffer){
	if(num_nivel == 1){
		rectfill(buffer, 10, 450, 100, 500, 0X05FC84);
	}
	
	if(num_nivel == 2){
		triangle(buffer, 110, 100, 300, 500, 110, 500, 0X7205FC);
		triangle(buffer, 500, 500, 600, 300, 600, 500, 0X7205FC);
		triangle(buffer, 600, 300, 800, 500, 600, 530, 0X7205FC);
		
		triangle(buffer, 200, 0, 400, 350, 400, 0, 0X7205FC);
		rectfill(buffer, 10, 450, 100, 500, 0X05FC84);
	}
	
	if(num_nivel == 3){
		
	}
}

void medidor_combustible(bool gastar_combustible, float &combustible, BITMAP *buffer){
	textout_centre_ex(buffer, font, "Combustible", 100, 30, 0XFC0505, 0X000000);
	rectfill(buffer, 50, 50, 50+combustible, 55, 0XFC0505);
	if(gastar_combustible == true){
		combustible -= 0.2;
	}
}

void pintar_motor(float da, float cx, float cy, BITMAP *buffer){
	float c1, c2;
	c1 = cx; c2 = cy;
	if(da != 0 && da == 90){
		c2 += 9;
		c1 += 6;
	}else if(da != 0 && da == -90){
		c2 += 9;
		c1 -= 6;
	}
	float fuego[16] = {c1-5,c2+5,  c1-10,c2+20,  c1-5,c2+20,  c1,c2+35,  c1+5,c2+20,  c1+10,c2+20,  c1+5,c2+5,  c1,c2+15};
	
	for(int i=0; i<=14; i+=2){
		rotar(fuego[i], fuego[i+1], cx, cy, da);
	}
	for(int i=0; i<=10; i+=2){
		line(buffer, fuego[i], fuego[i+1], fuego[i+2],fuego[i+3], 0XFC0505);
		if(i == 10) {
			line(buffer, fuego[i+2], fuego[i+3], fuego[i+4], fuego[i+5], 0X05FCED); 
			line(buffer, fuego[i+4], fuego[i+5], fuego[i-10], fuego[i-9], 0X05FCED);
			line(buffer, fuego[i-10], fuego[i-9], fuego[i+2], fuego[i+3], 0X05FCED);
		}
	}
}

void aceleracion(float da, float &vx, float &vy){
	float ax = 0, ay = -0.15;
	rotar(ax, ay, 0, 0, da); // Si da = 0 el vector aceleración queda intacto.
	vx += ax;
	vy += ay;
}

void pintar_nave(float cx, float cy, BITMAP *buffer){
	
	float nav[26] = {cx-20,cy+20,  cx-20,cy+10,  cx-10,cy,  cx-10,cy-10,  cx,cy-15,  cx+10,cy-10,  cx+10,cy,  cx+20,cy+10,  cx+20,cy+20,  cx-10,cy,  cx+10,cy};
	
	for(int i=0; i<=14; i+=2){
		line(buffer, nav[i], nav[i+1], nav[i+2], nav[i+3], 0XFCFC05);
		if(i == 14) line(buffer, nav[i+4], nav[i+5], nav[i+6], nav[i+7], 0XFCFC05);
	}
}

void mover_nave(float &cx, float &cy, float &vx, float &vy){
	float ax, ay;
	ax = 0.0;
	ay = 0.1;
	
	vx += ax;
	vy += ay;
	
	cx += vx;
	cy += vy;
}

/* 
   Rotar un vector con coordenadas polares a rectangulares si R es la longitud del vector origen (X1,Y1) y "Q" es el angulo que hace con el eje X
   sus coordenadas son:
    x = X1 + R*Cos(Q)
    y = Y1 + R*Sen(Q)
    
   Si queremos rotar el vector en un angulo "S", las nuevas coordenadas al rotar seran:
    x = X1 + R*Cos(Q+S)
    y = Y1 + R*Sen(Q+S)   
*/

void rotar(float &x, float &y, float cx, float cy, float da){ // da es un angulo en grados.
	float dx = x - cx;
	float dy = y -cy;
	float r = sqrt(dx*dx + dy*dy);
	float a = atan2(dy, dx);
	// pasamos da de grados a radianes.
	float da_rad = da/180*M_PI;
	a -= da_rad; // en este punto a vale la suma de los angulos.
	
	x = cx + r*cos(a);
	y = cy + r*sin(a);
	
	
	
}
