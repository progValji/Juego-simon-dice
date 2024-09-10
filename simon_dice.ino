#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

struct Nodo{
  int dato;
  Nodo *siguiente;
};

#define pinBuzzer 9
#define tiempoEspera 500
#define esperaBoton 250

const int tonos[] = {80, 177, 294, 359};
const int leds[] = {10, 11, 2, 3};
const int botones[] = {4, 5, 6, 7};
const int tonosDerrota[] = { 1000, 900, 800, 700, 600, 500 };
#define toneDuration 100

Nodo *frente = nullptr;
Nodo *fin = nullptr;

//----Funciones para la cola------
void insertarCola(Nodo *&frente, Nodo *&fin, int n){
  Nodo *nuevoNodo = new Nodo();

  nuevoNodo->dato = n;
  nuevoNodo->siguiente = nullptr;

  if(colaVacia(frente)){
    frente = nuevoNodo;
  }
  else{
    fin->siguiente = nuevoNodo;
  }
  fin = nuevoNodo;
}
void quitarCola(Nodo *&frente,Nodo *&fin){
	Nodo *aux = frente;
	
	if(frente == fin){
		frente = nullptr;
		fin = nullptr;
	}
	else{
		frente = frente->siguiente;
	}
	delete aux;
}
int obtenerIndice(Nodo *frente, int n){
  int contador = 0;
  Nodo *apoyo = frente;

  while(contador != n){
    apoyo = apoyo->siguiente;
    contador++;
  }
  return apoyo->dato;
}
bool colaVacia(Nodo *frente){
  return (frente == nullptr)? true:false;
}

//Funcion para reproducir sonido derrota
void derrota(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(pinBuzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(pinBuzzer, LOW);
    delayMicroseconds(tone);
  }
}

void menu(){
  int8_t aux = 0;
  lcd.setCursor(0,0); lcd.print(">");
  lcd.setCursor(1,0); lcd.print("Dos jugadores");
  lcd.setCursor(1,1); lcd.print("Contra la maquina");

  while(digitalRead(botones[0]) == HIGH){
    if(digitalRead(botones[1]) == LOW){
        delay(250);
        lcd.setCursor(0,0); lcd.print(" ");
        lcd.setCursor(0,1); lcd.print(">");
        aux = 1;
    }
    if(digitalRead(botones[2]) == LOW){
       delay(250);
       lcd.setCursor(0,1); lcd.print(" ");
       lcd.setCursor(0,0); lcd.print(">");
       aux = 0;
    }
  }
  if(aux == 0)
     DOSpersonas();
  else
     maquina();  
}

void DOSpersonas(){
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("En proceso..");
  Serial.println("Entre a la funcion de DOSpersonas");
}

void maquina(){
  int tonoAleatorio;
  int8_t indiceAleatorio, contador = 0, puntaje = 0, iterador = 0;
  char banderaBien = 'f', banderaError = 'f';
  bool jugarVSmaquina = true;
  
  lcd.clear(); lcd.home(); lcd.print("CARGANDO...");
  delay(1200);
  lcd.clear(); 

  while(jugarVSmaquina){
    lcd.setCursor(2, 1); lcd.print("Escucha...");
    //proceso mostrar
    if(colaVacia(frente)){
      indiceAleatorio = random(0, 4); 
      insertarCola(frente,fin,indiceAleatorio);
      contador++;
      tonoAleatorio = tonos[indiceAleatorio]; 
          
      tone(pinBuzzer,tonoAleatorio); digitalWrite(leds[indiceAleatorio], HIGH);
      delay(tiempoEspera);
      noTone(pinBuzzer); digitalWrite(leds[indiceAleatorio], LOW);
      delay(tiempoEspera);
    }
    else{
      for(int i = 0; i<contador; i++){
        indiceAleatorio = obtenerIndice(frente,i);
        tonoAleatorio = tonos[indiceAleatorio];

        tone(pinBuzzer,tonoAleatorio); digitalWrite(leds[indiceAleatorio], HIGH);
        delay(tiempoEspera);
        noTone(pinBuzzer); digitalWrite(leds[indiceAleatorio], LOW);
        delay(tiempoEspera);
      }
      indiceAleatorio = random(0, 4); 
      insertarCola(frente,fin,indiceAleatorio);
      contador++;
      tonoAleatorio = tonos[indiceAleatorio]; 
          
      tone(pinBuzzer,tonoAleatorio); digitalWrite(leds[indiceAleatorio], HIGH);
      delay(tiempoEspera);
      noTone(pinBuzzer); digitalWrite(leds[indiceAleatorio], LOW);
      delay(tiempoEspera);
    }
    //proceso lectura
    lcd.clear();
    lcd.setCursor(6, 0); lcd.print("Tu turno");
    lcd.setCursor(0, 1); lcd.print("Puntaje: "); lcd.setCursor(9, 1); lcd.print(puntaje); 
    indiceAleatorio = obtenerIndice(frente, iterador);
    while(banderaBien == 'f' && banderaError == 'f'){
      if(digitalRead(botones[indiceAleatorio]) == LOW){
        delay(esperaBoton);
        puntaje++; iterador++;
        lcd.setCursor(12,1); lcd.print("Bien");
        lcd.setCursor(9,1); lcd.print("  "); lcd.setCursor(9, 1); lcd.print(puntaje);
        if(iterador == contador){ 
          banderaBien = 't';
        }
        else{
          indiceAleatorio = obtenerIndice(frente, iterador);
        }
      }
      else{
        for(int i = 0; i<4; i++){
          if(digitalRead(botones[i]) == LOW && i != indiceAleatorio){
            delay(esperaBoton);
            lcd.clear(); lcd.home(); lcd.print("Has perdido :(");
            for(int i = 0; i < sizeof(tonosDerrota) / sizeof(tonosDerrota[0]); i++) {
              derrota(tonosDerrota[i], toneDuration);
              delay(100);  
            }
            banderaError = 't';
            while(frente != nullptr){
              quitarCola(frente,fin);
            }
          }
        }
      }
    }
    delay(1000);
    if(banderaError == 't'){
      lcd.clear(); menu();
    }
    banderaBien = 'f'; banderaError = 'f'; iterador = 0;
    lcd.clear();
  }
}

void setup() {
  Wire.begin(); 
  lcd.init(); 
  lcd.backlight();
  randomSeed(analogRead(0));
  Serial.begin(9600);

  for(int8_t i = 0; i<4; i++){
    pinMode(leds[i], OUTPUT);
  }
  for(int8_t i = 0; i<4; i++){
    pinMode(botones[i], INPUT_PULLUP);
  }

  lcd.setCursor(0, 0); lcd.print("Bienvenido a"); lcd.setCursor(0, 1); lcd.print("Simon dice");
  delay(1200);
  lcd.clear();
}

void loop() { 
  menu();
}
