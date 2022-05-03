# BubbleMotivator
 
This script is used for a bubble machine.
An [ESP8266](https://www.amazon.de/AZDelivery-NodeMCU-Lolin-WiFi-Parent/dp/B07Z5C3KQF) is controlling the spinning of a stepper motor [Nema17 17HS4401](https://de.aliexpress.com/item/32665922113.html?spm=a2g0o.order_list.0.0.1cad5c5fp7ACJF&gatewayAdapt=glo2deu)
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
