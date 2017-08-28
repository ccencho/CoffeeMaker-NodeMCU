# CoffeeMaker

This is part of a project to manage a coffee maker at home from a Android mobile phone. It's made up of an Android app written in Java as the front-end, Firebase service as the back-end and NodeMCU as a microcontroller written  in C/C++ and integrated to a coffee maker with water level sensors.

The coffee maker can be turned on/off by the Android app. Nonetheless, it can only be kept turned on maximum of 1 hour. If such an amount of time has passed, it automatically turns off and it rests up by 5 minutes in order to be available of turning on again.

Also, there is a couple of float ball liquid sensors in the cold water tank monitoring the water levels. If there is not enough water, the coffee maker cannot be turned on by the Android app. So then, it shows an error message on this issue.

## Project arquitecture

![Project arquitecture](https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/arquitecture%20project.png)

-	[CoffeeMaker-NodeMCU project](https://github.com/ivanph1017/CoffeeMaker-NodeMCU)
-	[CoffeeMaker-Android project](https://github.com/ivanph1017/CoffeeMaker-Android)

<div style="width:90%;margin:auto;">
    <img style="margin:20px;" src="https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/intro.gif" alt="Intro" height="480" width="288"/>
    <span>
        <img style="margin:20px;"
        src="https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/waterFilling.gif" alt="Water filling" height="480" width="288"/>
    </span>
    <span>
        <img style="margin:20px;"
        src="https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/preparingCoffee.gif" alt="Preparing coffee" height="480" width="288"/>
    </span>
</div>

<div style="width:90%;margin:auto;">
    <img style="margin:20px;" src="https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/coffeeReady.gif" alt="Coffee ready" height="480" width="288"/>
    <span>
        <img style="margin:20px;"
        src="https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/coffeeMakerResting.gif" alt="Coffee ready" height="480" width="288"/>
  </span>
</div>

## CoffeeMaker-NodeMCU

### Wiring

The full list of devices required are below:

-	Android smartphone
-	NodeMCU V1.0
-	2-relay module
-	Coffee maker
-	Breadboard
-	2 float ball liquid sensor
-	2 resistors of 10k ohms
-	Many jump wires with female and male header connectors
-	Many jump wires with female and female header connectors
-	Many jump wires with male and male header connectors

Then, the circuits should look like the image below:

![Circuit](https://raw.githubusercontent.com/ivanph1017/AssetsRepo/master/CoffeeMaker/circuit.png)

The 2-module relay is like our switch, it can open or close the electric circuit of the coffee maker. In other words, it's the device that turns on/off our machine. For more details of how a relay works, follow the next link:

-	![Introduction to relays (Spanish)](https://www.inventable.eu/introduccion-a-los-reles/)

### Setting up

The libraries used in this project are below:

-	[Firebase-Arduino](https://github.com/firebase/firebase-arduino)
-	ESP8266WiFi
- 	[Timer](https://playground.arduino.cc/Code/Timer) by Simon Monk

First of all, it's necessary to install NodeMCU drivers in order to be recognized by our OS. Therefore, it's  thoroughly recommended to follow the steps detailed here:

-	[How to setup NodeMCU drivers and Arduino IDE](https://www.marginallyclever.com/2017/02/setup-nodemcu-drivers-arduino-ide/)

Then, it's time to set up the others libraries. Here's a complete guide detailed step by step to add Firebase library: 

-	[Firebase-demo](https://github.com/firebase/firebase-arduino/tree/master/examples/FirebaseDemo_ESP8266)

However, according to the issue #276 addressed in Firebase-Arduino, it's  thoroughly recommended to download the zip library from this fork 

-	[RaemondBW/firebase-arduino](https://github.com/RaemondBW/firebase-arduino)

Finally, it follows the same method of adding the zip Firebase-Arduino library to add

-	[Timer library](https://github.com/JChristensen/Timer)
