int valor=0;

void setup(){
  Serial.begin(9600);
  Serial.println("Ingrese un valor");
}

void loop(){
 while (Serial.available()>0){
   valor = Serial.read();
   
   if(valor=='1'){
     Serial.println("Valor 1");
   }
 
 }
