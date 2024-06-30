/* Fill-in information from Blynk Device Info here */

#define BLYNK_TEMPLATE_ID "TMPL3DMGLgd6S"
#define BLYNK_TEMPLATE_NAME "Smart Streetlight Project"
#define BLYNK_AUTH_TOKEN "cH5VEqOCLf9D44N6XNsiv5YtEPOrZ5Z2"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 

// Your WiFi credentials.

char auth[] = "cH5VEqOCLf9D44N6XNsiv5YtEPOrZ5Z2";
char ssid[] = "ssl";
char pass[] = "12345678";
#define LDR_PIN A0 // Define the pin connected to LDR
#define btn D0
#define IR_PIN_1 D3 // Define the pin connected to IR sensor for LED 1
#define IR_PIN_2 D4 // Define the pin connected to IR sensor for LED 2
#define IR_PIN_3 D5 // Define the pin connected to IR sensor for LED 3
#define LED_PIN_1 D8 // Define the pin connected to LED 1
#define LED_PIN_2 D6 // Define the pin connected to LED 2
#define LED_PIN_3 D7 // Define the pin connected to LED 3
bool isAutomaticMode = true;
bool led1State = false;
bool led2State = false;
bool led3State = false;
float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;
WiFiServer server(80);
BlynkTimer timer;

// Setup a handler for the mode switch

BLYNK_WRITE(V3) {
isAutomaticMode = param.asInt() == 1;
}

// Setup handlers for manual switches for each LED

BLYNK_WRITE(V0) {
if (!isAutomaticMode) {
led1State = param.asInt() == 1;
digitalWrite(LED_PIN_1, led1State ? HIGH : LOW);
}
}
BLYNK_WRITE(V1) {
if (!isAutomaticMode) {
led2State = param.asInt() == 1;
digitalWrite(LED_PIN_2, led2State ? HIGH : LOW);
}
}
BLYNK_WRITE(V2) {
if (!isAutomaticMode) {
led3State = param.asInt() == 1;
digitalWrite(LED_PIN_3, led3State ? HIGH : LOW);
}
}
void checkLight() {
int ldrValue = analogRead(LDR_PIN);
int threshold = 500; // Adjust this threshold according to your LDR 
sensitivity
int ir1 = digitalRead(IR_PIN_1);
int ir2 = digitalRead(IR_PIN_2);
int ir3 = digitalRead(IR_PIN_3);
Serial.println(ldrValue);
Serial.println(ir1);
Serial.println(ir2);
Serial.println(ir3);
if (isAutomaticMode) {
if (ldrValue < threshold) {
if (ldrValue < threshold && !ir1) {
digitalWrite(LED_PIN_1, HIGH); // Full brightness
} else{
analogWrite(LED_PIN_1, 20-(ldrValue/500)); // Half brightness
}
if (ldrValue < threshold && !ir2) {
digitalWrite(LED_PIN_2, HIGH); // Full brightness
} else{
analogWrite(LED_PIN_2, 20-(ldrValue/500)); // Half brightness
}
if (ldrValue < threshold && !ir3) {
digitalWrite(LED_PIN_3, HIGH); // Full brightness
} else{
analogWrite(LED_PIN_3, 20-(ldrValue/500)); // Half brightness
}
}
else {
digitalWrite(LED_PIN_1, LOW); // Turn off
digitalWrite(LED_PIN_2, LOW); // Turn off
digitalWrite(LED_PIN_3, LOW); // Turn off
}
}
}
void notify()
{
int isAlert = digitalRead(btn);
Serial.println(isAlert);
if (isAlert==1) {
// Blynk.email("shameer50@gmail.com", "Alert", "Theft Alert in Home");
//Blynk.notify("Alert : Theft Alert in Home");
Blynk.logEvent("alert","Emergency");
// flag=1;
}
else if (isAlert==0)
{
// flag=0;
}
}
void setup() {
pinMode(btn, INPUT);
pinMode(IR_PIN_1, INPUT);
pinMode(IR_PIN_2, INPUT);
pinMode(IR_PIN_3, INPUT);
pinMode(LDR_PIN, INPUT);
pinMode(LED_PIN_1, OUTPUT);
pinMode(LED_PIN_2, OUTPUT);
pinMode(LED_PIN_3, OUTPUT);
Serial.begin(115200);
SerialGPS.begin(9600);
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
server.begin();
Serial.println("Server started");
Serial.println(WiFi.localIP());
Blynk.begin(auth, ssid, pass);

// Setup a virtual pin to control the mode (Automatic or Manual)

Blynk.virtualWrite(V3, isAutomaticMode ? 1 : 0);

// Setup virtual pins to control manual switches for each LED

Blynk.virtualWrite(V0, led1State ? 1 : 0);
Blynk.virtualWrite(V1, led2State ? 1 : 0);
Blynk.virtualWrite(V2, led3State ? 1 : 0);
timer.setInterval(10L, checkLight); // check LDR intensity every second
timer.setInterval(10L, notify);
}
void loop() {
Blynk.run();
timer.run();
while (SerialGPS.available() > 0)
if (gps.encode(SerialGPS.read()))
{
if (gps.location.isValid())
{
Latitude = gps.location.lat();
LatitudeString = String(Latitude , 6);
Longitude = gps.location.lng();
LongitudeString = String(Longitude , 6);
}
if (gps.date.isValid())
{
DateString = "";
date = gps.date.day();
month = gps.date.month();
year = gps.date.year();
if (date < 10)
DateString = '0';
DateString += String(date);
DateString += " / ";
if (month < 10)
DateString += '0';
DateString += String(month);
DateString += " / ";
if (year < 10)
DateString += '0';
DateString += String(year);
}
if (gps.time.isValid())
{
TimeString = "";
hour = gps.time.hour()+ 5; //adjust UTC
minute = gps.time.minute();
second = gps.time.second();
if (hour < 10)
TimeString = '0';
TimeString += String(hour);
TimeString += " : ";
if (minute < 10)
TimeString += '0';
TimeString += String(minute);
TimeString += " : ";
if (second < 10)
TimeString += '0';
TimeString += String(second);
}
}
WiFiClient client = server.available();
if (!client)
{
return;
}

//Response
        
String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE 
html> <html> <head> <title>Smart Illumination: Streetlights with safety 
instincts</title> <style>";
s += "table, th, td {border: 1px solid blue;} </style> </head> <body> 
<h1 style=";
s += "font-size:300%;";
s += " ALIGN=CENTER>Smart Illumination: Streetlights with safety 
instincts</h1>";
s += "<p ALIGN=CENTER style=""font-size:150%;""";
s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
s += "width:50%";
s += "> <tr> <th>Latitude</th>";
s += "<td ALIGN=CENTER >";
s += LatitudeString;
s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
s += LongitudeString;
s += "</td> </tr> <tr> <th>Date</th> <td ALIGN=CENTER >";
s += DateString;
s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
s += TimeString;
s += "</td> </tr> </table> ";
if (gps.location.isValid())
{
s += "<p align=center><a style=""color:RED;font-size:125%;"" 
href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
s += LatitudeString;
s += "+";
s += LongitudeString;
s += """ target=""_top"">Click here</a> to open the location in Google 
Maps.</p>";
}
s += "</body> </html> \n";
client.print(s);
delay(100);
}
