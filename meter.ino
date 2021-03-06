//License: CC-BY-NC-SA
//Author: Szymon Reiter "canis_lupus"

#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <Encoder.h>
#include <EEPROM.h>
LiquidCrystal_I2C lcd(0x27, 6,12);
Encoder encoder(A2, 3);
int mode_button=2;
float pwr=0;
float pwr_set=0;
float xa=0; // for calculation of coefficient of linear function from two calibration points
float ya=0; // for calculation of coefficient of linear function from two calibration points
float xb=0; // for calculation of coefficient of linear function from two calibration points
float yb=0; // for calculation of coefficient of linear function from two calibration points
int calibration_mode=0;
int pwr_adc[4] = {0,0,0,0};
int att=0;
int mode=0; // 0 for set frequency and 1 for set attenuator
int calib_mode=0; // 0 for select band and 1 for set power level
int freq_sel=0;
char line0[17]; 
char line1[17];
char *freq[10]={"30 MHz","50 MHz","144MHz","433MHz","1.2GHz","1.8GHz","2.1GHz","2.4GHz","3.4GHz","5.7GHz"};
int matrix[10][4] = {
  {0,1023,-30,15},// for 30MHz
  {0,1023,-30,15},// for 50MHz
  {0,1023,-30,15},// for 144MHz
  {0,1023,-30,15},// for 433MHz
  {0,1023,-30,15},// for 1,2GHz
  {0,1023,-30,15},// for 1,8GHz
  {0,1023,-30,15},// for 2,1GHz
  {0,1023,-30,15},// for 2,4GHz
  {0,1023,-30,15},// for 3,4GHz
  {0,1023,-30,15},// for 5,7GHz
};

void setup() {
  Serial.begin(19200);
  pinMode(mode_button, INPUT_PULLUP);
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("RF Power meter");
  lcd.setCursor(2,1);
  lcd.print("canis_lupus");
  delay(1000);
  lcd.setCursor(2,1);
  lcd.print("1MHz - 8GHz");
  delay(1500);
  attachInterrupt(digitalPinToInterrupt(mode_button), mode_interrupt, RISING);
  encoder.write(freq_sel);
  lcd.clear();
  if (mode_button == 0) {
    calibration_mode=1;
  }
}



void mode_interrupt(){
  detachInterrupt(digitalPinToInterrupt(mode_button));
  if (calibration_mode == 0){
    if (mode == 0) {
     mode = 1;
     encoder.write(pwr_set*4);
     
   }
    else if (mode == 1){
      mode = 0;
      encoder.write(freq_sel*4);
   }
  }

  else {
     if (mode == 0) {
     mode = 1;
     encoder.write(att*4);
     
   }
    else if (mode == 1){
      mode = 0;
      encoder.write(freq_sel*4);
   }
  }
  attachInterrupt(digitalPinToInterrupt(mode_button), mode_interrupt, RISING);
}

void update_disp1(){
  
  char cur1;
  char cur2;
  
  if (mode == 0){
    cur1 = '>';
    cur2 = ' ';
  }
  else if ( mode == 1){
    cur1 = ' ';
    cur2 = '>';
  }
  sprintf(line0,"%c %s %cAtt:%i" , cur1, freq[freq_sel], cur2, att);
  lcd.setCursor(0,0);
  lcd.print(line0);
}

void update_disp2(){
  char cur1;
  char cur2;
  if (mode == 0){
    cur1 = '>';
    cur2 = ' ';
  }
  else if ( mode == 1){
    cur1 = ' ';
    cur2 = '>';
  }
  sprintf(line0,"%c %s %cAtt:%i" , cur1, freq[freq_sel], cur2, att);

  String pwr_s = String(pwr,5);
  float pwr_w = pow(10.0,(pwr)/10.0);
  if (pwr_w > 999 ){
    pwr_w = pwr_w / 1000;
    String pwr_w_s = String(pwr_w,4);  
    sprintf(line1,"%c%c%c%c%cdBm %c%c%c%c%c%cW" ,pwr_s[0], pwr_s[1], pwr_s[2], pwr_s[3], pwr_s[4], pwr_w_s[0], pwr_w_s[1], pwr_w_s[2], pwr_w_s[3], pwr_w_s[4], pwr_w_s[5] );
  }
  else {
    String pwr_w_s = String(pwr_w,3);  
    sprintf(line1,"%c%c%c%c%cdBm %c%c%c%c%cmW" ,pwr_s[0], pwr_s[1], pwr_s[2], pwr_s[3], pwr_s[4], pwr_w_s[0], pwr_w_s[1], pwr_w_s[2], pwr_w_s[3], pwr_w_s[4] );
  }
  lcd.clear();
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
  Serial.print(pwr);
  Serial.print(";");
  Serial.println(pwr_w);
}

int measure(){
  float pwr1=0;
  pwr_adc[0] =  analogRead(A6);
  pwr_adc[1] =  analogRead(A6);
  pwr_adc[2] =  analogRead(A6);
  pwr_adc[3] = (pwr_adc[0] + pwr_adc[1] + pwr_adc[2])/3;
  pwr1 = map(pwr_adc[3], matrix[freq_sel][0], matrix[freq_sel][1], matrix[freq_sel][2], matrix[freq_sel][3]);
  return pwr1 + att;
}


void loop() {
  if (calibration_mode == 0){
    pwr = measure();
    if (mode == 0){
     freq_sel = (encoder.read()/4);
     if (freq_sel > 9){
       encoder.write(0);
       freq_sel = 0;
     }
     else if (freq_sel < 0){
       freq_sel = 10;
       encoder.write(10);
     }
   }
   else if ( mode == 1){
     att = (encoder.read()/4);
     if (att < 0){
       att = 0;
     }
   }

  }
  if (calibration_mode == 1) {
    pwr = measure();
    if (mode == 0){
     freq_sel = (encoder.read()/4);
     if (freq_sel > 9){
       encoder.write(0);
       freq_sel = 0;
     }
     else if (freq_sel < 0){
       freq_sel = 10;
       encoder.write(10);
     }
   }
   else if ( mode == 1){
     att = (encoder.read()/4);
     if (att < 0){
       att = 0;
     }
   }
  }
     if ((millis()%400) == 0){
     update_disp2();
   }
   if ((millis()%100) == 0){
     update_disp1();
   }
}
