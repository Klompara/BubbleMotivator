# BubbleMotivator
 
This script is used for a bubble machine.

# Hardware
An [ESP8266](https://www.amazon.de/AZDelivery-NodeMCU-Lolin-WiFi-Parent/dp/B07Z5C3KQF) with the help of an [A4988 driver](https://www.amazon.de/Pololu-A4988-Schrittmotortreiber/dp/B0096SHUQO) is controlling the spinning of a stepper motor [Nema17 17HS4401](https://de.aliexpress.com/item/32665922113.html?spm=a2g0o.order_list.0.0.1cad5c5fp7ACJF&gatewayAdapt=glo2deu)
![alt text](https://939506.smushcdn.com/2600043/wp-content/uploads/2/2/1/5/22159166/nodemcu-esp8266-12-e-controlling-stepper-motor-over-wifi_orig.png?lossy=0&strip=1&webp=1)

# REST API
The microcontroller connects to a nearby wifi in order to run a http server which can be used to control the bubble machine.
## start bubbles
### Request
`POST /start/`
```curl
curl --location --request POST 192.168.178.40/start
```
### Response
```
started
```
## stop bubbles
### Request
`POST /stop/`
```curl
curl --location --request POST 192.168.178.40/stop
```
### Response
```
stopped
```
## reverse spin direction
### Request
`POST /reverse/`
```curl
curl --location --request POST 192.168.178.40/reverse
```
### Response
```
motor spinning counterclockwise
motor spinning clockwise
```
