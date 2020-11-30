#include <NodeHue.h>
#include <SPI.h>
#include <MFRC522.h>

// SPECIFY FOR YOUR SETUP
#define IP "192.168.0.183" // Ip-adress of Philips Hue Bridge
#define PASS "oeAK1zez3MSmaO9z1ljYFt34GBLlpINlb0ZYGFO3" // also know as username
#define ID 4 // lamp ID
#define green 2
#define red 3
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

NodeHue hue(IP, &Serial); // Define NodeHue
long color = 0;
int sat = 0;
int bri = 0;
int scan = 0;

void setup(){
  hue.connect(PASS); // Setup serial connection
  Serial.begin(115200); // Setup Serial. MUST BE 115200
  hue.turnOn(ID,true); // Turn on hue (not necessary)

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT); 

  // RFID READER
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void cardScan() {
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
     Serial.println(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  delay(100);
  content.toUpperCase();
  if (content.substring(1) == "20 1C 8B 7C") //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 60000;
    sat += 255;
    bri += 255;
    scan++; 
  }

  if (content.substring(1) == "10 3F 8C 7C") //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 30000;
    sat += 100;
    bri += 50;
    scan++; 
  }

  if (content.substring(1) == "4B B9 2A 0B") //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 500;
    sat += 255;
    bri += 255;
    scan++; 
  }
  delay(50);

  if (content.substring(1) == "04 AB 8B 72 97 3C 80") //change here the UID of the card/cards that you want to give access
  {
    color = 0;
    sat = 0;
    bri = 0;
    scan = 0; 
    hue.turnOn(ID,false);
  }
}

void setColor() {
  hue.setHueSat(ID, color/scan, sat/scan);
  hue.setBri(ID,bri/scan);
}

void loop(){
    cardScan();
    if (scan != 0) {
    setColor();
    }
    delay(50);
} 
