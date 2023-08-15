#include <Wire.h>
#include <TDA7439.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <IRremote.h>
  IRrecv irrecv(10); // specify the output of IR reciver module
  TDA7439 tda;
  Encoder myEnc(9, 8);//CLK, DT
  decode_results ir;
  LiquidCrystal lcd(7, 6, 2, 3, 4, 5);// RS,E,D4,D5,D6,D7
  byte a1[8]={0b00000,0b11011,0b11011,0b11011,0b11011,0b11011,0b11011,0b00000};
  byte a2[8]={0b00000,0b11000,0b11000,0b11000,0b11000,0b11000,0b11000,0b00000};
  int vol,vol_ram,vol_d,z,bass,bass_ram,mids,mids_ram,treb,treb_ram,balans,balans_ram,in,in_ram,gain,gain1_ram,gain1,gain2,gain2_ram,gain3,gain3_ram,gain4,gain4_ram;
  byte menu,w,w1,in_d,gr1,gr2,n1,n2,s1,s2,v1,v2,b1,b2;
  unsigned long time,time1,time3,time4;long oldPosition  = -999;
void setup() {
  Serial.begin(9600);
  Serial.println("AB AUDIO TEC");
  lcd.begin(16, 2);
  irrecv.enableIRIn(); // run the IR module
  lcd.setCursor(0,0);
  lcd.print("   AB AUDIO TEC   ");
  delay(1000);
  pinMode(12,INPUT);// menu encoder button SW 
  pinMode(11,INPUT);// in 1-4
  lcd.createChar(0,a1);
  lcd.createChar(1,a2);
  vol = EEPROM.read(0);// vol eeprom 
  bass = EEPROM.read(1)-7;// bass eeprom
  mids = EEPROM.read(2)-7;// mids eeprom
  treb = EEPROM.read(3)-7;// treb eeprom
  balans = EEPROM.read(4)-4;// balans eeprom
  in = EEPROM.read(5);// in eeprom
  gain1 = EEPROM.read(6);// gain1 eeprom
  gain2 = EEPROM.read(7);// gain2 eeprom
  gain3 = EEPROM.read(8);// gain3 eeprom
  gain4 = EEPROM.read(9);// gain4 eeprom
  g1();audio();ram();
}

