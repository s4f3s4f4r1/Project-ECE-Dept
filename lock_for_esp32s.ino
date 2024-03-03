#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint8_t id;
int Lock = 18;    // relay
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13,12,14,27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26,25,33,32}; //connect to the column pinouts of the keypad
 
char keypressed;                 //Where the keys are stored it changes very often
char code[]= {'1','2','3','4','5','6'};  //The default code, you can change it or make it a 'n' digits one
 
char check1[sizeof(code)];  //Where the new key is stored
char check2[sizeof(code)];  //Where the new key is stored again so it's compared to the previous one
 
short a=0,i=0,s=0,j=0;          //Variables used later
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); 

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

void setup(){
       Serial.begin(57600);
       while (!Serial);  // For Yun/Leo/Micro/Zero/...
       delay(100);
       Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

       // set the data rate for the sensor serial port
       finger.begin(57600);

       if (finger.verifyPassword()) {
       Serial.println("Found fingerprint sensor!");
       } else {
       Serial.println("Did not find fingerprint sensor :(");
       // while (1) { delay(1); }
       }         
      
      lcd.begin();      // initialize the lcd
      lcd.backlight();
      lcd.print(" C . F . I ");
      lcd.setCursor(3,1);
      lcd.print("Smart Lock");
      delay(2000);
      lcd.clear(); 
      lcd.print("Press* to Unlock");
      lcd.setCursor(0,1);
      lcd.print("or D to settings");
      delay(1000);
      //What's written on the lcd you can change
      pinMode(Lock, OUTPUT);
//    for(i=0 ; i<sizeof(code);i++){        //When you upload the code the first time keep it commented
//       EEPROM.get(i, code[i]);             //Upload the code and change it to store it in the EEPROM
//    }                                  //Then uncomment this for loop and reupload the code (It's done only once) 
         
}

//keypad input
uint8_t readnumber(void) {
 bool check =1;
  
  char numsample[4];
  int i = 0; 
  while(check==1){
    char a=keypad.getKey();
    if(a=='*'){
      check=0;
      break;
      }
      if(a!=0){
        numsample[i++]=a;
        }
  }
  if(i>0){
    int pin=0;
    for(int j=0;j<i;j++){
      pin=pin*10 +(numsample[j] - '0');
    }
      return pin;
    }
 }

 
void loop()
{
  keypressed = keypad.getKey();               //Constantly waiting for a key to be pressed
    if(keypressed == '*'){                      // * to open the lock
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Enter code &");
            lcd.setCursor(8,1);    //Message to show
            lcd.print("press A ");
            ReadCode();                          //Getting code function
                  if(a==sizeof(code)){           //The ReadCode function assign a value to a (it's correct when it has the size of the code array)
                     lcd.clear();
                     lcd.print("Press your");
                     lcd.setCursor(3,1);
                     lcd.print("finger");
                     delay(2000);

                     finger.getTemplateCount();
                     if(finger.templateCount == 0) {
                         lcd.clear();
                         lcd.print("No fingerprint");
                         lcd.setCursor(0,1);
                         lcd.print("in database");
                         delay(1000);
                     }
                     
                     else {
                         lcd.clear();
                         lcd.print("Waiting for ");
                         lcd.setCursor(1,1);
                         lcd.print("valid finger...");
                         delay(2000);
                         getFingerprintID();
                     }
                     delay(500);
                  }
                  
                  else{
                     lcd.clear();               
                     lcd.print("Wrong code");          //Message to print when the code is wrong
                     lcd.setCursor(0,1);
                     lcd.print("Moye Moye!!");
                     delay(2000);
                  }
            delay(2000);
            lcd.clear();
            lcd.print("Press* to Unlock");
            lcd.setCursor(0,1);
            lcd.print("or D to settings");             //Return to standby mode it's the message do display when waiting
        }

     if(keypressed == 'D'){
      lcd.clear();
      lcd.print("Show me if");
      lcd.setCursor(3,1);
      lcd.print("you are admin.");
      delay(2000);

      finger.getTemplateCount();

      if(finger.templateCount != 0) {
      lcd.clear();
      lcd.print("Waiting for ");
      lcd.setCursor(1,1);
      lcd.print("valid finger...");
      delay(2000);    
      getFingerprintIDadmin();
      }

      else {
      lcd.clear();
      lcd.print("No fingerprint");
      lcd.setCursor(0,1);
      lcd.print("in database");
      }

  delay(500);
     }
}
 
