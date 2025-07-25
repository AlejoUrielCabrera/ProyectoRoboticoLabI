#include "setup.h"

bool modoImagen = false;
String bufferComando = "";  // 🧠 Acumular los caracteres recibidos

void setup() {
  CLKPR = 0x80; // 🔧 WAVGAT: permitir cambio de velocidad
  CLKPR = 0;    // 🔧 Velocidad al máximo
  initializeScreenAndCamera();
}

void loop() {
  // --- Leer comando byte a byte, hasta '\n' ---
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      bufferComando.trim();  // Elimina espacios y saltos previos

      if (bufferComando == "IMG") {
        modoImagen = true;
      } else if (bufferComando == "STOPIMG") {
        modoImagen = false;

        // 🧹 Limpiar cualquier residuo de datos
        delay(200);
        while (Serial.available()) Serial.read();
      }

      bufferComando = "";  // Limpiar para el próximo comando
    } else {
      // Solo acumular si no es basura
      if (isPrintable(c)) {
        bufferComando += c;
      }
    }
  }

  // --- Procesar imagen si corresponde ---
  if (modoImagen) {
    processFrame();
    delay(500);
  }
}