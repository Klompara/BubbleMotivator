# BubbleMotivator
 
This script is used for a bubble machine.

# Hardware
An [ESP8266](https://www.amazon.de/AZDelivery-NodeMCU-Lolin-WiFi-Parent/dp/B07Z5C3KQF) with the help of an [A4988 driver](https://www.amazon.de/Pololu-A4988-Schrittmotortreiber/dp/B0096SHUQO) is controlling the spinning of a stepper motor [Nema17 17HS4401.](https://de.aliexpress.com/item/32665922113.html?spm=a2g0o.order_list.0.0.1cad5c5fp7ACJF&gatewayAdapt=glo2deu)
Additionally an a2212/13t 1000kv with an 30A ESC is used to blow the bubbles.

![Circuit](https://github.com/Klompara/BubbleMotivator/raw/main/Schematic_ESP8266%20Node%20MCU_2022-05-12.png)


# MQTT
The microcontroller connects to a nearby wifi in order to connect to a mqtt broker
## Control-Messages
```
start
stop
reverse
setDelay <int default:3000 min:400 slower=faster stepper>
setServoSpeed <int default:15 min:0 max:180 higher=faster servo>
```