void ReadCode(){                  //Getting code sequence
       i=0;                      //All variables set to 0
       a=0;
       j=0; 
                                     
     while(keypressed != 'A'){                                     //The user press * to confirm the code otherwise he can keep typing
           keypressed = keypad.getKey();                         
             if(keypressed != NO_KEY && keypressed != 'A' ){       //If the char typed isn't * and neither "nothing"
              lcd.setCursor(j,1);                                  //This to write "*" on the lcd whenever a key is pressed it's position is controlled by j
              lcd.print("*");
              j++;
              
             if(keypressed == code[i]&& i<sizeof(code)){            //if the char typed is correct a and i increments to verify the next caracter
                 a++;                                              
                 i++;
                 }
            else
                a--;                                               //if the character typed is wrong a decrements and cannot equal the size of code []
            }
            }
    keypressed = NO_KEY;
 
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();   
      lcd.print("Image taken");
      delay(1000);
      break;
    case FINGERPRINT_NOFINGER:
      lcd.clear();
      lcd.print("No finger ");
      lcd.setCursor(0,1);
      lcd.print("detected");
      delay(800);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Communication ");
      lcd.setCursor(0,1);
      lcd.print("      error");
      delay(800);
      return p;
    case FINGERPRINT_IMAGEFAIL:
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("imaging error");
      delay(800);
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown error");
      delay(800);
      return p;
  }

  // OK success!

   p = finger.image2Tz();
   switch (p) {
     case FINGERPRINT_OK:
//           lcd.clear();
//       lcd.print("Image converted");
//       delay(800);
       break;
     case FINGERPRINT_IMAGEMESS:
//           lcd.clear();
//       lcd.setCursor(0,1);
//       lcd.print(" Blur Image ");
//       delay(800);
       return p;
     case FINGERPRINT_PACKETRECIEVEERR:
//           lcd.clear();
//       lcd.print("Communication ");
//       lcd.setCursor(0,1);
//       lcd.print("      error");
//       delay(800);
       return p;
     case FINGERPRINT_FEATUREFAIL:
//           lcd.clear();
//       lcd.print("Could not find ");
//       lcd.setCursor(0,1);
//       lcd.print("scanner");
//       delay(800);
       return p;
     case FINGERPRINT_INVALIDIMAGE:
//           lcd.clear();
//       lcd.print("Could not find ");
//       delay(800);
       return p;
     default:
//           lcd.clear();
//       lcd.print("Unknown error");
//       delay(800);
       return p;
   }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK && finger.confidence >=50 ) {
    OpenDoor();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
    delay(800);
    lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear();
    lcd.print("Didn`t find ");
    lcd.setCursor(1,1);
    lcd.print("a match");
    delay(1000);
    lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
    delay(800);
    lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
    return p;
  }

  return finger.fingerID;
}

