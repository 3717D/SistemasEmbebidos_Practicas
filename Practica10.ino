#include <WiFi.h>
#include <ESP32Servo.h>

const char* red = "3717_ESP32";
const char* contra = "ServoLED";
//ip y puerto = 192.168.4.1 8080

WiFiServer server(8080);

static const int sMotor = 13;
#define LED 2
Servo myservo;

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  // Parece que esto es necesario para separar el timer del Servo y el timer del LED.
  
  myservo.setPeriodHertz(50);
  myservo.attach(sMotor, 500, 2400);
  // Al servo se le tiene que limitar a 50 Hz.
  ledcAttach(LED, 5000, 8);
  
  WiFi.softAP(red, contra);
  Serial.println("Red creada");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();

}

void loop() {
  WiFiClient client = server.available();
  
  if(client){
    Serial.println("Cliente conectado");

    while(client.connected()){
      
      if(client.available()){
        String data = client.readStringUntil('\n');
        data.toUpperCase();
        data.trim();
        /*
        Serial.print("Recibido: ");
        Serial.println(data);
        */
        int pwm;
        int angulo;
        
        if(data.startsWith("LED")){
          pwm = data.substring(3).toInt();
          if(pwm >= 0 && pwm <=255){
            ledcWrite(LED, pwm);
            client.print("PWM: ");
            client.println(pwm);
          }
        }else if(data.startsWith("SRV")){
          angulo = data.substring(3).toInt();
          if(angulo >= 0 && angulo <= 180){
            myservo.write(angulo);
            client.print("Angulo: ");
            client.println(angulo);
          }
        }else if(data.startsWith("OFF")){
          ledcWrite(LED, 0);
          client.println("LED apagado");
        
        }else if(data.startsWith("MAX")){
          ledcWrite(LED,255);
          client.println("LED encendido");
        }else{
          client.println("Valor o comando incorrecto");
        }
      }
    }

    client.stop();
    Serial.println("Cliente desconectado");
  }
}
