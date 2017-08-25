#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include "Timer.h"

// Set these to run example.
#define FIREBASE_HOST "" //Here it goes Firebase HOST, it's something like example.firebaseio.com
#define FIREBASE_AUTH "" //Here it goes Firebase AUTH
#define WIFI_SSID "" //Here it goes the WIFI SSID
#define WIFI_PASSWORD "" //Here it goes the WIFI password

Timer timer;
Timer timerSleep;

boolean coffeeMakerReady;
boolean turnOn;

uint8_t coffeeMaker;
uint8_t sensorLVL2;
uint8_t sensorLVL0;

long currentTime;
long startTurnOn;
long startSleep;

int timerEvent;
int timerSleepEvent;

void setup() {
  
  coffeeMaker = D0;
  sensorLVL0 = D1;
  sensorLVL2 = D2;

  //Events' ID are 0 and 1, so then it uses any other number to identify the stopped event  
  //Los ID  de los eventos son 0 y 1, asi que se utiliza otro numero para identificar evento detenido
  timerEvent = 10;

  //Coffee maker
  //Cafetera
  pinMode(coffeeMaker, OUTPUT);
  //Sensor lvl 0
  pinMode(sensorLVL0, INPUT);
  //Sensor lvl 3
  pinMode(sensorLVL2, INPUT);
  
  Serial.begin(115200);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Turn off coffee maker by default
  //Apagar cafetera por default
  digitalWrite(coffeeMaker, HIGH);

  //It sets up that the coffee maker is ready
  //Se establece que la cafetera está lista
  coffeeMakerReady = true;

  //It sets up that the coffee maker is off
  //Se establece que la cafetera está apagada
  turnOn = false;

  //It publishs that the coffee maker is available to turn on
  //Se publica disponibilidad de la cafetera
  publishCoffeeMakerReady();

  //It publishs the default value of the timer
  //Se publica el valor por defecto del timer
  initTimer();
  
}

void loop() {

  //It publishs the starting water level with the liquid sensors
  //Se publica el nivel de agua inicial con los sensores
  measureWaterLevel();

  //It gets from Firebase whether it's ordered to turn on the coffee maker
  //Se lee en Firebase si se ordena encender la cafetera
  turnOn = Firebase.getBool("turnOn");
    
  Serial.print("turnOn: ");
  Serial.println(turnOn);
  
  if(coffeeMakerReady) {
    //It turns on the coffee maker if so it's ordered and there is enough cold water
    //Se enciende si se le ordena encender y si tiene agua suficiente
    if(turnOn && measureWaterLevel() > 0) {
      //It sets up and publishs that the coffee maker will be in use from now on
      //Se publica que la cafetera estará en uso a partir de ahora
      coffeeMakerReady = false;      
      publishCoffeeMakerReady();

      //It publishs that the coffee is not ready yet
      //Se publica que el cafe no esta listo todavia
      publishCoffeeReady(false);

      //It turns on the coffee maker
      //Se enciende cafetera
      digitalWrite(coffeeMaker, LOW);

      //It starts to run the timer (the coffee maker can be kept on for 1 hour at most) and the preparation stage begins
      //Empieza a correr el timer (máximo puede estar encendido 1 hora) y comienza fase de preparacion;
      startTurnOn = millis();
      timerEvent = timer.every(1000, preparationBlock);    
    }     
  } else if (timerEvent == 0) {
      checkTimerEvent();   
  }
  //It updates the timers    
  //Se actualizan timers
  timer.update();
  timerSleep.update();   
  delay(150);
}

void publishCoffeeMakerReady(){
  Firebase.setBool("coffeeMakerReady", coffeeMakerReady);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /coffeeMakerReady failed:");
    Serial.println(Firebase.error());  
  }  
}

int measureWaterLevel() {
  int sensorLVL0Value = digitalRead(sensorLVL0);
  int sensorLVL2Value = digitalRead(sensorLVL2);
  
  int waterLevel = 0;
  if(sensorLVL2Value == LOW) {
    waterLevel = 2;    
  } else if(sensorLVL0Value == HIGH) {
    waterLevel = 0;
  }   
      
  Firebase.setInt("waterLevel", waterLevel);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /waterLevel failed:");
    Serial.println(Firebase.error());  
  }

  return waterLevel;
}

