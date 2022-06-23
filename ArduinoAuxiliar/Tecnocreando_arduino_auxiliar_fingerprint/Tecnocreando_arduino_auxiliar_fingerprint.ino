/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>
#define WHITE_LED 4
#define GREEN_LED 5
#define RED_LED 6
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif
int str_len = 0;

int voidPosition = 0;
uint8_t positionToStore;
uint8_t positionFound;

uint8_t id;
int limitFingerprint = 10;
String fingerprintId = "";

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int getFingerprintIDez();

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  //Serial.println("Fingerprint template extractor");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    //Serial.println("Found fingerprint sensor!");
  } else {
    //Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  // Try to get the templates for fingers 1 through 10
  /*for (int finger = 1; finger < 3; finger++) {
    downloadFingerprintTemplate(finger);
  }*/
  pinMode(WHITE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
}

uint8_t downloadFingerprintTemplate(uint16_t id)
{
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #"); Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }

  // OK success!

  /*Serial.print("Attempting to get #"); Serial.println(id);
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }*/

  // one data packet is 267 bytes. in one data packet, 11 bytes are 'usesless' :D
  /*uint8_t bytesReceived[534]; // 2 data packets
  memset(bytesReceived, 0xff, 534);
  uint32_t starttime = millis();
  int i = 0;
  while (i < 534 && (millis() - starttime) < 20000) {
    if (mySerial.available()) {
      bytesReceived[i++] = mySerial.read();
    }
  }
  Serial.print(i); Serial.println(" bytes read.");
  Serial.println("Decoding packet...");

  uint8_t fingerTemplate[512]; // the real template
  memset(fingerTemplate, 0xff, 512);

  // filtering only the data packets
  int uindx = 9, index = 0;
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);   // first 256 bytes
  uindx += 256;       // skip data
  uindx += 2;         // skip checksum
  uindx += 9;         // skip next header
  index += 256;       // advance pointer
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);   // second 256 bytes

  for (int i = 0; i < 512; ++i) {
    //Serial.print("0x");
    printHex(fingerTemplate[i], 2);
    //Serial.print(", ");
  }
  Serial.println("\ndone.");*/

  return p;

}



void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}
int opt = 0;

uint8_t getTemplate(uint16_t id, int voidPos)
{
  uint8_t p = finger.loadModel(id);

  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      Serial.print("void position is: " ); Serial.println(voidPos);
      positionToStore = readnumber(voidPos);
      limitFingerprint = 1;
      break;
      return p;
  }
  
  /*if(p == FINGERPRINT_OK){
    Serial.println("OK");
  }
  Serial.println("Template: ");*/
  //Serial.println(p);
  //Serial.print(p, DEC);
}


uint8_t readnumber(int voidPos) {
  uint8_t num = voidPos;
  return num;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(positionToStore);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      Serial.println(p);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }


  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(positionToStore);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }


    // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }


    // OK converted!
  Serial.print("Creating model for #");  Serial.println(positionToStore);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(positionToStore);
  p = finger.storeModel(positionToStore);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  limitFingerprint = 10;

  return true;
 
}


uint8_t validateId() {
  
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
     
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL: 
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!
  /*if(p == FINGERPRINT_NOFINGER)
  {
    Serial.println("No finger print");  
  }*/

  digitalWrite(WHITE_LED, HIGH);  
  delay(2000);
  
  p = finger.image2Tz();
  
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Found a print match!");
    //fingerprintId = String(finger.fingerID);
    //String val = "Valor ID: " + fingerprintId; 

    //Send Data to NodeMCU
    positionFound = readnumber(finger.fingerID);
    Serial.println(positionFound);
    
    //Serial.println(val);
    //Serial.println(finger.fingerID);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    //return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    //Serial.println("Did not find a match");

    //Blynk led to indicate fingerprint was not found
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(RED_LED, LOW);
    delay(300);
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(RED_LED, LOW);
    delay(300);
    
    //Send data to NodeMCU
    //NOT FOUND = 7
    //Serial.println("NOT_FOUND");
    
    //return p;
  } else {
    //Serial.println("Unknown error");
    //return p;
  }

  // found a match!
  //Serial.print("Found ID #"); Serial.print(finger.fingerID);
  //Serial.print(" with confidence of "); Serial.println(finger.confidence);
 
  digitalWrite(WHITE_LED, LOW);
  //return finger.fingerID;
  return true;
}

void loop()
{
    /*if(opt == 0)
    {
      Serial.print("Mnesaje");
      opt = 1;
      for (int finger = 1; finger < 3; finger++) {
    downloadFingerprintTemplate(finger);
  }
    }*/
int opt = 0;  
 
  if(Serial.available() > 0)
  {
    String opcion = "";
   
    //opcion = String(Serial.readString());/Only read first character
    //Reads all characters until finish
    opcion = Serial.readStringUntil('\n');

    //Convert string to int
    opt = opcion.toInt();
    
    //opt = opt+"2";
    //Serial.println(opcion.length());
    //Serial.println(opt);

    //Serial.println(opcion);
    str_len = opcion.length() + 1;
    //Serial.println(str_len);
 
    char url_t[str_len];
    opcion.toCharArray(url_t, str_len);
   
    //Serial.write(url_t);
    
    //Fingerprint position was validated successful
    if(opt == 111)
    {  
        digitalWrite(GREEN_LED, HIGH);
        delay(500);
        digitalWrite(GREEN_LED, LOW);
        opt = 0;
       
    }
    //Fingerprint position is invalid or without grants
    else if(opt == 113)
    {
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
    //Turn on LED to indicate user to put his finger. When enroll fingerprint is enable from NodeMCU
    //This is when a fingerprint is goin to be saved in database
    else if(opt == 111)
    {
      digitalWrite(GREEN_LED, HIGH);
      opt = 0;
    }
    //Turn off LED when enroll fingerprint is disabled
    else if(opt == 112)
    {
      digitalWrite(GREEN_LED, LOW);
      opt = 0;
    }
    
    //Confirm fingerprint position was added
    else if(opt == 114)
    {
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
      delay(500);
      digitalWrite(GREEN_LED, HIGH);
      delay(500);
      digitalWrite(GREEN_LED, LOW);
      opt = 0;
    }
    //Confirm fingerprint position was not added
    else if(opt == 115)
    {
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      delay(500);
      digitalWrite(RED_LED, HIGH);
      delay(500);
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
    //Button lock pressed, waiting for fingerprint to validate and lock
    else if(opt == 116)
    {
      digitalWrite(RED_LED, HIGH);
      opt = 0;
    }
    //TURN OFF waiting for fingerprint to validate and lock
    else if(opt == 117)
    {
      digitalWrite(RED_LED, LOW);
      opt = 0;
    }
  else
  {
    /*Serial.println(opcion);
    Serial.println("Invalido");*/
  }
 }

validateId();
delay(50);
  
}
