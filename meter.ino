#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <RotaryEncoder.h>
LiquidCrystal_I2C lcd(0x27, 6,12);
RotaryEncoder encoder(A2, A3);
#define ROTARYMIN 0
int ROTARYMAX = 9;
int mode_button=2;
float pwr=0;
int pwr_adc = 0;
unsigned int att=0;
int mode=0; // 0 for set frequency and 1 for set attenuator
int freq_sel=0;
char *freq[10]={"30 MHz","50 MHz","144MHz","433MHz","1.2GHz","1.8GHz","2.1GHz","2.4GHz","3.4GHz","5.7GHz"};
int matrix[10][4] = {
  {0,1023,0,50},// for 30MHz
  {0,1023,0,50},// for 50MHz
  {0,1023,0,50},// for 144MHz
  {0,1023,0,50},// for 433MHz
  {0,1023,0,50},// for 1,2GHz
  {0,1023,0,50},// for 1,8GHz
  {0,1023,0,50},// for 2,1GHz
  {0,1023,0,50},// for 2,4GHz
  {0,1023,0,50},// for 3,4GHz
  {0,1023,0,50},// for 5,7GHz
};

void setup() {
  Serial.begin(9600);
  pinMode(mode_button, INPUT_PULLUP);
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("RF Power");
  lcd.setCursor(1,1);
  lcd.print("1 MHz - 8 GHz");
  delay(500);
  attachInterrupt(digitalPinToInterrupt(mode_button), mode_interrupt, RISING);
  encoder.setPosition(freq_sel);
}

void mode_interrupt()
{
  detachInterrupt(mode_interrupt);
  
  if (mode == 0) {
    mode = 1;
    encoder.setPosition(att);
    ROTARYMAX = 100;
    
  }
  else if (mode == 1){
    mode = 0;
    encoder.setPosition(freq_sel);
    ROTARYMAX = 9;
  }
  attachInterrupt(digitalPinToInterrupt(mode_button), mode_interrupt, RISING);
}

void update_disp(){
  String pwr_s = String(pwr);
  float pwr_w = pow(10.0,(pwr)/10.0);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(freq[freq_sel]);
  if (mode == 0){
    lcd.setCursor(0,0);
    lcd.print(">");
  }
  else if ( mode == 1){
    lcd.setCursor(9,0);
    lcd.print(">");
  }
  lcd.setCursor(10,0);
  lcd.print("Att:");
  lcd.print(att);
  lcd.setCursor(0,1);
  lcd.print("P:");
  lcd.print(pwr_s[0]);
  lcd.print(pwr_s[1]);
  lcd.print(pwr_s[2]);
  lcd.print(pwr_s[3]);
//  lcd.print(pwr_s[4]);
  lcd.print("dBm/");

  if (pwr_w > 999 ){
    pwr_w = pwr_w / 1000;
    String pwr_w_s = String(pwr_w);  
    lcd.print(pwr_w_s[0]);
    lcd.print(pwr_w_s[1]);
    lcd.print(pwr_w_s[2]);
    lcd.print(pwr_w_s[3]);
    lcd.print("W");
  }
  else {
    String pwr_w_s = String(pwr_w);  
    lcd.print(pwr_w_s[0]);
    lcd.print(pwr_w_s[1]);
    lcd.print(pwr_w_s[2]);
    lcd.print(pwr_w_s[3]);
    lcd.print("mW");
  }
  Serial.println(pwr);
  Serial.println(pwr_w);
}

int measure(){
  float pwr1=0;
  pwr_adc =  analogRead(A5);
  pwr1 = map(pwr_adc, matrix[freq_sel][0], matrix[freq_sel][1], matrix[freq_sel][2], matrix[freq_sel][3]);
  return pwr1;
}

//int encoder_update(){
//    encoder.tick();
//
//  if (mode == 0){
//    freq_sel = encoder.getPosition();
//  }
//  else if ( mode == 1){
//    att = encoder.getPosition();
//  }
//}

void loop() {
  pwr = measure();
  update_disp();
  encoder.tick();

  if (mode == 0){
    freq_sel = encoder.getPosition();
  }
  else if ( mode == 1){
    att = encoder.getPosition();
  }
  delay(100);
}






