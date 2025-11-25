#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

#define LED_PIN 8       // Main indicator LED/Buzzer
#define BUZZER_PIN 7    // Optional buzzer or second LED

// Authorized card UIDs (format: "XX XX XX XX")
String knownIDs[] = {"A3 A1 95 98", "B4 4D 51 21"};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Reader Ready. Scan your card...");
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  // Convert UID to string in the format "XX XX XX XX"
  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uidString += " ";
  }
  uidString.toUpperCase(); // Converts to uppercase, in-place

  Serial.print("Card UID: ");
  Serial.println(uidString);

  // Check if UID is authorized
  bool authorized = false;
  for (int i = 0; i < sizeof(knownIDs) / sizeof(knownIDs[0]); i++) {
    if (uidString == knownIDs[i]) {
      authorized = true;
      break;
    }
  }

  if (authorized) {
    Serial.println("Authorized - Attendance Marked!");
    // Blink LED/buzzer for authorized
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
    digitalWrite(LED_PIN, LOW);
  } else {
    Serial.println("Unauthorized Card!");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000); // long alert for unauthorized
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(2000); // Small delay before next read
}
