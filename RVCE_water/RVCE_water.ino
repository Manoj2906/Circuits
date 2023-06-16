#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include <SPI.h>
#include <MFRC522.h>
 int state = 0;
const int pin = 7;
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


#include <EEPROM.h>
#include "GravityTDS.h"
 
#define TdsSensorPin A0
GravityTDS gravityTds;
 
float temperature = 25,tdsValue = 0;

int sensorPin = A1;
float volt;
float ntu;

float calibration_value = 26.34;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

float ph_act;

 #include <SoftwareSerial.h>
#define RX 2 // TX of esp8266 in connected with Arduino pin 2
#define TX 3 // RX of esp8266 in connected with Arduino pin 3
String WIFI_SSID = "Manoj";// WIFI NAME
String WIFI_PASS = "Manoj0106"; // WIFI PASSWORD
String API = "EUL5ISFUEX073BU8";// Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false;   
SoftwareSerial esp8266(RX,TX);
void setup() 
{
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water Quality");
  lcd.setCursor(0,1);
  lcd.print("Monitor System");
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();


    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
    
 esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ WIFI_SSID +"\",\""+ WIFI_PASS +"\"",20,"OK");
}
void loop() 
{
 if (digitalRead(pin) == HIGH && state == 0) {
    Serial.print("\r");
    delay(1000);
    Serial.print("AT+CMGF=1\r");
    delay(1000);
    /*Replace XXXXXXXXXX to 10 digit mobile number & ZZ to 2 digit country code*/
    Serial.print("AT+CMGS=\"+916361442431\"\r");
    delay(1000);
    //The text of the message to be sent.
    Serial.print("Water quality is poor, we are sending it irrigation purpose");
    delay(1000);
    Serial.write(0x1A);
    delay(1000);
    state = 1;
  }
 if (digitalRead(pin) == HIGH && state == 1) {
    state = 0;

 }
  // Edhu Turbuidity

  
   volt = 0;
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(sensorPin)/1023)*5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,2);
    if(volt < 2.5){
      ntu = 3000;
    }else{
      ntu = -1120.4*square(volt)+5742.3*volt-4353.8; 
    }
Serial.println(ntu);

// Elithanka Turbuidity

// EDhu TDS dhu


//  temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    delay(1000);

    // ellithanka TDS

    // Ellinda PH

     for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(A3);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buffer_arr[i]>buffer_arr[j])
 {
 temp=buffer_arr[i];
 buffer_arr[i]=buffer_arr[j];
 buffer_arr[j]=temp;
 }
 }
 }
 avgval=0;
 for(int i=2;i<8;i++)
 avgval+=buffer_arr[i];
 float volt=(float)avgval*5.0/1024/6; 
  ph_act = -5.70 * volt + calibration_value;

// Serial.println("pH Val: ");
 Serial.println(ph_act);
 delay(1000);

// Ellithanka Ph dhu

double quality =  5.05935481*ph_act -0.01233624*ntu -0.07638136*tdsValue +83.40846924211432;
 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "99 4B 03 4D") //change here the UID of the card/cards that you want to give access
  {
    lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lohith");
  
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
    
       lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water Quality =");
   lcd.setCursor(0,1);
  lcd.print(quality);
  }

 if (content.substring(1) == "13 9D 7B 11") //change here the UID of the card/cards that you want to give access
  {
       lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pramod");
    Serial.println("Authorized access");
    Serial.println();
    delay(3000);
    
       lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Water Quality =");
   lcd.setCursor(0,1);
  lcd.print(quality);
  }
 
 else   {
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Invaild card!!");
    Serial.println(" Access denied");
    delay(3000);
  }
   String getData="GET /update?api_key="+ API+"&field1="+ph_act+
 "&field2="+ntu+"&field3="+tdsValue;
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
  
} 
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  } 
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  } 
  found = false;
 }
float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
