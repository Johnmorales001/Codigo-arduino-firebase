#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Credenciales de WiFi
#define WIFI_SSID "NETLIFE-rmhjvmoraless1"
#define WIFI_PASSWORD "1707988547"

// Configuración de Firebase
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

// URL y clave de Firebase
#define FIREBASE_HOST "https://motor-trifasico-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "YqN1yolAD0ieWuIh22T9Srbb0PHnVYV8YBJ07Qzs"

// Pines de sensores
#define PIN_CORRIENTE 34  // Pin del sensor de corriente
#define PIN_VOLTAJE 35    // Pin del sensor de voltaje

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128  // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 64   // Alto de la pantalla OLED
#define OLED_RESET    -1   // Reset (-1 si no se usa un pin específico)

// Instancia de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Factores de conversión
const float FACTOR_CONVERSION_VOLT = 3.3 / 4095.0;  
const float FACTOR_CONVERSION_AMP = 3.3 / 4095.0;   

void setup() {
    Serial.begin(115200);

    // Inicializar pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección I2C 0x3C
        Serial.println("⚠️ Error al inicializar la pantalla OLED ⚠️");
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("Iniciando...");
    display.display();
    delay(2000);

    // Conectar a WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando a WiFi...");
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Conectando WiFi...");
    display.display();

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConectado a WiFi");
    
    // Mostrar mensaje en OLED
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("WiFi conectado");
    display.display();
    delay(1000);

    // Configurar Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Conectado a Firebase");

    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Firebase OK");
    display.display();
    delay(1000);
}

void loop() {
    // Leer ADC de sensores
    int lecturaCorriente = analogRead(PIN_CORRIENTE);
    int lecturaVoltaje = analogRead(PIN_VOLTAJE);

    // Verificar si las lecturas son válidas
    if (lecturaCorriente == 0 || lecturaVoltaje == 0) {
        Serial.println("⚠️ Error en la lectura de los sensores ⚠️");
        return;
    }

    // Convertir lecturas a unidades reales
    float voltaje = lecturaVoltaje * FACTOR_CONVERSION_VOLT;
    float corriente = lecturaCorriente * FACTOR_CONVERSION_AMP;

    // Imprimir valores en el monitor serie
    Serial.print("Voltaje: ");
    Serial.print(voltaje);
    Serial.print(" V | Corriente: ");
    Serial.print(corriente);
    Serial.println(" A");

    // Enviar datos a Firebase
    Firebase.setFloat(firebaseData, "/medicion/voltaje", voltaje);
    Firebase.setFloat(firebaseData, "/medicion/corriente", corriente);

    // Mostrar datos en la pantalla OLED
    display.clearDisplay();
    display.setCursor(10, 10);
    display.print("Voltaje: ");
    display.print(voltaje);
    display.println(" V");
    
    display.setCursor(10, 30);
    display.print("Corriente: ");
    display.print(corriente);
    display.println(" A");

    display.display();

    delay(1000);  // Esperar 1 segundo antes de la siguiente lectura
}
