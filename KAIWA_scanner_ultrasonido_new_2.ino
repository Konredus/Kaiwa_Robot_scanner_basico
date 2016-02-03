//-----------------------------------------------------------------
//          Programa hecho por: Konrad Peschka
//          Entidad: Kaiwa Technology
//          Fecha: Enero 2016
//-----------------------------------------------------------------


//--------------defino librerias a utilizar------------------

#include <Servo.h> 
#include <NewPing.h>
//#include "LedControl.h"
#include "LedControlMS.h"

//------------------defino alias de las conexiones---------------

 
Servo servo_eje_x;
Servo servo_eje_y;
/*
 pin 4 is connected to the DataIn 
 pin 6 is connected to the CLK 
 pin 5 is connected to LOAD 
 1 cantidad de matrices en serie
 */
   
LedControl lc=LedControl(4,6,5,1);                   //defino los pines de la matriz  (Din, Clk,  CS)


#define TRIGGER_PIN  8                             // el trigger va conectado al pin 8
#define ECHO_PIN     9                            // el echo esta en el pin 9
#define MAX_DISTANCE 100                            //distancia maxima que quiero medir

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  //defino pines del ultrasonido


//--------------------------defino las variables ------------------------------


int variable_x=100;                    //valor de inicio en el eje x, si incremento, hace giro antihorario
int variable_y=150;                    //valor de inicio en el eje y, si decremento sube


int modo=0;                           //me sirve para detectar si esta en modo manual=0 o automatico=1
 
int minimo_x=50;                      //defino maximos y minimos de cada eje
int maximo_x=160;
int minimo_y=100;
int maximo_y=150;

const int distancia_feliz=60;        //me define a partir de cual se pone contento
const int distancia_contento=30;      //distancia a partir del cual se enoja

int variable_carita=0;                //cero=enojado  uno=feliz abierto, dos=feliz cerrado, tres=contento, cuatro=contento picaron
int aux_caritas=0;                     //cero=ojos abiertos , uno = ojos cerrados
int tiempo_proximo_parpadeo=3000;     //cada 5 segundos va a parpadear
float aux_millis;                     //auxiliar que voy a utilizar para guardar el cero relativo de millis()


int distancia_anterior=MAX_DISTANCE;   //con esta variable me salteo los errores de medicion
float aux_tiempo_random;                   //guardo el tiempo relativo de millis
const int tiempo_random=4000;          //que haga el random cada 5 segundos

int vector_distancia[10]={0,0,0,0,0,0,0,0,0,0};
int aux_vector=0;
//--------------------------------------------------------------------------------------------
//                       funcion setup donde inicializo las librerias 
//--------------------------------------------------------------------------------------------


void setup() 
{
pinMode(13, INPUT);   //defino el pulsador del Joystick como entrada  
//pinMode(13, OUTPUT);                 //uso el 13 como led testigo para saber si me toma el pulsador

servo_eje_x.attach(2);              //defino en que pin esta conectado servo eje x
servo_eje_y.attach(3);               //defino en que pin esta conectado servo eje x

servo_eje_x.write(variable_x);      //me muevo a la sposiciones inicial x
delay(15);
servo_eje_y.write(variable_y);      //me muevo a la sposiciones inicial y
delay(15);

randomSeed(analogRead(A4));         //semilla para le random

  lc.shutdown(0,false);
  
   // Set the brightness to a medium values 
  lc.setIntensity(0,8);
  
   // and clear the display 
  lc.clearDisplay(0);
  
  millis();                        //lo voy a utilizar para abrir y cerrar ojitos

}

//--------------------------------------------------------------------------------------------
//                                Programa principal
//--------------------------------------------------------------------------------------------

void loop() 
{

  vision_ultrasonido();       //me quedo siempre mirando para despues mostrar las caritas

  if(variable_carita==0)          //que solo haga el random cuando no detecto a nadie
   secuencia_azar();             //hace la secuencia random de movimiento hasta que encuentra alguien
  
  if(digitalRead(13)==HIGH)
    saludo_cumple();
    
  movimiento_servos();               //llamo a la funcion que actualiza posicion de los servos  
  caritas();                        //llama a la funcion que me muestra las caritas
}

//--------------------------------------------------------------------------------------------
//                               Declaracion de Funciones
//--------------------------------------------------------------------------------------------


//-----------------------------        saludo cumple   ------------------------------

void saludo_cumple(void)
{
  lc.clearDisplay(0);
  delay(200);
  lc.writeString(0,"Kiwi");
  delay(500);
}

//-----------------------------funciones de movimiento de cabeza------------------------------

