/********************************************************************
 * Projeto: Envio de Dados MQTT com ESP32 - Global Solution 2025
 * Autor: Grupo Lugia Weather
 * Placa: ESP32-DEVKIT-C-V4
 * 
 * Descrição:
 * Este projeto conecta o ESP32 a uma rede Wi-Fi e a um Broker MQTT.
 * A cada 10 segundos, envia uma mensagem payload String contendo:
 * - Nome do Projeto
 * - ID do modulo iot
 * - Wifi IP local
 * - Wifi Mac Address
 * - Distância medida pelo sensor (cm)
 * - Nivel de água (cm)
 * - Status de acordo com o nível de água (NORMAL, ATENÇÃO OU ALAGADO)
 * 
 * Baseado no repositório:
 * https://github.com/prof-atritiack/2TDS-GS2025
 * Professor André Tritiack - FIAP
 ********************************************************************/

//----------------------------------------------------------
// Bibliotecas já disponíveis no ambiente ESP32

#include <WiFi.h>

//----------------------------------------------------------
// Bibliotecas a instalar pelo Gerenciador de Bibliotecas

#include <PubSubClient.h>

//----------------------------------------------------------
// Definições e configurações

#define HC_ECHO_PIN 22
#define HC_TRIG_PIN 21

#define LED_RED 25 // LED VERMELHO 
#define LED_YELLOW 26 // LED AMARELO
#define LED_GREEN 27 // LED VERDE
#define LED_CYAN 14 // LED CIANO

// Identificadores
const char* projeto = "lugia_weather";
const char* module_id  = "esp32_bueiro_1";

// Wi-Fi
const char* ssid = "Wokwi-GUEST"; // Rede Wi-Fi local (deixar "Wokwi-GUEST" para utilizar a rede simulada do Wokwi)
const char* password = ""; // Senha do Wi-Fi local (deixar "" para usar o Wokwi-GUEST)

// ThingSpeak MQTT
const char* mqtt_server = "mqtt3.thingspeak.com";
const int   mqtt_port = 1883;

// Credenciais do ThingSpeak 
const char* channel_id = "2976892";         
const char* client_id = "MhIcPREBFTwBIhcCICAABCU"; 
const char* mqtt_username = "MhIcPREBFTwBIhcCICAABCU";  
const char* mqtt_password = "8iD7BQmFrBOzPj9r57SoKG7n";

// Tópico MQTT
String topic = "channels/" + String(channel_id) + "/publish";

//----------------------------------------------------------
// Variáveis globais

const float altura_maxima = 200.0;
const char* status_labels[] = {"NORMAL", "ATENÇÃO", "ALAGADO"};

WiFiClient espClient;
PubSubClient MQTT(espClient);

//----------------------------------------------------------
// Conexão Wi-Fi

void initWifi() {
  Serial.println("\n============== CONEXÃO WI-FI ==============");

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado com sucesso!");
  Serial.print("IP Local: ");
  Serial.println(WiFi.localIP().toString());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("===========================================\n");
}


void reconectaWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[Wi-Fi] Reconectando...");
    initWifi();
  }
}

//----------------------------------------------------------
// Conexão MQTT

void initMQTT() {
  Serial.println("\n============= CONEXÃO MQTT =============");
  MQTT.setServer(mqtt_server, mqtt_port);

  while (!MQTT.connected()) {
    Serial.println("Conectando ao broker MQTT... ");
    if (MQTT.connect(client_id, mqtt_username, mqtt_password)) {
      Serial.println("Conectado com sucesso!");
    } else {
      Serial.print("Falha (Código ");
      Serial.print(MQTT.state());
      Serial.println("). Tentando novamente em 2 segundos...");
      delay(2000);
    }
  }
  Serial.println("========================================\n");
}


void verificaConexoes() {
  bool conectado = true;

  if (WiFi.status() != WL_CONNECTED) {
    initWifi();
    conectado = false;
  }

  if (!MQTT.connected()) {
    initMQTT();
    conectado = false;
  }

  // LED CIANO aceso quando tudo está conectado, apagado caso contrário
  digitalWrite(LED_CYAN, conectado ? HIGH : LOW);

  MQTT.loop();
}

