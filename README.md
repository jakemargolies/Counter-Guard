# Counter-Guard

An automatic cat sprayer for kitchen counters

**Project Author: Jacob Margolies**

# Problem Background

Many cat owners (myself included) have brazen pets that feel entitled to roam the surfaces upon which their human companions prepare and eat food, namely kitchen countertops. Unfortunately, our furry friends spend a good deal of their day mucking about in boxes filled with their own excrement which means those cute little paws can carry loads of not-so-cute germs. 

Currently, the cat is pretty heavily incentivized to "counter surf", as some pet owners refer to this behavior, not only because they have a chance to snatch up some stray morsels left out, but also because cats enjoy higher vantage points. While this makes for a great deal for the cat, we can't have them tracking their business all over what should be a sanitary surface. This means there needs to exist adequate disincentive for a stroll on the kitchen island. *Cue: water spray bottle.* 

The water spray bottle is the go-to choice for cat behavioral correction, but has several downsides:

  1. The user must move to the spray bottle, which most cats will learn, giving the animal an opportunity to escape. Far from disincentivizing, this merely forces the cat to be more clever with their strategy. 
  2. Many cats, especially when engaging in food-seeking behavior, will still seek out negative attention over being ignored. This can counterintuitively lead to the punished behavior becoming reinforced. 
  
In order to effectively curb this unwanted behavior, these problems must be overcome with a solution that removes the direct physical intervention from the user.

# User Needs

An effective solution must meet two central requirements:
  1. Always spray a cat with water whenever it jumps onto the countertop
  2. Never spray humans that enter the kitchen 

While there exist many DIY projects on the internet that accomplish the first function, they do not make an attempt to discern between human and animal targets. This element is critical for the solution to be practical in a home environment.

# Proposed Solution Design

The proposed design combines ideas from existing projects with knowledge acquired during the ECE 4180 coursework at Georgia Tech. 

The solution works by using a motion sensor to detect the presence of a moving object on the countertop, and spraying water at the object (cat) when the system is "armed". When the user plans on entering the kitchen, they use the interface provided on their phone to disarm the device via bluetooth. Once disarmed, the user receives a message that the system is disabled, and the screen on the device indicates that it is safe to approach. Because the system power is supplemented with a rechargeable battery used to power the sprayer pump, the device cannot be armed if the battery is too low. The user will receive a message on their phone which tells them the system cannot be armed until properly charged. As soon as the battery is charged, the user receives a message that the device is once again armable.

# Hardware Design

## Parts List

To create this system the following components are required:
  1. Mbed - LPC1768
  2. PIR motion detector
  3. LCD display - uLCD-144-G2 from 4D systems
  4. Adafruit Bluetooth low energy UART module
  5. 2N3904 Transistor, 1kΩ, and diode (for pump on/off circuit)
  7. Electric Plant Mister Spray Bottle - (https://www.amazon.com/gp/product/B087TSDMJQ/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&th=1)

## Modifying the Electric Sprayer

![image of sprayer](/images/sprayer.jpg "Electric Plant Mister")

### Inside the Sprayer

![image of sprayer](/images/disassembled.jpg "Electric Plant Mister")

Inside the sprayer there are 3 boards, a rechargeable battery, and a DC motor pump with inlet and outlet tubing.

### Main PCB

![image of sprayer](/images/main_sprayer_board.jpg "Main Board")

The main PCB has connections to the other boards, a microcontroller that governs the behavior of the device, some passive elements including four battery indicator LEDs, and a slide connector for the battery which feeds into a voltage regulator. The lowest of these battery indicator LEDs will be used as an input to the Mbed to determine when the battery has been depleted (when the LED turns off).

### Main PCB Reverse Side

![image of sprayer](/images/board_backside.jpg "Reverse of Main PCB")

On the back of the main board we can see another slide connector providing DC power to the pump. There is also a 5V connection for the charging port and two connections to something called "key".

### Button/Trigger

![image of sprayer](/images/trigger.jpg "Sprayer trigger")

Here is the board connected to the board on the pads labelled "key". It is a SPST button that established continuity between the two leads coming off of it. Here you can see that I have soldered two additional jumper cables so that the trigger can be activated programmatically using the Mbed. This is the integral interaction point with the rest of the system. The trigger works in an unintuitive way, as depicted below, so the code must go through a specific sequence of button presses and holds to get the sprayer to perform a short burst and then power off. 

![image of sprayer](/images/button_usage.png "Instructions")

### Battery

![image of sprayer](/images/battery.jpg "Sprayer Battery")

This is the rechargeable battery that comes inside the sprayer. Oddly enough, it is 3.7V. I opted to reuse all of the internal components from the sprayer in my design, but a potential option would be using a buck converter or voltage regulator to step up the 3.3V provided by the Mbed to match the 3.7V the main board needs in order to avoid using this battery, but for me it was more convenient to leave it in since I needed supplemental power anyway.

## Display

While the two user visible device states could be represented with LEDs, I find that the mini LCD makes the device more enjoyable to use and provides good room for expanding user output without needing additional hardware. A future design with more complex functionality could have a navigable menu, status messages, and error codes displayed. 

## Motion Detector

Like many similar projects, I opted to use a PIR motion detector for sensing movement in the general vicinity in front of the device. The original design of this project (which the TA's predicted was too ambitious) involved using computer vision on a Raspberry Pi to classify the object as a cat or not a cat. Then the sprayer would track the cat through space and aim at the cat while spraying. This is outside of my current knowledge and abilities but could be added on later when I learn how to work with computer vision. 

## Bluetooth Module

The bluetooth module allows for the device and the user's phone to communicate with each other, enabling control of the devices state as well as feedback from the device to the user about the status of the device. 

## Block Wiring Diagram 

![Wiring Diagram](/images/Block%20Diagram.png "Wiring Diagram")

As can be seen there is a bit of extra circuitry added to control the trigger mechanism. To trigger the sprayer programmatically, the two contacts of the trigger button are placed on the collector and emitter of a basic NPN transistor and activated through a 1kΩ resistor driven by a DigitalOut pin on the mbed. 

# Software Design

## Block Diagram of States and Functions

![State Diagram](/images/Drawing.png "State Machine")

There are a few things not included in the diagram shown here. 
  1. The "armable" flag is set to true and false by an interrupt routine triggered by the lowest battery indicator LED. If the light goes out, the device can no longer be armed, because there is no power to the motor from the battery. If the light comes on, the device becomes armable again as the battery has charge. 
  2. There is an interrupt routine that is triggered by user input on the bluetooth serial port. It parses the input and sets the "button_ready" flag so the main function knows to check for user input and change state accordingly. This is how the arm() and disarm() functions are called.

# Prototype Demonstration

https://youtu.be/aVsBjvqTC80