void mueve_cabeza_si(void)
{
  int i;
  for(i=0; i<2;i++)
  {
    variable_y=variable_y-3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }
  
  for(i=0; i<4;i++)
  {
    variable_y=variable_y+3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }

  for(i=0; i<2;i++)
  {
    variable_y=variable_y-3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }
  
}

void mueve_cabeza_no(void)
{
  
    int i;
  for(i=0; i<2;i++)
  {
    variable_x=variable_x-3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }
  
  for(i=0; i<4;i++)
  {
    variable_x=variable_x+3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }

  for(i=0;i<2;i++)
  {
    variable_x=variable_x-3;
    chequeo_limites();
    movimiento_servos();
    delay(50);
  }
}


//---------------------------------FUNCION VISION ULTRASONIDO----------------------------

void vision_ultrasonido(void)
{

  int distancia = sonar.ping() / US_ROUNDTRIP_CM;   //capturo primera esquina

  if(distancia==0)                     //codigo para evitar los errores en las mediciones del sensor
  distancia=distancia_anterior;
  else
  distancia_anterior=distancia;
  
 vector_distancia[aux_vector]=distancia;              //cargo el array con el valor de la distancia
 aux_vector++;      //incremento el puntero de array
 
 if(aux_vector==10)
 aux_vector=0;
 
 distancia=0;                        //lo reseteo para poder sumar solo los valores del array
 for(int i=0;i<10;i++)
 {
  distancia=distancia+vector_distancia[i];                      //sumo todos los valores del array
 }
 
  distancia=distancia/10;                      //divido pro dies para hacer el promedio
  
  if((millis()-aux_millis)>tiempo_proximo_parpadeo)   //trabajo el timer con millis para parpadeo
  {
    aux_millis=millis();
    aux_caritas=1;
  }
  
  
  if(distancia>distancia_feliz)                    //si no detecto a nadie queda carita enojado
  {
    variable_carita=0;                            //enojado
    if(aux_caritas==1)
    {
    mueve_cabeza_no();                         //agrega movimiento de NO
    aux_caritas=0;
    }
  }
  
  if((distancia>distancia_contento)&&(distancia<distancia_feliz)) //pone carita feliz
  {
  variable_carita=1;
    if(aux_caritas==1)
    {
        variable_carita=2;                      //realiza la secuencia del parpadeo
       caritas();
       delay(200);
       variable_carita=1;
       caritas();
       aux_caritas=0;                         //para que no vuelva a entrar por un tiempo
       mueve_cabeza_si();                     //agrega movimiento de SI
     }
  }
  
  if(distancia<distancia_contento)        //cuando esta muy cerca se enoja
  {
  
      variable_carita=3;
      if(aux_caritas==1)
      {  
        variable_carita=4;               //realiza la secuencia del parpadeo
       caritas();
       delay(200);
       variable_carita=3;
       caritas();
       aux_caritas=0;                   //para que no vuelva a entrar por un tiempo
       
        mueve_cabeza_si();            //agrega movimiento de SI
      }
   }
}


//------------------------------FUNCION MUEVE AL AZAR--------------------------------------------

void secuencia_azar(void)
{

  if((millis()-aux_tiempo_random)>tiempo_random)     //entro cuando se cumpla el tiempo 
  {
  int variable_x_nuevo= random(minimo_x, maximo_x);    //asigno moviminetos random entre esos valores
  int variable_y_nuevo= random(minimo_y, maximo_y); 
   aux_tiempo_random=millis();
   
  while(variable_x_nuevo>variable_x)    //hace que el movimiento del azar sea mas suave
  {
    variable_x++;
    chequeo_limites();
    movimiento_servos();
    delay(5);
  }
  
  while(variable_x_nuevo<variable_x)   //hace que el movimiento del azar sea mas suave
  {
    variable_x--;
    chequeo_limites();
    movimiento_servos();
    delay(5);
  }
  
    while(variable_y_nuevo>variable_y)     //hace que el movimiento del azar sea mas suave
  {
    variable_y++;
    chequeo_limites();
    movimiento_servos();
    delay(5);
  }
  
   while(variable_y_nuevo<variable_y)    //hace que el movimiento del azar sea mas suave
  {
    variable_y--;
    chequeo_limites();
    movimiento_servos();
    delay(5);
  }
   
  } 
  
}

 
//--------------------------------MUEVE LOS SERVOS---------------------------------------------------
 
void movimiento_servos(void)
{
  
servo_eje_x.write(variable_x);    //me muevo a la sposiciones iniciales
delay(15);
servo_eje_y.write(variable_y);
delay(15); 


}

