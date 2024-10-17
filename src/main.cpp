#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Definições do display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos para sensores de umidade (um para cada planta)
#define SENSOR_UMIDADE_1 34
#define SENSOR_UMIDADE_2 35
#define SENSOR_UMIDADE_3 32
#define SENSOR_UMIDADE_4 33
#define SENSOR_UMIDADE_5 25

// Pinos para relés (um para cada planta e sanitário)
#define RELAY_SANITARIO_PIN 26  // Relé do sanitário
#define RELAY_PLANTA_1 27       // Relé da planta 1
#define RELAY_PLANTA_2 14       // Relé da planta 2
#define RELAY_PLANTA_3 12       // Relé da planta 3
#define RELAY_PLANTA_4 13       // Relé da planta 4
#define RELAY_PLANTA_5 15       // Relé da planta 5

// Pino do sensor PIR (presença para o sanitário)
#define PIR_PIN 23

unsigned long previousMillis = 0;
const long interval = 5000;  // Intervalo de atualização a cada 5 segundos

void setup() {
  // Inicializando pinos
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_SANITARIO_PIN, OUTPUT);
  pinMode(RELAY_PLANTA_1, OUTPUT);
  pinMode(RELAY_PLANTA_2, OUTPUT);
  pinMode(RELAY_PLANTA_3, OUTPUT);
  pinMode(RELAY_PLANTA_4, OUTPUT);
  pinMode(RELAY_PLANTA_5, OUTPUT);

  digitalWrite(RELAY_SANITARIO_PIN, LOW);
  digitalWrite(RELAY_PLANTA_1, LOW);
  digitalWrite(RELAY_PLANTA_2, LOW);
  digitalWrite(RELAY_PLANTA_3, LOW);
  digitalWrite(RELAY_PLANTA_4, LOW);
  digitalWrite(RELAY_PLANTA_5, LOW);

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha ao inicializar o display OLED");
    for (;;) {}  // Trava se não conseguir iniciar o display
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Sistema Iniciado");
  display.display();
  delay(2000);  // Exibe mensagem inicial por 2 segundos
}

void loop() {
  unsigned long currentMillis = millis();

  // Lê o estado do sensor PIR
  int pirState = digitalRead(PIR_PIN);

  // Lê a umidade dos sensores
  int umidade1 = analogRead(SENSOR_UMIDADE_1);
  int umidade2 = analogRead(SENSOR_UMIDADE_2);
  int umidade3 = analogRead(SENSOR_UMIDADE_3);
  int umidade4 = analogRead(SENSOR_UMIDADE_4);
  int umidade5 = analogRead(SENSOR_UMIDADE_5);

  // Converte os valores lidos em porcentagem (exemplo, pode ajustar conforme os sensores)
  float percentual1 = map(umidade1, 0, 4095, 0, 100);
  float percentual2 = map(umidade2, 0, 4095, 0, 100);
  float percentual3 = map(umidade3, 0, 4095, 0, 100);
  float percentual4 = map(umidade4, 0, 4095, 0, 100);
  float percentual5 = map(umidade5, 0, 4095, 0, 100);

  // Alterna a exibição no display a cada X segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Umidade das Plantas:");

    display.print("Planta 1: ");
    display.print(percentual1);
    display.println("%");

    display.print("Planta 2: ");
    display.print(percentual2);
    display.println("%");

    display.print("Planta 3: ");
    display.print(percentual3);
    display.println("%");

    display.print("Planta 4: ");
    display.print(percentual4);
    display.println("%");

    display.print("Planta 5: ");
    display.print(percentual5);
    display.println("%");

    // Lógica de controle do sanitário com base no PIR
    if (pirState == HIGH) {
      display.println("Sanitario: Ativo");
      digitalWrite(RELAY_SANITARIO_PIN, HIGH);  // Aciona o relé do sanitário
    } else {
      display.println("Sanitario: Inativo");
      digitalWrite(RELAY_SANITARIO_PIN, LOW);   // Desliga o relé do sanitário
    }

    // Controle da irrigação com base na umidade para cada planta
    if (percentual1 < 50) {
      digitalWrite(RELAY_PLANTA_1, HIGH);  // Aciona o relé da planta 1 se a umidade estiver abaixo de 50%
    } else {
      digitalWrite(RELAY_PLANTA_1, LOW);   // Desliga o relé da planta 1
    }

    if (percentual2 < 50) {
      digitalWrite(RELAY_PLANTA_2, HIGH);  // Aciona o relé da planta 2
    } else {
      digitalWrite(RELAY_PLANTA_2, LOW);   // Desliga o relé da planta 2
    }

    if (percentual3 < 50) {
      digitalWrite(RELAY_PLANTA_3, HIGH);  // Aciona o relé da planta 3
    } else {
      digitalWrite(RELAY_PLANTA_3, LOW);   // Desliga o relé da planta 3
    }

    if (percentual4 < 50) {
      digitalWrite(RELAY_PLANTA_4, HIGH);  // Aciona o relé da planta 4
    } else {
      digitalWrite(RELAY_PLANTA_4, LOW);   // Desliga o relé da planta 4
    }

    if (percentual5 < 50) {
      digitalWrite(RELAY_PLANTA_5, HIGH);  // Aciona o relé da planta 5
    } else {
      digitalWrite(RELAY_PLANTA_5, LOW);   // Desliga o relé da planta 5
    }

    display.display();  // Atualiza o display
  }
}
