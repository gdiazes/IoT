// BLOQUE 0  
/*
Título:Pulsador - Led
Autor: Godofredo Diaz
Fecha de modificación: Setiembre 2022
*/

// BLOQUE 1
const int inPulsador = 4;                   //Configurar el pin 4 con el nombre inPulsador
const int ouL1 = 7;                         //Configurar el pin 7 con el nombre ouL1
const int ouL2 = 8;                         //Configurar el pin 8 con el nombre ouL2
int estado =0;                              //Configurar la variable estado en 0

// BLOQUE 2
void setup()
{
  pinMode(inPulsador, INPUT_PULLUP);        //Seteando el inPulsador en modo INPU_PULLUP
  pinMode(ouL1, OUTPUT);                    //Seteando ouL1 en modo OUTPUT
  pinMode(ouL2, OUTPUT);                    //Seteando ouL2 en modo OUTPUT
  Serial.begin(9600);                       //Seteando la conexión al puerto serial a una velocidad de 9600 baudios.
  digitalWrite(inPulsador,LOW);             //Escribiendo LOW en inPulsador
  digitalWrite(ouL1,LOW);                   //Apagando el Led conectado a ouL1
  digitalWrite(ouL2,LOW);                   //Apagando el Led conectado a ouL2
}

// BLOQUE 3
void loop()
{
  estado = digitalRead(inPulsador);         //Sensando el estado del pulsador inPulsador
  digitalWrite(ouL1,estado);                //Enviando el estado del pulsador al led ouL1
  digitalWrite(ouL2,estado);                //Enviando el estado del pulsador al led ouL2
  Serial.println(estado);                   //Imprimiendo en el monitor serial el estado del pulsador inPulsador.
  delay(1000);                              //Hacer una pausa de 1000 milisegundos.
}

