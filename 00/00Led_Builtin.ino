// BLOQUE 0  
// C++ code
/*
Título:
Autor: Godofredo Diaz
Fecha de modificación: Setiembre 2022
*/

// BLOQUE 1


// BLOQUE 2
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);                         //Configurando el pin LED_BUILTIN (13) en modo OUTPUT.
}

// BLOQUE 3
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);                      //Enviado un 1 digital,HIGH (5V) para encender el LED_BUILTIN.
  delay(100);                                           // 
                                                        // 
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);                                           // 
}
