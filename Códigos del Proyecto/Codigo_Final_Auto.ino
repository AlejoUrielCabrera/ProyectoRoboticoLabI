#include <SoftwareSerial.h>

// Pines motores
const int m1a = 10;
const int m1b = 9;
const int m2a = 12;
const int m2b = 11;

SoftwareSerial BTSerial(2, 3);  // RX, TX

bool modoImagen = false;
char val;

#define FRAME_START 0x02
#define FRAME_END   0x03

// Control de envío en modoImagen
bool enviandoFrame = false;

void setup() {
  delay(1000);

  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);

  Serial.begin(115200);    // UART hacia cámara
  BTSerial.begin(115200);  // Bluetooth HC-06
}

void loop() {
  // --- Siempre escuchar comandos Bluetooth ---
  if (BTSerial.available()) {
    String comando = BTSerial.readStringUntil('\n');
    comando.trim();

    if (comando == "IMG") {
      modoImagen = true;
      limpiarBufferSerial();
      limpiarBufferBT();
      Serial.println("IMG");
      enviandoFrame = false;  // arranca sin frame parcial
    } 
    else if (comando == "STOPIMG") {
      modoImagen = false;
      enviandoFrame = false;
      Serial.println("STOPIMG");
      delay(200);
      limpiarBufferSerial();
      limpiarBufferBT();
    }
    else if (!modoImagen && comando.length() > 0) {
      val = comando.charAt(0);
      manejarAuto();
    }
  }

  // --- Forward en MODO IMAGEN ---
  if (modoImagen) {
    if (!enviandoFrame && Serial.available()) {
      // inicio de nuevo frame
      BTSerial.write(FRAME_START);
      enviandoFrame = true;
    }

    // reenviar en chunks pequeños
    int chunkSize = 64; // Ajustable: 16, 32, 64...
    while (modoImagen && Serial.available() && chunkSize-- > 0) {
      BTSerial.write(Serial.read());
    }

    // Si no quedan más datos de la cámara, cerramos el frame
    if (enviandoFrame && !Serial.available()) {
      BTSerial.write(FRAME_END);
      enviandoFrame = false;
    }
  } 
  else {
    // NO está en modo imagen => descartar basura
    if (Serial.available()) {
      limpiarBufferSerial();
    }
  }
}

// --- Limpiar buffers ---
void limpiarBufferSerial() {
  while (Serial.available()) Serial.read();
}

void limpiarBufferBT() {
  while (BTSerial.available()) BTSerial.read();
}

// --- Control del auto ---
void manejarAuto() {
  if (val == 'F') {
    digitalWrite(m1a, LOW);  digitalWrite(m1b, HIGH);
    digitalWrite(m2a, HIGH); digitalWrite(m2b, LOW);
  } 
  else if (val == 'B') {
    digitalWrite(m1a, HIGH); digitalWrite(m1b, LOW);
    digitalWrite(m2a, LOW);  digitalWrite(m2b, HIGH);
  } 
  else if (val == 'L') {
    digitalWrite(m1a, LOW);  digitalWrite(m1b, LOW);
    digitalWrite(m2a, HIGH); digitalWrite(m2b, LOW);
  } 
  else if (val == 'R') {
    digitalWrite(m1a, LOW);  digitalWrite(m1b, HIGH);
    digitalWrite(m2a, LOW);  digitalWrite(m2b, LOW);
  } 
  else if (val == 'S') {
    digitalWrite(m1a, LOW);  digitalWrite(m1b, LOW);
    digitalWrite(m2a, LOW);  digitalWrite(m2b, LOW);
  }
}
