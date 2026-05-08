#include <WiFi.h>

const char* ssid = "3717DH";
const char* password = "LuisLuis";
WiFiServer server(80);
// No me roben los datos >:(

int ledPin = 2;
String Intensidad = "127";
// Valor por defecto. Antes acomodado directamente en la parte donde se hacía el slider, pero
// ahora tiene la funcionalidad para guardar el último dato que se agregó
String ledEstado = "Apagado";

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  ledcAttach(2, 5000, 8);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if(client){
    String request = client.readStringUntil('\r');
    client.flush();

    // 1. Check for ON button
    if (request.indexOf("/ON") != -1){
      ledcWrite(2,255);
      ledEstado = "Encendido";
    }

    // 2. Check for OFF button
    if (request.indexOf("/OFF") != -1){
      ledcWrite(2,0);
      ledEstado = "Apagado";
    }

    // Actualización del Slider
    int pwmIndex = request.indexOf("?Intensidad=");
    if (pwmIndex != -1) {
      
      int startIndex = pwmIndex + 12; // El 12 es por lo de arriba, son los caracteres de "?Intensidad="
      int endIndex = request.indexOf(" ", startIndex);
      
      
      Intensidad = request.substring(startIndex, endIndex);
      
      // Obtener el pwm, basado en lo que te de Intensidad, pasarlo a entero (es un string)
      int pwm = Intensidad.toInt();
      ledcWrite(ledPin, pwm); 
      Serial.println(pwm);
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: text/html");
    client.println();

    client.println("<html><body style='font-family:Arial;text-align:center;'>");
    client.println("<h1>Control del LED</h1>");
    client.println("<p>Estado actual del LED: <strong>" + ledEstado + "</strong></p>");

    if(ledEstado == "Encendido"){
      client.println("<svg height='100' width='100'>");
      client.println("<circle cx='50' cy='50' r='40' fill='lime' stroke='black' stroke-width='3'/>");
      client.println("</svg>");
    }
    else{
      client.println("<svg height='100' width='100'>");
      client.println("<circle cx='50' cy='50' r='40' fill='red' stroke='black' stroke-width='3'/>");
      client.println("</svg>");
    }

    client.println("<h2>Intensidad del LED</h2>");
    
    
    client.println("<form action='/' method='GET'>");
    client.println("<div>");
    client.println("<input type='range' id='Intensidad' name='Intensidad' min='0' max='255' value='" + Intensidad + "'/>");
    client.println("<label for='Intensidad'>Intensidad</label>");
    client.println("</div>");
    client.println("<br>");
    
    client.println("<input type='submit' value='Actualizar Intensidad'>");
    client.println("</form>");
    
    client.println("<br><br>");
    client.println("<a href=\"/ON\">Encender LED</a><br><br>");
    client.println("<a href=\"/OFF\">Apagar LED</a><br>");
    client.stop();
  }
}
