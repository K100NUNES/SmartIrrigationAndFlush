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
#define SENSOR_UMIDADE_JABUTICABEIRA 34
#define SENSOR_UMIDADE_AMOREIRA 35
#define SENSOR_UMIDADE_HORTELA 32
#define SENSOR_UMIDADE_HERA 33

// Pinos para relés (um para cada planta e sanitário)
#define RELAY_SANITARIO_PIN 26  // Relé do sanitário
#define RELAY_JABUTICABEIRA 27  // Relé da Jabuticabeira
#define RELAY_AMOREIRA 14       // Relé da Amoreira
#define RELAY_HORTELA 12        // Relé da Hortelã
#define RELAY_HERA 13           // Relé da Hera

// Pino do sensor PIR (presença para o sanitário)
#define PIR_PIN 23

// Ajustes de umidade para cada planta
#define LIMITE_UMIDADE_JABUTICABEIRA 60
#define LIMITE_UMIDADE_AMOREIRA 55
#define LIMITE_UMIDADE_HORTELA 65
#define LIMITE_UMIDADE_HERA 50

unsigned long previousMillis = 0;
const long interval = 5000;  // Intervalo de atualização a cada 5 segundos

// Tempos de ativação dos relés
unsigned long relayOnTimePlanta = 5000;   // Relés das plantas ativados por 5 segundos
unsigned long relayOnTimeSanitario = 10000;  // Relé do sanitário ativado por 10 segundos

// Temporizadores dos relés das plantas
unsigned long previousMillisRelay1 = 0;
unsigned long previousMillisRelay2 = 0;
unsigned long previousMillisRelay3 = 0;
unsigned long previousMillisRelay4 = 0;

// Variáveis para temporização do sanitário
unsigned long pirDetectedTime = 0;  // Momento em que o PIR detectou presença
bool sanitarioAguardando = false;   // Indica que o sanitário está aguardando para ser acionado
bool sanitarioAtivado = false;      // Indica se o relé do sanitário foi ativado

void setup() {
  // Inicializando pinos
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_SANITARIO_PIN, OUTPUT);
  pinMode(RELAY_JABUTICABEIRA, OUTPUT);
  pinMode(RELAY_AMOREIRA, OUTPUT);
  pinMode(RELAY_HORTELA, OUTPUT);
  pinMode(RELAY_HERA, OUTPUT);

  digitalWrite(RELAY_SANITARIO_PIN, LOW);
  digitalWrite(RELAY_JABUTICABEIRA, LOW);
  digitalWrite(RELAY_AMOREIRA, LOW);
  digitalWrite(RELAY_HORTELA, LOW);
  digitalWrite(RELAY_HERA, LOW);

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
  float percentualJabuticabeira = map(analogRead(SENSOR_UMIDADE_JABUTICABEIRA), 0, 4095, 0, 100);
  float percentualAmoreira = map(analogRead(SENSOR_UMIDADE_AMOREIRA), 0, 4095, 0, 100);
  float percentualHortela = map(analogRead(SENSOR_UMIDADE_HORTELA), 0, 4095, 0, 100);
  float percentualHera = map(analogRead(SENSOR_UMIDADE_HERA), 0, 4095, 0, 100);

  // Alterna a exibição no display a cada X segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Umidade das Plantas:");

    display.print("Jabuticabeira: ");
    display.print(percentualJabuticabeira);
    display.println("%");

    display.print("Amoreira: ");
    display.print(percentualAmoreira);
    display.println("%");

    display.print("Hortela: ");
    display.print(percentualHortela);
    display.println("%");

    display.print("Hera: ");
    display.print(percentualHera);
    display.println("%");

    // Controle de irrigação baseado nas necessidades de cada planta
    if (percentualJabuticabeira < LIMITE_UMIDADE_JABUTICABEIRA) {
      digitalWrite(RELAY_JABUTICABEIRA, HIGH);  // Ativa o relé da Jabuticabeira
      previousMillisRelay1 = currentMillis;  // Armazena o tempo de ativação
    }
    if (currentMillis - previousMillisRelay1 >= relayOnTimePlanta) {
      digitalWrite(RELAY_JABUTICABEIRA, LOW);   // Desativa o relé após 5 segundos
    }

    if (percentualAmoreira < LIMITE_UMIDADE_AMOREIRA) {
      digitalWrite(RELAY_AMOREIRA, HIGH);  // Ativa o relé da Amoreira
      previousMillisRelay2 = currentMillis;
    }
    if (currentMillis - previousMillisRelay2 >= relayOnTimePlanta) {
      digitalWrite(RELAY_AMOREIRA, LOW);   // Desativa o relé após 5 segundos
    }

    if (percentualHortela < LIMITE_UMIDADE_HORTELA) {
      digitalWrite(RELAY_HORTELA, HIGH);  // Ativa o relé da Hortelã
      previousMillisRelay3 = currentMillis;
    }
    if (currentMillis - previousMillisRelay3 >= relayOnTimePlanta) {
      digitalWrite(RELAY_HORTELA, LOW);   // Desativa o relé após 5 segundos
    }

    if (percentualHera < LIMITE_UMIDADE_HERA) {
      digitalWrite(RELAY_HERA, HIGH);  // Ativa o relé da Hera
      previousMillisRelay4 = currentMillis;
    }
    if (currentMillis - previousMillisRelay4 >= relayOnTimePlanta) {
      digitalWrite(RELAY_HERA, LOW);   // Desativa o relé após 5 segundos
    }

    display.display();  // Atualiza o display
  }

  // Lógica de controle do sanitário com base no PIR
  if (pirState == HIGH) {
    pirDetectedTime = currentMillis;  // Armazena o tempo de detecção
    sanitarioAguardando = true;       // O sanitário aguarda ser acionado
  }

  // Sanitário será ativado 1 minuto após a detecção do PIR, se o pet não estiver presente
  if (sanitarioAguardando && pirState == LOW && (currentMillis - pirDetectedTime >= 60000)) {
    digitalWrite(RELAY_SANITARIO_PIN, HIGH);  // Aciona o relé do sanitário
    sanitarioAtivado = true;
    sanitarioAguardando = false;  // Cancela a espera
  }

  // Desliga o relé do sanitário após 10 segundos
  if (sanitarioAtivado && (currentMillis - pirDetectedTime >= 60000 + relayOnTimeSanitario)) {
    digitalWrite(RELAY_SANITARIO_PIN, LOW);  // Desativa o relé
    sanitarioAtivado = false;
  }
}
