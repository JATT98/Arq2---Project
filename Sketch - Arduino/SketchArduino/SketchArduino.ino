#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>  
 
#define DHT11_PIN 18
#define DS18B20 5
#define PERIODO 1000
 
float temp, humedad, BPM, SpO2, temperatura;
 
const char* ssid = "";     // ingresar el nombre de la red
const char* password = "";  // ingresar la contraseña
 
dht DHT;
PulseOximeter pox;
uint32_t reporte = 0;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);
 
 
WebServer server(80);             
 
void Latido()
{
  Serial.println("¡Latido!");
}
 
void setup() {
  
  Serial.begin(115200);
  pinMode(19, OUTPUT);
  delay(100);   
 
  Serial.println("Conectándose a ");
  Serial.println(ssid);

  //Se inicia la conexión a la Red
  WiFi.begin(ssid, password);

  //Se comprueba la conexión a la Red
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }

  //Se imprime la dirección a la cual se esta conectado
  Serial.println("");
  Serial.println("¡WiFi conectado..!");
  Serial.print("IP: ");  Serial.println(WiFi.localIP());
 
  server.on("/", Conexion);
  server.onNotFound(NotFound);

  
  server.begin();
  Serial.println("HTTP server iniciado");
 
  Serial.print("Iniciando el oxímetro...");
 
  if (!pox.begin()) {
    Serial.println("FALLIDO");
    for (;;);
  } else {
    Serial.println("EXITOSO");
    pox.setOnBeatDetectedCallback(Latido);
  }
 
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  
}
void loop() {
  server.handleClient();
  pox.update();
  sensors.requestTemperatures();
  int chk = DHT.read11(DHT11_PIN);

  //Se leen los datos de cada sensor
  temp = DHT.temperature;
  humedad = DHT.humidity;
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  temperatura = sensors.getTempCByIndex(0);
 
  
  if (millis() - reporte > PERIODO) 
  {
    //Se imprimen los datos almacenados
    Serial.print("Temperatura: ");
    Serial.print(DHT.temperature);
    Serial.println("°C");
    
    Serial.print("Húmedad: ");
    Serial.print(DHT.humidity);
    Serial.println("%");
    
    Serial.print("BPM: ");
    Serial.println(BPM);
    
    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
 
    Serial.print("Temperatura Corporal: ");
    Serial.print(temperatura);
    Serial.println("°C");
    
    Serial.println("*********************************");
    Serial.println();
 
    reporte = millis();
  }
  
}
 
void Conexion() {
  
  server.send(200, "text/html", SendHTML(temp, humedad, BPM, SpO2, temperatura)); 
}
 
void NotFound(){
  server.send(404, "text/plain", "Not found");
}
 
  String SendHTML(float temp,float humedad,float BPM,float SpO2, float temperatura){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>Monitor ESP32</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0' charset=utf-16>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center; color: #444444; }";
  ptr +="body { margin: 0px; }";
  ptr +="h1 { margin: 50px auto 30px; }";
  ptr +=".box { display: table-cell; vertical-align: middle; position: relative; }";
  ptr +=".text { font-weight: 600; font-size: 19px; width: 200px; }";
  ptr +=".reading { font-weight: 300; font-size: 50px; padding-right: 25px; }";
  ptr +=".temp .reading { color: #ff5242; }";
  ptr +=".humedad .reading { color: #0086cd; }";
  ptr +=".BPM .reading { color: #00c7ff; }";
  ptr +=".SpO2 .reading { color: #00c7ff; }";
  ptr +=".temperatura .reading { color: #00b6eb; }";
  ptr +=".superscript { font-size: 17px; font-weight: 600; position: absolute; top: 10px; }";
  ptr +=".data { padding: 10px; }";
  ptr +=".container { display: table; margin: 0 auto; }";
  ptr +=".icon { width: 65px }";
  ptr +="</style>";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1>MONITOR DEL PACIENTE</h1>";
  ptr +="<div class='container'>";
  ptr +="<div class='data temperatura'>";
  ptr +="<div class='box icon'>";
  ptr +="<img src='https://assets.dryicons.com/uploads/icon/svg/9588/0a975bb8-ef48-40be-a4de-7a8e2b0f414e.svg'>";
  ptr +="</div>";
  ptr +="<div class='box text'>Temperatura</div>";
  ptr +="<div class='box reading'>";
  ptr +=(int)temp;
  ptr +="<span class='superscript'>&deg;C</span>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="<div class='data humedad'>";
  ptr +="<div class='box icon'>";
  ptr +="<img src='https://assets.dryicons.com/uploads/icon/svg/9579/fd1261bf-771d-49e7-993f-d771a81696d7.svg'>";
  ptr +="</div>";
  ptr +="<div class='box text'>Húmedad</div>";
  ptr +="<div class='box reading'>";
  ptr +=(int)humedad;
  ptr +="<span class='superscript'>%</span>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="<div class='data BPM'>";
  ptr +="<div class='box icon'>";
  ptr +="<img src='https://assets.dryicons.com/uploads/icon/svg/12146/f78ab3ce-30d4-45af-89a2-6d5b5c154724.svg'>";
  ptr +="</div>";
  ptr +="<div class='box text'>Ritmo cardíaco</div>";
  ptr +="<div class='box reading'>";
  ptr +=(int)BPM;
  ptr +="<span class='superscript'>BPM</span>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="<div class='data SpO2'>";
  ptr +="<div class='box icon'>";
  ptr +="<img src='https://assets.dryicons.com/uploads/icon/svg/12148/c8aa521e-7fb4-4f86-9bb4-3cd1268d8849.svg'>";
  ptr +="</div>";
  ptr +="<div class='box text'>Oxígenación Sanguínea</div>";
  ptr +="<div class='box reading'>";
  ptr +=(int)SpO2;
  ptr +="<span class='superscript'>%</span>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="<div class='data temp'>";
  ptr +="<div class='box icon'>";
  ptr +="<img src='https://assets.dryicons.com/uploads/icon/svg/9589/24490b47-fdbb-4043-a638-23b7dff618bf.svg'>";
  ptr +="</div>";
  ptr +="<div class='box text'>Temperatura Corporal</div>";
  ptr +="<div class='box reading'>";
  ptr +=(int)temperatura;
  ptr +="<span class='superscript'>&deg;C</span>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}