//----------------------------------------------------------
// Envio e feedback
void enviaEstadoOutputMQTT(const String& payload) {
  bool enviado = MQTT.publish(topic.c_str(), payload.c_str());

  if (enviado) {
    Serial.println("\n============= MQTT ENVIADO =============");

    // Lógica para exibir o payload formatado
    String linha = "";
    for (int i = 0; i < payload.length(); i++) {
      if (payload[i] == '&') {
        Serial.println(linha);
        linha = "";
      } else {
        linha += payload[i];
      }
    }

    // Exibe a última linha
    if (linha.length() > 0) {
      Serial.println(linha);
    }

    Serial.println("========================================\n");
  } else {
    Serial.println("\n[ERRO] Falha ao enviar mensagem MQTT.\n");
    Serial.print("[MQTT] Código de erro: ");
    Serial.println(MQTT.state());
  }
}

//----------------------------------------------------------
// Função para inicializar os pinos

void setupPinos(){
  // PINOS DO HC - SR04
  pinMode(HC_TRIG_PIN, OUTPUT);
  pinMode(HC_ECHO_PIN, INPUT);

  // PINOS DOS LEDS
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_CYAN, OUTPUT);
}

//----------------------------------------------------------
// Função para ler a distância em centímetros usando o sensor HC-SR04.

float leituraDistanciaCm() {
  // Garante que o TRIG esteja em nível baixo antes de enviar o pulso
  digitalWrite(HC_TRIG_PIN, LOW);
  delayMicroseconds(5); // espera curta para estabilizar

  // Envia pulso de 10 microssegundos para o sensor iniciar a medição
  digitalWrite(HC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(HC_TRIG_PIN, LOW);

  // Mede o tempo em que o pino ECHO ficou em nível alto (em microssegundos)
  long duration = pulseIn(HC_ECHO_PIN, HIGH, 30000); // timeout de 30ms evita travamentos

  // Se não houver resposta dentro do tempo, retorna -1 (sem obstáculo detectado)
  if (duration == 0) return -1;

  // Converte o tempo em distância (cm)
  float distance = duration * 0.034 / 2;
  
  return distance;
}

//----------------------------------------------------------
// Função para calcular o nivel de agua do local com base na altura do local, posicionamento e leitura do sensor.

float calcularNivelAguaCm(float distancia, float altura_maxima) {
  
  if (distancia < 0) return -1; // erro na leitura 

  float nivel_agua = altura_maxima - distancia;

  if (nivel_agua < 0) return -1;

  return nivel_agua;
}

//----------------------------------------------------------
// Setup inicial

void setup() {
  Serial.begin(115200);
  setupPinos();
  initWifi();
  initMQTT();

}

//----------------------------------------------------------
// Loop principal

void loop() {
  verificaConexoes();

  float distancia = leituraDistanciaCm();

  float nivel_agua = calcularNivelAguaCm(distancia, altura_maxima);

  // Define os status conforme o nível de água
  bool normal = nivel_agua <= (altura_maxima * 0.75); // NORMAL se o nivel de agua estiver menor que 60% da altura maxima do local
  bool atencao = nivel_agua > (altura_maxima * 0.75) && nivel_agua < (altura_maxima * 0.95); // ATENÇÃO se estivar acima de 80% da altura maxima do local
  bool alagado = nivel_agua >= (altura_maxima * 0.95); // ALAGADO se estiver acima de 95% da altura maxima do local

  int status_code = 0;

  if (alagado) {
    status_code = 2;
  } else if (atencao) {
    status_code = 1;
  } else {
    status_code = 0;
  };

  // Liga o LED do status correspondente e apaga os outros
  digitalWrite(LED_GREEN, normal ? HIGH : LOW);
  digitalWrite(LED_YELLOW, atencao ? HIGH : LOW);
  digitalWrite(LED_RED, alagado ? HIGH : LOW);

  // Serial Monitor
  Serial.println("\n============= PRINCIPAIS DADOS =============");

  Serial.print("Nível da água: ");
  Serial.print(nivel_agua);
  Serial.println(" cm");

  Serial.print("Distância medida: ");
  Serial.print(distancia);
  Serial.println(" cm");

  Serial.print("Status: ");
  Serial.println(String(status_labels[status_code]));

  Serial.println("========================================\n");


  // Preparação do payload para envio por MQTT
  String ip = WiFi.localIP().toString();
  String mac = WiFi.macAddress();

  String payload = "field1=" + String(projeto) +
                  "&field2=" + String(module_id) +
                  "&field3=" + ip +
                  "&field4=" + mac +
                  "&field5=" + String(distancia) +
                  "&field6=" + String(nivel_agua) +
                  "&field7=" + status_code;
                  
  enviaEstadoOutputMQTT(payload);

  delay(10000);
}
