#include <NodeHue.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SR04.h>

// SPECIFY FOR YOUR SETUP
#define IP "192.168.0.183" // Ip-adress of Philips Hue Bridge
#define PASS "oeAK1zez3MSmaO9z1ljYFt34GBLlpINlb0ZYGFO3" // also know as username
#define ID 4 // lamp ID
#define TRIG_PIN 6
#define ECHO_PIN 7

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;

NodeHue hue(IP, &Serial); // Define NodeHue
long color = 0;
int sat = 0;
int bri = 0;
int scan = 0;

int change;
int satChange = 0;

int currentState1;
int currentState2;
int valueSat;

boolean card1 = false;
boolean card2 = false;
boolean card3 = false;

// constants won't change. They're used here to set pin numbers:
const int SENSOR1 = 4;       // the Arduino's input pin that connects to the sensor's SIGNAL pin
const int SENSOR2 = 5;       // the Arduino's input pin that connects to the sensor's SIGNAL pin

const byte numReaders = 2;
const byte ssPins[] = {8, 10};
const byte resetPin = 9;

MFRC522 mfrc522[numReaders];

void setup(){
  hue.connect(PASS); // Setup serial connection
  Serial.begin(115200); // Setup Serial. MUST BE 115200
  hue.turnOn(ID,true); // Turn on hue (not necessary)

  // RFID READER
  SPI.begin();      // Initiate  SPI bus
  for (uint8_t i = 0; i < numReaders; i++) {
    mfrc522[i].PCD_Init(ssPins[i], resetPin);
  }

  // initialize the Arduino's pin as aninput
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
}

void cardScan() {
  // Look for new cards
    for (uint8_t i = 0; i < numReaders; i++) {
  if ( ! mfrc522[i].PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522[i].PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte j = 0; j < mfrc522[i].uid.size; j++) 
  {
     Serial.print(mfrc522[i].uid.uidByte[j] < 0x10 ? " 0" : " ");
     Serial.println(mfrc522[i].uid.uidByte[j], HEX);
     content.concat(String(mfrc522[i].uid.uidByte[j] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522[i].uid.uidByte[j], HEX));
  }
  delay(100);
  content.toUpperCase();
  if (content.substring(1) == "20 1C 8B 7C" && card1 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 60000;
    sat += 255;
    bri += 255;
    scan++;
    card1 = true;
    change = 1;
    delay(100);
  }
  else if (content.substring(1) == "20 1C 8B 7C" && card1 == true){
    color -= 60000;
    sat -= 255;
    bri -= 255;
    scan--;
    card1 = false;
    delay(100);
    }
  

  if (content.substring(1) == "10 3F 8C 7C" && card2 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 30000;
    sat += 100;
    bri += 50;
    scan++;
    card2 = true;
    change = 1;
    delay(100);
  }
  else if (content.substring(1) == "10 3F 8C 7C" && card2 == true) {
    color -= 30000;
    sat -= 100;
    bri -= 50;
    scan--;
    card2 = false;
    delay(100);
    }

  if (content.substring(1) == "4B B9 2A 0B" && card3 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 500;
    sat += 200;
    bri += 200;
    scan++;
    card3 = true;
    change = 1;
    delay(100);
  }
  else if (content.substring(1) == "4B B9 2A 0B" && card3 == true) {
    color -= 500;
    sat -= 200;
    bri -= 200;
    scan--;
    card3 = false;
    delay(100);
    }


  if (content.substring(1) == "04 AB 8B 72 97 3C 80") //change here the UID of the card/cards that you want to give access
  {
    color = 0;
    sat = 0;
    bri = 0;
    scan = 0;
    satChange = 0;

    card1 = false;
    card2 = false;
    card3 = false;
    
    hue.turnOn(ID,false);
    delay(100);
  }
    }
}

int getSat() {
  if (scan > 0) {
    return sat/scan;
    }
  return 0;
}

void setColor() {
  hue.setHueSat(ID, color/scan, sat/scan);
}

void setNewColor() {
  hue.setHueSat(ID, color/scan, sat/scan);
  hue.setBri(ID,bri/scan);
}

void setSat() {
  currentState1 = digitalRead(SENSOR1);     // the current reading from the input pin
  currentState2 = digitalRead(SENSOR2);     // the current reading from the input pin
    
  if (currentState1 == 1 && currentState2 == 1 && valueSat <= 250) {
    if (sat <= 250) {
    sat += 5;
    }
    hue.setHueSat(ID, color/scan, sat);
    Serial.println("Sat+");
    delay(100);
    setSat();
    }
    
  if ((currentState1 == 1 && sat >=5) || (currentState2 == 1 && sat >= 5)) {
    if (sat >= 5) {
      sat -= 5;
      }
    hue.setHueSat(ID, color/scan, sat);
    Serial.println("Sat-");
    delay(100);
    setSat();
  }
}

void loop(){
    cardScan();
    if (scan > 0 && change == 1) {
      setNewColor();
    }
    else if (scan > 0) {
      setColor();
      }
    else if (scan == 0) {
      hue.turnOn(ID, false);
      }
    long dis = sr04.Distance();
    if (0 <= dis && dis <= 25.5) {
      hue.setBri(ID, (dis * 10));
      Serial.println(dis*10);
      }
    setSat();
    change = 0;
    delay(500);
}
