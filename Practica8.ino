#define LED1 2
#define LED2 4
#define LED3 5
#define BTN 18
#define POT 34
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BTN, INPUT);
  ledcAttach(LED2, 5000, 8);
  
}

void loop() {
  // BOTON
  if(digitalRead(BTN) == LOW){
    digitalWrite(LED1, HIGH);
  } else {
    digitalWrite(LED1, LOW);
  }
  
  // ADC to PWM
  int adc = analogRead(POT);
  int pwm = map(adc, 0, 4095, 0, 255);
  ledcWrite(LED2, pwm);
  
  // SERIAL
  if(Serial.available() ){
    
    char c = Serial.read();
    
    if(c == 't'){
      ledState = !ledState;
      digitalWrite(LED3, ledState);

    }

    Serial.print("ADC = ");
    Serial.println(adc);
    Serial.print("%PWM = ");
    Serial.println(pwm*100/255);

  }
}