void loop() {
  if(millis()-time3>200){
   if ( irrecv.decode( &ir )) {Serial.print("0x");Serial.println( ir.value,HEX);time1=millis();time=millis();w=1;w1=1;irrecv.resume();}// IR приемник
   if(digitalRead(12)==LOW){menu++;ram();in_d=0;myEnc.write(0);oldPosition  = -999;time1=millis();w=1;if(menu>4){menu=0;}}// меню
   if(digitalRead(11)==HIGH){in++;;ram();in_d=1;g2();myEnc.write(0);oldPosition  = -999;menu=5;w=1;time1=millis();if(in>4||in<1){in=1;}}// in
 
   if(ir.value==0x807F926D){in_d=0;menu++;cl();if(menu>4){menu=0;}}// menu button up 0xFF18E7
   if(ir.value==0x807FB04F){in_d=0;menu--;cl();if(menu==255){menu=4;}}// menu button down 0xFF4AB5
  
   if(ir.value==0x807FF00F){menu=0;vol=0;cl();in_d=0;}// button #(MUTE) 0xFFB04F
   if(ir.value==0x807FB24D){in++;;ram();in_d=1;g2();myEnc.write(0);oldPosition  = -999;menu=5;w=1;time1=millis();if(in>4||in<1){in=1;}ir.value=0;}//  кнопка OK(IN) 0xFF38C7
   time3=millis();}
  // volume indication +control  //////////////
   if(menu==0){ 
     if(millis()-time3>100){
     if(ir.value==0x807F32CD ){vol++;gr1=1;gr2=0;cl();}// button > 0xFF5AA5
     if(ir.value==0xFFFFFFFF and gr1==1){vol++;gr2=0;cl();}//button >>>>>>
     if(ir.value==0x807F728D){vol--;gr1=0;gr2=1;cl();}// button < 0xFF10EF
     if(ir.value==0xFFFFFFFF and gr2==1){vol--;gr1=0;cl();}//button <<<<<<
     }
    long newPosition = myEnc.read()/4+vol_ram;
    if (newPosition != oldPosition) {
    oldPosition = newPosition;
 
   if(millis()-time4>50){
    lcd.clear();vol=newPosition;
   if(vol>48){vol=48;} if(vol<0){vol=0;}// 48 maximum volume, 0 minimum volume 
  lcd.setCursor(0,0);
  lcd.print("Volume   ");
  lcd.print(-48+vol);
  lcd.setCursor(13,0);
  lcd.print("dB");
  vol_d=vol/2;
   if(vol_d>=0){for(z=0;z<=vol_d;z++){lcd.setCursor(z/2,1);lcd.write((uint8_t)0);}}
   if(vol_d%2==0){lcd.setCursor(z/2,1);lcd.write((uint8_t)1);}
   lcd.setCursor(13,1);lcd.print(vol);
   audio();time=millis();time1=millis();w=1;w1=1;
  time4=millis();} 
 
 }}
 
 // bass indication + control of button //////////////
   if(menu==1){
      if(ir.value==0x807F32CD){bass++;n1=1;n2=0;cl();}// button > 0xFF5AA5
      if(ir.value==0xFFFFFFFF and n1==1){bass++;n2=0;cl();}// button >>>>>>
      if(ir.value== 0x807F728D){bass--;n1=0;n2=1;cl();}// button < 0xFF10EF
      if(ir.value==0xFFFFFFFF and n2==1){bass--;n1=0;cl();}// button <<<<<<
 
    long newPosition = myEnc.read()/4+bass_ram;
   if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();bass=newPosition;
   if(bass>7){bass=7;}if(bass<-7){bass=-7;}
  lcd.setCursor(0,0);
  lcd.print("Bass      ");
  lcd.print(bass*2);
  lcd.setCursor(13,0);
  lcd.print("dB");
   for(z=-7;z<=bass;z++){lcd.setCursor(z+7,1);lcd.write((uint8_t)0);}
   audio();time=millis();time1=millis();w=1;w1=1;
 }}
 ///////////////////////////////////////////////////////////
 
    // mid indication + control of button //////////////
   if(menu==2){
         if(ir.value==0x807F32CD){mids++;s1=1;s2=0;cl();}// button > 0xFF5AA5
         if(ir.value==0xFFFFFFFF and s1==1){mids++;s2=0;cl();}// button >>>>>>
         if(ir.value==0x807F728D){mids--;s1=0;s2=1;cl();}// button < 0xFF10EF
         if(ir.value==0xFFFFFFFF and s2==1){mids--;s1=0;cl();}// button <<<<<<
 
    long newPosition = myEnc.read()/4+mids_ram;
   if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();mids=newPosition;
   if(mids>7){mids=7;}if(mids<-7){mids=-7;}
  lcd.setCursor(0,0);
  lcd.print("Mids      ");
  lcd.print(mids*2);
  lcd.setCursor(13,0);
  lcd.print("dB");
   for(z=-7;z<=mids;z++){lcd.setCursor(z+7,1);lcd.write((uint8_t)0);}
   audio();time=millis();time1=millis();w=1;w1=1;
 }}
 ///////////////////////////////////////////////////////////
 
  // treble indication + control of button //////////////
   if(menu==3){
      if(ir.value==0x807F32CD){treb++;v1=1;v2=0;cl();}// button > 0xFF5AA5
      if(ir.value==0xFFFFFFFF and v1==1){treb++;v2=0;cl();}// button >>>>>>
      if(ir.value== 0x807F728D){treb--;v1=0;v2=1;cl();}// button < 0xFF10EF
      if(ir.value==0xFFFFFFFF and v2==1){treb--;v1=0;cl();}// button <<<<<<
 
    long newPosition = myEnc.read()/4+treb_ram;
   if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();treb=newPosition;
   if(treb>7){treb=7;}if(treb<-7){treb=-7;}
  lcd.setCursor(0,0);
  lcd.print("Trebble   ");
  lcd.print(treb*2);
  lcd.setCursor(13,0);
  lcd.print("dB");
   for(z=-7;z<=treb;z++){lcd.setCursor(z+7,1);lcd.write((uint8_t)0);}
   audio();time=millis();time1=millis();w=1;w1=1;
 }}
 ///////////////////////////////////////////////////////////
 
  // balance indication + control of button +\- 4 дБ ///////
   if(menu==4){
      if(ir.value==0x807F32CD){balans++;b1=1;b2=0;cl();}// button > 0xFF5AA5
      if(ir.value==0xFFFFFFFF and b1==1){balans++;b2=0;cl();}// button >>>>>>
      if(ir.value==0x807F728D){balans--;b1=0;b2=1;cl();}//button < 0xFF10EF
      if(ir.value==0xFFFFFFFF and b2==1){balans--;b1=0;cl();}// button <<<<<<
 
    long newPosition = myEnc.read()/4+balans_ram;
   if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();balans=newPosition;
   if(balans>4){balans=4;}if(balans<-4){balans=-4;}
  lcd.setCursor(0,0);
   if(balans>=0){lcd.print("-");}else{lcd.print("+");}
  lcd.print(abs(balans));lcd.print(" dB ");
  lcd.print(" <>  ");
   if(balans>=0){lcd.print("+");}else{lcd.print("-");}
  lcd.print(abs(balans));
  lcd.print(" dB ");
  lcd.setCursor(0,1);
  lcd.print("L");
  lcd.setCursor(15,1);
  lcd.print("R");
   if(balans<0){lcd.setCursor(balans+7,1);lcd.write((uint8_t)0);}
   if(balans>0){lcd.setCursor(balans+8,1);lcd.write((uint8_t)0);}
   if(balans==0){lcd.setCursor(7,1);lcd.write((uint8_t)0);lcd.setCursor(8,1);lcd.write((uint8_t)0);}
   audio();time=millis();time1=millis();w=1;w1=1;
 }}
 /////////////////////////////////////////////////////////////
 
  /////////// input + gain1 /////////////////////////////////////////
   if(in_d==1&&in==1){
    long newPosition = myEnc.read()/4+gain1_ram;
    if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();gain1=newPosition;
   if(gain1>15){gain1=15;}if(gain1<0){gain1=0;}
    lcd.setCursor(0,0);
    lcd.print("Input: ");
    lcd.print(in);
    lcd.setCursor(0,1);
    lcd.print("Input Gain: ");
    lcd.print(gain1);
   g1();audio();time=millis();time1=millis();w=1;w1=1; g2();
 }}
