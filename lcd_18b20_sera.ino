#include <OneWire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

OneWire  ds(12);  // on pin 12
const int sensorpin = A5;// mouisure sensor is aNalog input 
int moisture = 0 ;// beginning of moisture is 0
void setup(void) {
  lcd.begin(16, 2);
  Serial.begin(9600);

}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(2500);
    return;
  }

  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  Serial.println();

  // the first ROM byte indicates which chip
  switch (addr[0]) {
  case 0x10:
    Serial.println("  Chip = DS18S20");  // or old DS1820
    type_s = 1;
    break;
  case 0x28:
    Serial.println("  Chip = DS18B20");
    type_s = 0;
    break;
  case 0x22:
    Serial.println("  Chip = DS1822");
    type_s = 0;
    break;
  default:
    Serial.println("Device is not a DS18x20 family device.");
    return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  lcd.clear();
  lcd.print("   TEMPRATURE ");
  lcd.setCursor(0, 1);
  lcd.print(celsius);
  lcd.print(" C /");
  lcd.print(fahrenheit);
  lcd.print("F");
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
  delay(3000);
  if (celsius <= 20)
  {

    lcd.clear();
    lcd.print(celsius);
    lcd.print(" C /");
    lcd.print(fahrenheit);
    lcd.print("F");
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    lcd.setCursor(0, 1);
    lcd.print("  TEMP. IS LOW");
    delay(2000);
    lcd.clear();
    lcd.print(celsius);
    lcd.print(" C /");
    lcd.print(fahrenheit);
    lcd.print("F");
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    lcd.setCursor(0, 1);
    lcd.print(" HEATER STARTED ");
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);
    delay(2000);

  }
  else {
    digitalWrite(9,LOW); 
  }
  if (celsius >=25){//sıcaklık yüksek olunca 
    lcd.clear();
    lcd.print(celsius);
    lcd.print(" C /");
    lcd.print(fahrenheit);
    lcd.print("F");
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    lcd.setCursor(0, 1);
    lcd.print(" TEMP. IS HIGH");
    delay(2000);
    lcd.clear();
    lcd.print(celsius);
    lcd.print(" C /");
    lcd.print(fahrenheit);
    lcd.print("F");
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
    lcd.setCursor(0, 1);
    lcd.print("  A/C STARTED");
    digitalWrite(9,LOW);// heater pin 
    digitalWrite(8,HIGH);// ac pin 
    delay(2000);
  }
  else {
    digitalWrite(8,LOW);
  }
  moisture = analogRead (sensorpin);
  Serial.println("Moisture sensor is ");
  Serial.println(moisture);
  if (moisture >=300){//water level is suitable for plant , it should be between 0-1023 , 0 is less 1024 is max.  
    digitalWrite(10,LOW);// it controls water pomp
  }
  else {
    digitalWrite(10,HIGH);// it controls water pomp
    lcd.clear();
    lcd.print("WATER LEVEL: ");
    lcd.print(moisture);
    lcd.setCursor(0, 1);
    lcd.print("   WATER LESS");
    delay(2000);
    lcd.clear();
    lcd.print("WATER LEVEL: ");
    lcd.print(moisture);
    lcd.setCursor(0, 1);
    lcd.print("  PUMP STARTED  ");
    delay(2000);
  }
    if (moisture >=500){
      lcd.clear();
       lcd.print("WATER LEVEL: ");
    lcd.print(moisture);
        lcd.setCursor(0, 1);
      lcd.print("    PERFECT" );
    } else {
  if (moisture >= 400){
    lcd.clear();
  lcd.print("WATER LEVEL: ");
    lcd.print(moisture);
  lcd.setCursor(0, 1);
      lcd.print("     NORMAL" );
  }
    }
}

