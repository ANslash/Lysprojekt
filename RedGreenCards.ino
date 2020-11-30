#include <NodeHue.h>
#include <SPI.h>
#include <MFRC522.h>

// SPECIFY FOR YOUR SETUP
#define IP "192.168.0.183" // Ip-adress of Philips Hue Bridge
#define PASS "oeAK1zez3MSmaO9z1ljYFt34GBLlpINlb0ZYGFO3" // also know as username
#define ID 4 // lamp ID
#define green 2
#define red 3
#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

NodeHue hue(IP, &Serial); // Define NodeHue

void setup(){
  hue.connect(PASS); // Setup serial connection
  Serial.begin(115200); // Setup Serial. MUST BE 115200
  Serial1.begin(9600); //
  hue.turnOn(ID,true); // Turn on hue (not necessary)

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT); 

  // RFID READER
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop(){
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
  Serial1.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial1.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial1.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase();
  if (content.substring(1) == "20 1C 8B 7C") //change here the UID of the card/cards that you want to give access
  {
    digitalWrite(green, HIGH);
    hue.setRGB(ID, 0, 255, 0 );
    delay(3000);
    digitalWrite(green, LOW);
    hue.setRGB(ID, 0, 0, 250 );
  }
 
 else   {
    digitalWrite(red, HIGH);
    hue.setRGB(ID, 255, 0, 0 );
    delay(3000);
    digitalWrite(red, LOW);
    hue.setRGB(ID, 0, 0, 250 );
  }
} 
