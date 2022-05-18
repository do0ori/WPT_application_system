/*
 * When a card is tagged to RFID/NFC module (RC522),
 * operate servo motor only for Authorized card access.
 * (Authorized means the uid of card is stored in EEPROM.)
 * 
 * https://github.com/do0ori/WPT_application_system
 * 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <EEPROM.h>

/***********************************************************************
 *                           PARAMETERS
 **********************************************************************/
#define SS_PIN 10               // spi 통신을 위한 SS(chip select)핀 설정
#define RST_PIN 9               // 리셋 핀 설정 
#define SERVO 7                 // servo motor pin
#define LED_RED 6               // led_red pin
#define LED_GREEN 5             // led_green pin
MFRC522 rfid(SS_PIN, RST_PIN);  // rfid이름으로 mfrc522 객체 생성
Servo servo;                    // servo이름으로 Servo 객체 생성 (자물쇠)
int UNLOCK = 0;                 // servo motor angle for unlocking
int LOCK = 90;                  // servo motor angle for locking
int stand_by = 5;               // setup 카드 인식 대기 시간 [초]

/***********************************************************************
 *                             FUNCTION
 **********************************************************************/
boolean read_card(int duration = 0)
{
  // duration : 카드 인식 대기 시간 [초]
  for (int i = 0; i < duration*10; i++) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      return true;
    }
    delay(100);
  }
  return false;
}

int eeprom_avail_addr(boolean print_enable = true)
{
  if (print_enable) {
    Serial.print("-------------EEPROM------------\n");
    Serial.print("address\t  value\t\tstatus\n");
    for (int addr = 0; addr < EEPROM.length(); addr+=rfid.uid.size) {
      String value = get_uid_string_from_eeprom(addr);  // uid 전체 값
      Serial.print(addr);
      Serial.print("\t  ");
      Serial.print(value);
      Serial.print("\t");
      
      byte val = EEPROM.read(addr); // uid 첫번째 byte
      if (val != 0) {
        Serial.print("full\n");
      } else {
        Serial.print("empty\n");
        Serial.print("-------------------------------\n");
        return addr;
      }
    }
  } else {
    for (int addr = 0; addr < EEPROM.length(); addr+=rfid.uid.size) {
      byte val = EEPROM.read(addr); // uid의 첫번째 byte 읽어옴
      if (val == 0) {
        return addr;
      }
    }
  }
}

String get_uid_string_from_reader()
{
  Serial.print("UID tag :");
  String target_uid= "";
  for (byte i = 0; i < rfid.uid.size; i++) {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     target_uid.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     target_uid.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  target_uid.toUpperCase();
  return target_uid.substring(1);
}

String get_uid_string_from_eeprom(int addr)
{
  String eeprom_uid= "";
  for (byte i = 0; i < rfid.uid.size; i++) {
     eeprom_uid.concat(String(EEPROM.read(addr+i) < 0x10 ? " 0" : " "));
     eeprom_uid.concat(String(EEPROM.read(addr+i), HEX));
  }
  eeprom_uid.toUpperCase();
  return eeprom_uid.substring(1);
}

void eeprom_store_uid(int addr)
{ 
  for (byte i = 0; i < rfid.uid.size; i++) {   
     byte value = rfid.uid.uidByte[i];
     EEPROM.write(addr+i, value);
  }
  Serial.println(">> Card is successfully registered.\n");
}

boolean card_in_eeprom(int addr, String target_uid)
{
  for (byte i = 0; i < addr; i+=rfid.uid.size) {
     String eeprom_uid = get_uid_string_from_eeprom(i);
     if (target_uid == eeprom_uid) {
       return true;
     }
  }
  return false;
}

void lock_system()
{
  /* Check & Toggle the state  */
  /*      LOCK <-> UNLOCK      */
  if (servo.read() == LOCK) {
    servo.write(UNLOCK);
  } else {
    servo.write(LOCK);
  }
  // EEPROM에 servo state 저장
  EEPROM.write(EEPROM.length()-1, servo.read());
}

/***********************************************************************
 *                            MAIN CODE
 **********************************************************************/
void setup() 
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  servo.attach(SERVO);
  servo.write(EEPROM.read(EEPROM.length()-1));  // get last state of servo motor
  Serial.begin(9600);       // 시리얼통신
  SPI.begin();              // SPI 통신 시작
  rfid.PCD_Init();          // RFID(MFRC522) 초기화
  Serial.print(">> Register Card : ");
  Serial.print("Tag your card in ");
  Serial.print(stand_by);
  Serial.print(" seconds...");
  Serial.println();
  
  digitalWrite(LED_RED, HIGH);
  delay(100);
  digitalWrite(LED_GREEN, HIGH);
  boolean reset = read_card(stand_by);   // n초 동안 카드 인식 대기
  if (reset) {
    String target_uid = get_uid_string_from_reader();
    int address = eeprom_avail_addr();
    if (card_in_eeprom(address, target_uid)) {
      Serial.println(">> Card is already registered.\n");
    } else {
      eeprom_store_uid(address);    // EEPROM에 uid 저장
    }
    delay(3000);
  } else {
    Serial.println(">> Card not detected.\n");
  }
  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_GREEN, LOW);
  
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop() 
{  
  // 새카드 접촉이 있을 때만 다음 단계로 넘어감
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }
  // 카드 읽힘이 제대로 되면 다음으로 넘어감
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid_content = get_uid_string_from_reader();
  int address = eeprom_avail_addr(false);
  Serial.print("Message : ");

  if (card_in_eeprom(address, uid_content)) {
    digitalWrite(LED_GREEN, HIGH);
    Serial.println("Authorized access");
    Serial.println();
    lock_system();
    delay(2000); // 카드 접촉 및 연속체크 시간에 대한 딜레이 주기
    digitalWrite(LED_GREEN, LOW);
  } else {
    Serial.println(" Access denied");
    digitalWrite(LED_RED, HIGH);
    delay(2000); // 카드 접촉 및 연속체크 시간에 대한 딜레이 주기
    digitalWrite(LED_RED, LOW);
  }
}