uint8_t getFingerprintIDadmin() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image taken");
      delay(1000);
      break;
    case FINGERPRINT_NOFINGER:
      lcd.clear();
      lcd.print("No finger ");
      lcd.setCursor(0,1);
      lcd.print("detected");
      delay(800);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Communication ");
      lcd.setCursor(0,1);
      lcd.print("      error");
      delay(800);
      return p;
    case FINGERPRINT_IMAGEFAIL:
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("imaging error");
      delay(800);
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown error");
      delay(800);
      return p;
  }

  // OK success!

   p = finger.image2Tz();
   switch (p) {
     case FINGERPRINT_OK:
//           lcd.clear();
//       lcd.print("Image converted");
//       delay(800);
       break;
     case FINGERPRINT_IMAGEMESS:
//           lcd.clear();
//       lcd.setCursor(0,1);
//       lcd.print(" Blur Image ");
//       delay(800);
       return p;
     case FINGERPRINT_PACKETRECIEVEERR:
//           lcd.clear();
//       lcd.print("Communication ");
//       lcd.setCursor(0,1);
//       lcd.print("      error");
//       delay(800);
       return p;
     case FINGERPRINT_FEATUREFAIL:
//           lcd.clear();
//       lcd.print("Could not find ");
//       lcd.setCursor(0,1);
//       lcd.print("image");
//       delay(800);
       return p;
     case FINGERPRINT_INVALIDIMAGE:
//           lcd.clear();
//       lcd.print("Could not find ");
//       lcd.setCursor(0,1);
//       lcd.print("image");
//       delay(800);
       return p;
     default:
     
//           lcd.clear();
//       lcd.print("Unknown error");
//       delay(800);
       return p;
   }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK && finger.confidence >=80 && (finger.fingerID == 1 || finger.fingerID == 2) ) {
    lcd.clear();
    lcd.print("Opening Settings");
    delay(2000);
    lcd.clear();
    lcd.print("A)Enroll B)Del");
    lcd.setCursor(0,1);
    lcd.print("C)Return");
    delay(2000);
    lcd.clear();
    lcd.print("Press # to");
    lcd.setCursor(0,1);
    lcd.print("change password");
    delay(1000);   
    settings();
    lcd.clear();
    lcd.print("Press* to Unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
    delay(1000); 
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      lcd.clear();
      lcd.print("Communication error");
      delay(800);
      lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
      return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
      lcd.clear();
      lcd.print("Didn`t find ");
      lcd.setCursor(1,1);
      lcd.print("a match");
      delay(800);
      lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
      return p;
  } else {
      lcd.clear();
      lcd.print("Unknown error");
      delay(800);
      lcd.clear();
    lcd.print("Press* to unlock");
    lcd.setCursor(0,1);
    lcd.print("or D to settings");
      return p;
  }

  return finger.fingerID;
}
 
void settings(){   //modified
  bool check =1;
// Serial.println(key);
while (check){
  char key1 = keypad.getKey();
  
//   if (key1=='A' || key1=='B' || key1=='#' || key1=='C')
//     {
//       check=0;
//       break;
//     }
    
//   else if (key1=='')
//     {
//       check=1;
//     }

//   else
//     {
//       lcd.clear();
//       lcd.print("Wrong Key");
//       delay(1000);
//       check=1;
//     }
// }
  
   if(key1=='A')
   {
    lcd.clear();
    lcd.print("Now you can");
    lcd.setCursor(0,1);
    lcd.print("enroll ID");
    delay(500);
    enroll();
    check=0;
    break;
   }
   else if(key1=='B')
   {
    lcd.clear();
    lcd.print("Now you can");
    lcd.setCursor(0,1);
    lcd.print("Delete ID");
    delay(500);
    del();
    check=0;
    break;
   }
    if(key1=='C')
    {
     break;
    }
   else if(key1=='#')
   {
    lcd.clear();
    ChangeCode();
    check=0;
    break;
   }

//   else 
//   {
//    lcd.clear();
//    lcd.print("Wrong Key !!");
//   }
  
}
}