////////////////////////////////////////////////////////////
  /////////// input + gain2 /////////////////////////////////////////
   if(in_d==1&&in==2){
    long newPosition = myEnc.read()/4+gain2_ram;
    if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();gain2=newPosition;
   if(gain2>15){gain2=15;}if(gain2<0){gain2=0;}
    lcd.setCursor(0,0);
    lcd.print("Input: ");
    lcd.print(in);
    lcd.setCursor(0,1);
    lcd.print("Input Gain: ");
    lcd.print(gain2);
   g1();audio();time=millis();time1=millis();w=1;w1=1; g2();
 }}
////////////////////////////////////////////////////////////
  /////////// input + gain3 /////////////////////////////////////////
   if(in_d==1&&in==3){
    long newPosition = myEnc.read()/4+gain3_ram;
    if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();gain3=newPosition;
   if(gain3>15){gain3=15;}if(gain3<0){gain3=0;}
    lcd.setCursor(0,0);
    lcd.print("Input: ");
    lcd.print(in);
    lcd.setCursor(0,1);
    lcd.print("Input Gain: ");
    lcd.print(gain3);
   g1();audio();time=millis();time1=millis();w=1;w1=1; g2();
 }}
////////////////////////////////////////////////////////////
  /////////// input + gain4 /////////////////////////////////////////
   if(in_d==1&&in==4){
    long newPosition = myEnc.read()/4+gain4_ram;
    if (newPosition != oldPosition) {
    oldPosition = newPosition;lcd.clear();gain4=newPosition;
   if(gain4>15){gain4=15;}if(gain4<0){gain4=0;}
    lcd.setCursor(0,0);
    lcd.print("Input: ");
    lcd.print(in);
    lcd.setCursor(0,1);
    lcd.print("Input Gain: ");
    lcd.print(gain4);
   g1();audio();time=millis();time1=millis();w=1;w1=1; 
 }}
////////////////////////////////////////////////////////////
 
   if(millis()-time>10000 && w1==1){// return to the volume menu after 10 seconds of inactivity
     ram();myEnc.write(0);menu=0;w1=0;in_d=0; }
   if(millis()-time1>60000 && w==1){// writing all settings to EEPROM after 60 seconds of inactivity
     EEPROM.update(0,vol);
     EEPROM.update(4,balans+4);
     EEPROM.update(1,bass+7);
     EEPROM.update(2,mids+7);
     EEPROM.update(3,treb+7);
     EEPROM.update(5,in);
     EEPROM.update(6,gain1);EEPROM.update(7,gain2);EEPROM.update(8,gain3);EEPROM.update(9,gain4);w=0;
 }
 }//void
 void ram(){vol_ram=vol,bass_ram=bass,mids_ram=mids,treb_ram=treb,balans_ram=balans,in_ram=in,gain1_ram=gain1;gain2_ram=gain2;gain3_ram=gain3;gain4_ram=gain4;}
 void g1(){if(in==1){gain=gain1;}if(in==2){gain=gain2;}if(in==3){gain=gain3;}if(in==4){gain=gain4;}}
 void g2(){if(in==1){gain1=gain;}if(in==2){gain2=gain;}if(in==3){gain3=gain;}if(in==4){gain4=gain;}}
 void cl(){ir.value=0;ram();delay(100);lcd.clear();myEnc.write(0);oldPosition  = -999;}
 void audio(){
  tda.setInput(in); // 1 to 4 // channel number
  tda.inputGain(gain); // 0 to 15 // gain 
  tda.setVolume(vol); // 0 to 48 ( 0 is mute) //volume
  tda.setSnd(bass, 1); //-7 to +7 , 1 - Bass | 2 - Mids | 3 - Trebble
  tda.setSnd(mids, 2);
  tda.setSnd(treb, 3); 
  tda.spkAtt(4-balans, 4+balans); //balance R L or 0 to 79 (db) 


  
}
