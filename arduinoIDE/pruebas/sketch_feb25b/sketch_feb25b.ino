#define HALL0 33
#define HALL1 25
#define HALL2 26
#define HALL3 27

void setup() {
  Serial.begin(9600);
  // Initialize the HALL sensors
  pinMode(HALL0, INPUT);
  pinMode(HALL1, INPUT);
  pinMode(HALL2, INPUT);
  pinMode(HALL3, INPUT);
}

void loop() {
  // Read the HALL sensors
  int hall0 = digitalRead(HALL0);
  int hall1 = digitalRead(HALL1);
  int hall2 = digitalRead(HALL2);
  int hall3 = digitalRead(HALL3);
  // Calculate the direction with the HALL sensors
  if (hall0 == 1 && hall1 == 1 && hall2 == 0 && hall3 == 0) {
    Serial.println("N");
  } else if (hall0 == 0 && hall1 == 1 && hall2 == 1 && hall3 == 0) {
    Serial.println("NE");
  } else if (hall0 == 0 && hall1 == 0 && hall2 == 1 && hall3 == 1) {
    Serial.println("E");
  } else if (hall0 == 0 && hall1 == 0 && hall2 == 0 && hall3 == 1) {
    Serial.println("SE");
  }
    else if (hall0 == 1 && hall1 == 0 && hall2 == 0 && hall3 == 0) {
        Serial.println("S");
    } else if (hall0 == 1 && hall1 == 1 && hall2 == 0 && hall3 == 0) {
        Serial.println("SW");
    } else if (hall0 == 1 && hall1 == 0 && hall2 == 0 && hall3 == 0) {
        Serial.println("W");
    } else if (hall0 == 1 && hall1 == 0 && hall2 == 0 && hall3 == 1) {
        Serial.println("NW");
    } else {
        Serial.println("No hay viento");
    }
  delay(1000);
}