void ChangeCode(){                      //Change code sequence
      lcd.clear();
      lcd.print("Changing code");
      delay(1000);
      lcd.clear();
      lcd.print("Enter old code &");
      lcd.setCursor(8,1);
      lcd.print("Press A");
      ReadCode();                      //verify the old code first so you can change it
      
            if(a==sizeof(code)){      //again verifying the a value
            lcd.clear();
            lcd.print("Changing code");
            GetNewCode1();            //Get the new code
            GetNewCode2();            //Get the new code again to confirm it
            s=0;
              for(i=0 ; i<sizeof(code) ; i++){     //Compare codes in array 1 and array 2 from two previous functions
              if(check1[i]==check2[i])
              s++;                                //again this how we verifiy, increment s whenever codes are matching
              }
                  if(s==sizeof(code)){            //Correct is always the size of the array
                  
                   for(i=0 ; i<sizeof(code) ; i++){
                  code[i]=check2[i];         //the code array now receives the new code
                  EEPROM.put(i, code[i]);        //And stores it in the EEPROM
                  
                  }
                  lcd.clear();
                  lcd.print("Code Changed");
                  delay(2000);
                  lcd.clear();
                       lcd.print("A)Enroll B)Del");
                       lcd.setCursor(0,1);
                       lcd.print("C)Empty Database");
                       delay(2000);
                       lcd.clear();
                       lcd.print("Press # to");
                       lcd.setCursor(0,1);
                       lcd.print("change password");
                       delay(1000);  
                  }
                  else{                         //In case the new codes aren't matching
                  lcd.clear();
                  lcd.print("Codes are not");
                  lcd.setCursor(0,1);
                  lcd.print("matching !!");
                  delay(2000);
                  lcd.clear();
                       lcd.print("A)Enroll B)Del");
                      //  lcd.setCursor(0,1);
                      //  lcd.print("C)Empty Database");
                       delay(2000);
                       lcd.clear();
                       lcd.print("Press # to");
                       lcd.setCursor(0,1);
                       lcd.print("change password");
                       delay(1000);  
                  }
          }
          else{                     //In case the old code is wrong you can't change it
          lcd.clear();
          lcd.print("Wrong Code !!");
          delay(2000);
          lcd.clear();
                       lcd.print("A)Enroll B)Del");
                      //  lcd.setCursor(0,1);
                      //  lcd.print("C)Empty Database");
                       delay(2000);
                       lcd.clear();
                       lcd.print("Press # to");
                       lcd.setCursor(0,1);
                       lcd.print("change password");
                       delay(1000);  
          
          }
}
 
void GetNewCode1(){                      
  i=0;
  j=0;
  lcd.clear();
  lcd.print("Enter new code &");   //tell the user to enter the new code and press A
  lcd.setCursor(8,1);
  lcd.print("press A");
  delay(2000);     //Press A keep showing while the top row print *
             
         while(keypressed != 'A'){            //A to confirm and quits the loop
             keypressed = keypad.getKey();
               if(keypressed != NO_KEY && keypressed != 'A' ){
                lcd.setCursor(j,1);
                lcd.print("*");                 //On the new code you can show * as I did or change it to keypressed to show the keys
                check1[i]=keypressed;     //Store caracters in the array
                i++;
                j++;                    
                }
                }
keypressed = NO_KEY;
}
 
void GetNewCode2(){                         //This is exactly like the GetNewCode1 function but this time the code is stored in another array
  i=0;
  j=0;
  
  lcd.clear();
  lcd.print("Confirm code &");
  lcd.setCursor(8,1);
  lcd.print("press A");
  delay(3000);
 
         while(keypressed != 'A'){
             keypressed = keypad.getKey();
               if(keypressed != NO_KEY && keypressed != 'A' ){
                lcd.setCursor(j,1);
                lcd.print("*");
                check2[i]=keypressed;
                i++;
                j++;                    
                }
                }
keypressed = NO_KEY;
}

