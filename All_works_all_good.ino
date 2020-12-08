#include <NodeHue.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SR04.h>

// SPECIFY FOR YOUR SETUP
#define IP "192.168.0.183" // Ip-adress of Philips Hue Bridge
#define PASS "oeAK1zez3MSmaO9z1ljYFt34GBLlpINlb0ZYGFO3" // also know as username
#define ID 4 // lamp ID
//#define ID2 1 // lamp id 2
#define SS_PIN 10
#define SS_PIN2 8
#define RST_PIN 9
#define TRIG_PIN 6
#define ECHO_PIN 7

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522 mfrc522_2(SS_PIN2, RST_PIN);   // Create MFRC522 instance.

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;

NodeHue hue(IP, &Serial); // Define NodeHue
//NodeHue hue2(IP, &Serial); // Define NodeHue
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
boolean card4 = false;

// constants won't change. They're used here to set pin numbers:
const int SENSOR1 = 4;       // the Arduino's input pin that connects to the sensor's SIGNAL pin
const int SENSOR2 = 5;       // the Arduino's input pin that connects to the sensor's SIGNAL pin



void setup(){
  hue.connect(PASS); // Setup serial connection
  //hue2.connect(PASS); // Setup serial connection
  Serial.begin(115200); // Setup Serial. MUST BE 115200
  hue.turnOn(ID,true); // Turn on hue (not necessary)
  //hue2.turnOn(ID2,true); // Turn on hue (not necessary)
  
  // RFID READER
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // initialize the Arduino's pin as aninput
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
}

void reset() {
  color = 0;
  sat = 0;
  bri = 0;
  scan = 0;
  satChange = 0;

  card1 = false;
  card2 = false;
  card3 = false;
  card4 = false;
    
  hue.turnOn(ID,false);
  //hue2.turnOn(ID2,false);
  delay(100);
  }

void cardScan1() {
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
  if (content.substring(1) == "20 1C 8B 7C" && card1 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    //hue2.turnOn(ID2,true);
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
    satChange = 0;
    delay(100);
    }
  

  if (content.substring(1) == "10 3F 8C 7C" && card2 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    //hue2.turnOn(ID2,true);
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
    satChange = 0;
    delay(100);
    }

  if (content.substring(1) == "4B B9 2A 0B" && card3 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    //hue2.turnOn(ID2,true);
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
    satChange = 0;
    delay(100);
    }


  if (content.substring(1) == "04 AB 8B 72 97 3C 80" && card4 == false) //change here the UID of the card/cards that you want to give access
  {
    hue.turnOn(ID,true);
    color += 30000;
    sat += 25;
    bri += 250;
    scan++;
    card4 = true;
    change = 1;
    delay(100);
  }
  else if (content.substring(1) == "04 AB 8B 72 97 3C 80" && card4 == true) //change here the UID of the card/cards that you want to give access
  {
    color -= 30000;
    sat -= 25;
    bri -= 250;
    scan--;
    card4 = false;
    satChange = 0;
    delay(100);
  }
}



int getSat() {
  if (scan > 0) {
    return sat/scan;
    }
  return 0;
  }

void setColor() {
  hue.setHueSat(ID, color/scan, sat/scan + satChange);
  //hue2.setHueSat(ID2, color/scan, sat/scan + satChange);
}

void setNewColor() {
  hue.setHueSat(ID, color/scan, sat/scan + satChange);
  //hue2.setHueSat(ID2, color/scan, sat/scan + satChange);
  hue.setBri(ID,bri/scan);
  //hue2.setBri(ID2,bri/scan);
}

void setSat() {
  currentState1 = digitalRead(SENSOR1);     // the current reading from the input pin
  currentState2 = digitalRead(SENSOR2);     // the current reading from the input pin
    
  if (currentState1 == 1 && currentState2 == 1 && valueSat <= 250) {
    if (sat/scan + satChange <= 250) {
    satChange += 5;
    }
    hue.setHueSat(ID, color/scan, sat/scan + satChange);
    //hue2.setHueSat(ID2, color/scan, sat/scan + satChange);
    Serial.println("Sat+");
    delay(100);
    setSat();
    }
    
  if ((currentState1 == 1 && sat >=5) || (currentState2 == 1 && sat >= 5)) {
    if (sat/scan + satChange >= 5) {
      satChange -= 5;
      }
    hue.setHueSat(ID, color/scan, sat/scan + satChange);
    //hue2.setHueSat(ID2, color/scan, sat/scan + satChange);
    Serial.println("Sat-");
    delay(100);
    setSat();
  }
}

void loop(){
    cardScan1();
    //cardScan1();
    if (scan > 0 && change == 1) {
      setNewColor();
    }
    else if (scan > 0) {
      setColor();
      }
    else if (scan == 0) {
      hue.turnOn(ID, false);
      sat = 0;
      //hue2.turnOn(ID2, false);
      }
    long dis = sr04.Distance();
    if (5 <= dis && dis <= 30.5) {
      hue.setBri(ID, ((dis - 5) * 10));
      //hue2.setBri(ID2, ((dis - 5) * 10));
      Serial.println((dis - 5) * 10);
      }
    else if (dis <= 3) {
      reset();
      }
    setSat();
    change = 0;
    delay(300);
}
