#include <DHT.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <DS1302.h>


int potPin = 0; 
int DHTPin = 2;
int button3Pin = 3;
int button1Pin = 7;
int button2Pin = 8;
int ledPanel = 9;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
DS1302 rtc(4, 5, 6);    // (RST, DAT, CLK)
DHT dht(DHTPin, DHT11);

Time t;

byte ZN[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

int potLevel = 0;
int levelPanel = 0;
int ledLevel = 0;
boolean flag1 = false;
boolean flag2 = false;
boolean flag3 = false;
boolean current1Button = false;
boolean current2Button = false;
boolean current3Button = false;
int status = 0;
int statuslcd = 1;

void(* resetFunc) (void) = 0;

void setup() { 
  pinMode(potPin, INPUT);
  pinMode(ledPanel,OUTPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  lcd.begin(16, 2); 
  lcd.createChar(8,ZN);
  lcd.clear();
  Serial.begin(9600); 
  dht.begin();
  //Установка  часов для запуска
  // rtc.halt(false);
  // rtc.writeProtect(false);
  // The following lines can be commented out to use the values already stored in the DS1302
  //rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY  Monday
  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(6, 8, 2010);   // Set the date to August 6th, 2010
} 

void LCD_Britghtness(int data){ 
  int persent = map(data,0,255,0,100);
  int znak = map(persent,0,100,0,16);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BRIGHTNESS: "); 
  lcd.print(persent); 
  lcd.print("%   ");
  lcd.setCursor(0,1);
  for(int i = 0;i<znak;i++){
    lcd.write(8);
  }
  delay(80);
}

void LCD_DHT(int h, int t){
  if ((h == 0) && (t == 0)) {
    lcd.setCursor(2,0);
    lcd.print("ERROR SENSOR");
    delay(500);
    lcd.clear();
    return;
  }
  lcd.print("Humidity:    "); 
  lcd.print(h);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Temperature: "); 
  lcd.print(t);
  lcd.print("C");
}

boolean debounce1(boolean last){
  boolean current1 = digitalRead(button1Pin);
  if (last!=current1){
    delay(5);
    current1 = digitalRead(button1Pin);
  }
  return current1;
}

boolean debounce2(boolean last){
  boolean current2 = digitalRead(button2Pin);
  if (last!=current2){
    delay(5);
    current2 = digitalRead(button2Pin);
  }
  return current2;
}

boolean debounce3(boolean last){
  boolean current3 = digitalRead(button3Pin);
  if (last!=current3){
    delay(5);
    current3 = digitalRead(button3Pin);
  }
  return current3;
}

void inits(){
  lcd.print("INITIALIZAION...");
  lcd.setCursor(0,1);
  for(int i = 0;i<16;i++){
    lcd.write(8);
    delay(100);
  }
  delay(1000);
}

void loop() 

{ 
  potLevel = analogRead (potPin);
  ledLevel = map(potLevel,0,1023,0,255);
  analogWrite(ledPanel,ledLevel);
  int h = dht.readHumidity();
  int tem = dht.readTemperature();
  current1Button = debounce1(flag1);
  if ((flag1==false) && current1Button == HIGH){
    lcd.clear();
    status++;
    if (status>3){
      status = 1; 
    }
  } 
  flag1 = current1Button;
  if (status == 0){
    inits();
    status++;
  }
  if (status == 1){
    LCD_Britghtness(ledLevel);
  }
  if (status == 2){
    LCD_DHT(h,tem);
  }
  if (status == 3){
    t = rtc.getTime(); 

    lcd.setCursor(3, 0); // отступ первой строки слева 0 .
    // lcd.print(rtc.getDateStr());  //  дата  в формате    день.месяц.год

    lcd.print(t.date, DEC);            // день
    lcd.print(" ");
    lcd.print(rtc.getMonthStr(FORMAT_SHORT));     //  месяц
    lcd.print(" ");
    lcd.print(t.year, DEC);         //   год

    lcd.setCursor(2, 1); // отступ второй строки слева 1 .
    //

    // или по отдельности
    lcd.print(rtc.getDOWStr(FORMAT_SHORT)); // день недели
    lcd.print(" ");
    lcd.print(rtc.getTimeStr());    //  Время в формате    час:минуты:секунды
  }

  current2Button = debounce2(flag2);
  if ((flag2==false) && current2Button == HIGH){
    statuslcd++;
    if (statuslcd>2){
      statuslcd =1;
    } 
  }
  flag2 = current2Button;
  if (statuslcd == 1){
    lcd.backlight();
  }
  if (statuslcd == 2){
    lcd.noBacklight();
  }

  current3Button = debounce3(flag3);
  if ((flag3==false) && current3Button == HIGH){
    lcd.clear();
    lcd.print("RELOADING...");
    lcd.setCursor(0,1);
    for(int i = 0;i<16;i++){
      lcd.write(8);
      delay(100);
    }
    analogWrite(ledPanel,0);
    resetFunc();
  } 
}