void initTimer() {
  Firebase.setString("timer", "00:00");
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /timer failed:");
    Serial.println(Firebase.error());  
  }  
}

void preparationBlock() {
  currentTime = millis();
  long waitingTime = currentTime - startTurnOn;
  long waitingHour = waitingTime / 1000 / 60 / 60;
  long waitingMinute = waitingTime / 1000 / 60 % 60;
  long waitingSecond = waitingTime / 1000 % 60;
  String timeString = String(waitingMinute) + ":" + String(waitingSecond);
  
  Firebase.setString("timer", timeString);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /timer failed:");
    Serial.println(Firebase.error());  
  }

  //It publishs the water level and if the water tank has drained, it publishs that the coffee is ready
  //Se publica el nivel de agua y si el agua se termino el cafe esta listo
  if(measureWaterLevel() == 0) {
      publishCoffeeReady(true);  
  }
}

void checkTimerEvent() {
  currentTime = millis();

  //It stops the timer and it turns off the coffee maker if it has been kept on for 1 hour or the user wants to turn it off
  //Se detiene el timer y se apaga la cafetera si se llega a 1 hora o el usuario quiere apagar
  if(!turnOn) {
    stopTimerEvent();   
  } else if(currentTime - startTurnOn > 1000 * 60 * 60) {
    turnOn = false;
    Firebase.setBool("turnOn", turnOn);
    // handle error
    if (Firebase.failed()) {
      Serial.print("setting /turnOn failed:");
      Serial.println(Firebase.error());  
    }
    stopTimerEvent();  
  }
}

void stopTimerEvent() {
  //CoffeeReady's value comes back to his idle state
  //El valor de coffeeReady vuelve a su valor por defecto
  publishCoffeeReady(false);

  //Stopping timer's event
  //Detener evento del timer
  timer.stop(timerEvent); 

  //Turning off the coffee maker
  //Apagar cafetera  
  digitalWrite(coffeeMaker, HIGH);

  //Events' ID are 0 and 1, so then it uses any other number to identify the stopped event 
  //Los ID  de los eventos son 0 y 1, asi que se utiliza otro numero para identificar evento detenido
  timerEvent = 10;

  //It publishs the reset of the timer
  //Se publica el reset del timer
  initTimer();

  //The coffee maker sleeps for 5 minutes in order to be available to turn on again
  //Tiempo de descanso de 5 minutos de la cafetera para que este disponible de nuevo
  startSleep = millis();
  timerSleepEvent = timerSleep.every(1000 * 60, sleepBlock); 
  timerSleep.after(1000 * 60 * 5, stopTimerSleepEvent); 
} 

void sleepBlock() {
  currentTime = millis();
  long waitingTime = currentTime - startSleep;
  long waitingMinute = waitingTime / 1000 / 60;
  
  String timeString = waitingMinute < 4 ? String(5 - waitingMinute) : "< 1";
  publishTimerSleep(timeString);
}

void publishTimerSleep(String timeString) {
  Firebase.setString("timerSleep", timeString);
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /timerSleep failed:");
    Serial.println(Firebase.error());  
  }
}

void stopTimerSleepEvent() {
  currentTime = millis();
   
  if(currentTime - startSleep > 5 * 60 * 1000) { 
    timerSleep.stop(timerSleepEvent);

    //It sets up that the coffee maker is ready
    //Se establece que la cafetera está lista
    coffeeMakerReady = true;

    //It publishs the coffee maker's availability
    //Se publica disponibilidad de la cafetera
    publishCoffeeMakerReady();   

    //The sleep time comes back to his default value
    //El tiempo de descanso vuelve a su valor por defecto
    publishTimerSleep(String(5));
  }    
}

void publishCoffeeReady(boolean coffeeReady) {    
  Firebase.setBool("coffeeReady", coffeeReady);
  
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /coffeeReady failed:");
    Serial.println(Firebase.error());  
  }
}

