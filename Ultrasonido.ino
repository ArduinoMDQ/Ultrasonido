#include <Arduino_FreeRTOS.h>

// define taask
void TaskBlink( void *pvParameters );
void TaskSensor( void *pvParameters );
void TaskSerie( void *pvParameters );

const int Trig = 11;
const int Echo = 12;
const int Led = 2;
const int Rs485 = 5;
String Id="1";
int MuestraTiempo = 100;
static boolean control=false;
static boolean ocupado=false;
long duracion;
static long distancia;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;

void setup() {
  
  Serial.begin(9600);
  
  pinMode(Trig,OUTPUT);
  pinMode(Echo,INPUT);
  pinMode(Led, OUTPUT);
  pinMode(Rs485, OUTPUT);
  digitalWrite(Led, HIGH);
  delay(100);
  digitalWrite(Led,LOW );
  digitalWrite(Rs485, LOW);
  inputString.reserve(50);

  xTaskCreate(TaskSerie,(const portCHAR *)"Serie"
    ,  128  // Stack size
    ,  NULL
    ,  1  // priority
    ,  NULL );
  
  xTaskCreate(TaskBlink,(const portCHAR *)"Blink"
    ,  128  // Stack size
    ,  NULL
    ,  1  // priority
    ,  NULL );

  xTaskCreate(TaskSensor,(const portCHAR *)"Sensor"
    ,  128  // Stack size
    ,  NULL
    ,  1 // priority
    ,  NULL );

  EnviarOK();
}

void loop() {
}


void TaskSerie(void *pvParameters){
 while(true){
  if(Serial.available()>0) {
    char inChar = (char)Serial.read();
    if ((inChar != '\r')) {
       inputString += inChar;
    }
    if (inChar == '\r') {
      if(Id.equals(inputString)){
      
       if(digitalRead(Led)){
        EnviarOcupado();
        }else{
          EnviarLibre();}
      }
      stringComplete = true;
    
    }
  }
  
  if (stringComplete){
     
       inputString = "";
       stringComplete = false;
   }

   
  }
}

void TaskSensor(void *pvParameters){
 while(true){
    digitalWrite(Trig,LOW);
    delayMicroseconds(4);
    digitalWrite(Trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig,LOW);

    duracion= pulseIn(Echo,HIGH);
    //duracion = duracion/2;
    distancia=duracion/58;
   // Serial.println(distancia);   
    vTaskDelay( (MuestraTiempo) / portTICK_PERIOD_MS ); // wait for one second
    
    if(distancia<200){
      control=true;
      }else{
        control=false;
        }
  }
}

void TaskBlink(void *pvParameters){
  while(true){
      while(control){ 
        if(distancia<50){       
            digitalWrite(Led,true);
           
        }else{
            if(distancia<100){       
                digitalWrite(Led, !digitalRead(Led));
                vTaskDelay( (100) / portTICK_PERIOD_MS ); // wait for one second
            }else{
                if(distancia<200){       
                    digitalWrite(Led, !digitalRead(Led));
                    vTaskDelay( (300) / portTICK_PERIOD_MS ); // wait for one second
                }
            }
        }
      }
      digitalWrite(Led, LOW);
       
 }
}

void EnviarOK(){
  
   digitalWrite(Rs485,HIGH);
   delay(5);
   Serial.println(Id+";READY");
   delay(30);
   digitalWrite(Rs485,LOW);
  
  
  }


void EnviarOcupado(){
  
   digitalWrite(Rs485,HIGH);
   delay(5);
   Serial.println("BUSY");
   delay(30);
   digitalWrite(Rs485,LOW);
  
  
  }

void EnviarLibre(){
  
   digitalWrite(Rs485,HIGH);
   delay(5);
   Serial.println("FREE");
   delay(30);
   digitalWrite(Rs485,LOW);
  
  
  }