void enroll()   //Enrolling Fingerprint
{
  lcd.clear();
  lcd.print("Ready to enroll");
  lcd.setCursor(0,1);
  lcd.print("a fingerprint!");
  delay(1000);
  lcd.clear();
  lcd.print("Type ID #");
  lcd.setCursor(0,1);
  lcd.print("(from 1 to 127)");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  lcd.clear();
  lcd.print("Enrolling ID #");
  lcd.setCursor(0,1);
  lcd.print(id);
  delay (3000);

  while (!  getFingerprintEnroll() );
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  lcd.clear();
  lcd.print("please wait...");
  delay(500);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
       lcd.print("Image taken");
       delay(500);
      break;
    case FINGERPRINT_NOFINGER:
           lcd.clear();
       lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
           lcd.clear();
      lcd.print("Communication");
       lcd.setCursor(2,1);
       lcd.print("error");
       delay (500);
      break;
    case FINGERPRINT_IMAGEFAIL:;
      lcd.clear();
      lcd.print("Imaging error");
       delay (500);   
      break;
    default:
    lcd.clear();
      lcd.print("Unknown error");
       delay (500);
      break;
    }
  }

  // OK success!

   p = finger.image2Tz(1);
   switch (p) {
     case FINGERPRINT_OK:
//     lcd.clear();
//       lcd.print("Image converted");
//        delay (500);     
       break;
     case FINGERPRINT_IMAGEMESS:
//     lcd.clear();
//       lcd.print("Image too messy");
//        delay (500);
       return p;
     case FINGERPRINT_PACKETRECIEVEERR:
//     lcd.clear();
//       lcd.print("Communication error");
//        delay (500);
       return p;
     case FINGERPRINT_FEATUREFAIL:
//     lcd.clear();
//       lcd.print("Could not find");
//       lcd.setCursor(0,1);
//       lcd.print("Sensor features");
//        delay (500);
       return p;
     case FINGERPRINT_INVALIDIMAGE:
//     lcd.clear();
//       lcd.print("Could not find");
//       lcd.setCursor(0,1);
//       lcd.print("Image features");
//        delay (500);
       return p;
     default:
//     lcd.clear();
//       lcd.print("Unknown error");
//        delay (500);
       return p;
   }

  lcd.clear();
  lcd.print("Remove finger");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place again ");
  lcd.setCursor(1,1);
  lcd.print("same finger");
  delay (1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
      lcd.print("Image taken");
       delay (1000);    
      break;
    case FINGERPRINT_NOFINGER:
    lcd.clear();
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
    lcd.clear();
      lcd.print("Communication error");
       delay (500);    
      break;
    case FINGERPRINT_IMAGEFAIL:
    lcd.clear();
      lcd.print("Imaging error");
       delay (500);
      break;
    default:
    lcd.clear();
      lcd.print("Unknown error");
       delay (500);
      break;
    }
  }

  // OK success!

   p = finger.image2Tz(2);
   switch (p) {
     case FINGERPRINT_OK:
  //   lcd.clear();
  //     lcd.print("Image converted");
  //      delay (1000);
       break;
     case FINGERPRINT_IMAGEMESS:
  //   lcd.clear();
  //     lcd.print("Image too messy");
  //      delay (500);
       return p;
     case FINGERPRINT_PACKETRECIEVEERR:
  //   lcd.clear();
  //     lcd.print("Communication error");
  //      delay (500);
       return p;
     case FINGERPRINT_FEATUREFAIL:
  //   lcd.clear();
  //     lcd.print("Could not find");
  //     lcd.setCursor(0,1);
  //     lcd.print("fingerprint features");
  //      delay (500);
       return p;
     case FINGERPRINT_INVALIDIMAGE:
  //   lcd.clear();
  //     lcd.print("Could not find");
  //     lcd.setCursor(0,1);
  //     lcd.print("fingerprint features");
  //      delay (500);
       return p;
     default:
  //   lcd.clear();
  //     lcd.print("Unknown error");
  //      delay (1000);
      
       return p;
   }

  // OK converted!
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Creating model");
  lcd.setCursor(0,1);  
  lcd.print("ID #");
  lcd.setCursor(5,1);
  lcd.print(id);
  delay (1000);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Prints matched!");
       delay (1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
       delay (800);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.print("Fingerprints did");
    lcd.setCursor(0,1);
    lcd.print("not match");
    delay (2000);
    lcd.clear();
    lcd.print("Please Try Again");
    delay(1000);
    lcd.clear();

    lcd.print("Press A ");
    lcd.setCursor(0,1); 
    lcd.print("for enroll :)");  
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
       delay (800);
    return p;
  }

     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Id #");
     lcd.setCursor(4,0);
     lcd.print(id);
         
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.setCursor(8,1);
    lcd.print("Stored!");
    delay (2000);
    lcd.clear();
    // lcd.print("A)Enroll B)Del");
    // lcd.setCursor(0,1);
    // lcd.print("C)Empty Database");
    // delay(2000);
    // lcd.clear();
    // lcd.print("Press # to");
    // lcd.setCursor(0,1);
    // lcd.print("change password");

    // lcd.print("Press* to Unlock");   //modified
    // lcd.setCursor(0,1);
    // lcd.print("or D to settings");
    // delay(1000);
    // loop();  
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
    delay (800);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.clear();
    lcd.print("Could not store");
    lcd.setCursor(0,1);
    lcd.print("in that location");
    delay (2000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.clear();
    lcd.print("Error writing");
    lcd.setCursor(0,1);
    lcd.print("to flash");
    delay (800);
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
    delay (500);
    return p;
  }

  return true;
}