//-----------------------CHEQUEA LOS LIMITES SETEADOS ARRIBA DE TODO--------------------------------

void chequeo_limites(void)
{
         if(variable_x<=minimo_x)   //chequeo si esta por debajo del limite y lo limito al mismo
         variable_x=minimo_x;
         
         if(variable_x>=maximo_x)   //chequeo si esta por escima del limite y lo limito al mismo
         variable_x=maximo_x;
         
         if(variable_y>=maximo_y)   //chequeo si esta por escima del limite y lo limito al mismo
         variable_y=maximo_y;
         
         if(variable_y<=minimo_y)   //chequeo si esta por debajo del limite y lo limito al mismo
         variable_y=minimo_y;
}


//--------------------------------FUNCION PARA HACER LAS CARITAS-----------------------------------


void caritas(void) 
{
  
//  lc.clearDisplay(0);
  
  /* aqui dibujo las caritas en cada pixel*/
  
  byte feliz1[8]={B00000000,    //ojos abiertos
                  B01100011,
                  B11000011,
                  B10001000,
                  B10001000,
                  B11000011, 
                  B01100011,
                  B00000000};
                 
  byte feliz2[8]={B00000000,   //ojos cerrados
                  B01100010,
                  B11000010,
                  B10001000,
                  B10001000,
                  B11000010, 
                  B01100010,
                  B00000000};
                 
  byte enojado[8]={B00000000,   //enojado
                   B10000011,
                   B01000011,
                   B01001000,
                   B01001000,
                   B01000011,
                   B10000011,
                   B00000000};  
  
   byte contento1[8]={B00000000,   //contento ojos abiertos
                      B01100011,
                      B10100011,
                      B10101000,
                      B10101000, 
                      B10100011,  
                      B01100011,
                      B00000000}; 
                      
  byte contento2[8]={ B00000000,   //contento picaron
                      B01100010,
                      B10100010,
                      B10101000,
                      B10101000,
                      B10100011, 
                      B01100011,
                      B00000000}; 
   
  if(variable_carita==0)            //imprime enojado si no encuentra a nadie
 {
     lc.setRow(0,0, enojado[0]);
  lc.setRow(0,1, enojado[1]);
  lc.setRow(0,2, enojado[2]);
  lc.setRow(0,3, enojado[3]);
  lc.setRow(0,4, enojado[4]);
  lc.setRow(0,5, enojado[5]);
  lc.setRow(0,6, enojado[6]);
  lc.setRow(0,7, enojado[7]);
  delay(50);
 }
 
  if(variable_carita==1)            //imprime feliz ojos abiertos
 {
  lc.setRow(0,0,feliz1[0]);
  lc.setRow(0,1,feliz1[1]);
  lc.setRow(0,2,feliz1[2]);
  lc.setRow(0,3,feliz1[3]);
  lc.setRow(0,4,feliz1[4]);
  lc.setRow(0,5,feliz1[5]);
  lc.setRow(0,6,feliz1[6]);
  lc.setRow(0,7,feliz1[7]);
  delay(50);
 }
    
 if(variable_carita==2)            //imprime feliz ojo cerrado
 {
   
  lc.setRow(0,0,feliz2[0]);
  lc.setRow(0,1,feliz2[1]);
  lc.setRow(0,2,feliz2[2]);
  lc.setRow(0,3,feliz2[3]);
  lc.setRow(0,4,feliz2[4]);
  lc.setRow(0,5,feliz2[5]);
  lc.setRow(0,6,feliz2[6]);
  lc.setRow(0,7,feliz2[7]);
  delay(50);
 }
 
  if(variable_carita==3)          //imprime contento ojos abiertos
 {
  
  lc.setRow(0,0, contento1[0]);
  lc.setRow(0,1, contento1[1]);
  lc.setRow(0,2, contento1[2]);
  lc.setRow(0,3, contento1[3]);
  lc.setRow(0,4, contento1[4]);
  lc.setRow(0,5, contento1[5]);
  lc.setRow(0,6, contento1[6]);
  lc.setRow(0,7, contento1[7]);
  delay(50);
 }
 
   if(variable_carita==4)         //imprime contento ojo picaron
 {
  
  lc.setRow(0,0, contento2[0]);
  lc.setRow(0,1, contento2[1]);
  lc.setRow(0,2, contento2[2]);
  lc.setRow(0,3, contento2[3]);
  lc.setRow(0,4, contento2[4]);
  lc.setRow(0,5, contento2[5]);
  lc.setRow(0,6, contento2[6]);
  lc.setRow(0,7, contento2[7]);
  delay(50);
 }

}