void del(){          //Deleting Fingerprint
  lcd.clear();
  lcd.print("Type ID #");
  lcd.setCursor(0,1);
  lcd.print("to delete");
  
  uint8_t id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Deleting ID #");
  lcd.setCursor(14,0);
  lcd.print(id) ; 
  delay(1000);
  deleteFingerprint(id);
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Deleted!");
    delay(3000);
    lcd.clear();
    // lcd.print("A)Enroll B)Del");
    // lcd.setCursor(0,1);
    // lcd.print("C)Empty Database");
    // delay(2000);
    // lcd.clear();
    // lcd.print("Press # to");
    // lcd.setCursor(0,1);
    // lcd.print("change password");

    // lcd.print("Press* to Unlock");   //modified
    // lcd.setCursor(0,1);
    // lcd.print("or D to settings");
    // delay(1000);
    // loop();     
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication");
    lcd.setCursor(0,1);
    lcd.print("error");
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.clear();
    lcd.print("Could not delete");
    lcd.setCursor(0,1);
    lcd.print("in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.clear();
    lcd.print("Error writing");
    lcd.setCursor(0,1);
    lcd.print("to flash");
  } else {
    lcd.clear();
    lcd.print("Unknown error:0x");
    lcd.setCursor(0,1);
    lcd.print(p, HEX);
  }

  return p;
}

// void emptydatabase() {   //Deleting full database
//   lcd.clear();
//   lcd.print("Want to clear");
//   lcd.setCursor(0, 1);
//   lcd.print("the Database?");
//   delay(2000);
//   lcd.clear();

//   lcd.print("Enter the");
//   lcd.setCursor(0, 1);
//   lcd.print("Password :)");

//   int letter = readnumber();
//   if (letter == 123) {
//     finger.emptyDatabase();
//     lcd.clear();
//     lcd.print("Database clear");
//     lcd.setCursor(0,1);
//     lcd.print("successful");
//     delay(2000);
//     lcd.clear();
//     lcd.print("A)Enroll B)Del");
//     lcd.setCursor(0,1);
//     lcd.print("C)Empty Database");
//     delay(2000);
//     lcd.clear();
//     lcd.print("Press # to");
//     lcd.setCursor(0,1);
//     lcd.print("change password");
//     delay(1000);  
//   }
//   else {
//     lcd.clear();
//     lcd.print("Wrong Password");
//     lcd.setCursor(0, 1);
//     lcd.print("Fuck Youuuu !!");
//     delay(2000);

//     lcd.clear();
//     lcd.print("Press 'C'");
//     lcd.setCursor(0,1);
//     lcd.print("to restart !!");
//   }
// }
 
void OpenDoor(){             //Lock opening function open for 3s
    lcd.clear();
    lcd.print("Access Granted");
    lcd.setCursor(0,1);
    lcd.print("welcome to C.F.I");
    delay(1000);
    lcd.clear();
    lcd.print("Door is opening");
    digitalWrite(Lock,HIGH);
    delay(5000);
    lcd.clear();
    lcd.print("Door is closing");
    delay(1000);
    digitalWrite(Lock,LOW);
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

// found a match!
//  Serial.print("Found ID #"); Serial.print(finger.fingerID);
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
