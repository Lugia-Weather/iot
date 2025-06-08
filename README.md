# Lugia Weather – Monitoramento de Nível de Água com ESP32

## Descrição da Solução

O **Lugia Weather** é um sistema IoT para monitoramento de nível de água em bueiros, utilizando ESP32, sensor ultrassônico (HC-SR04) e LEDs indicadores. Os dados coletados são enviados via MQTT para o [ThingSpeak](https://thingspeak.mathworks.com/channels/2976892), permitindo acompanhamento remoto em tempo real.

O sistema indica o status do nível de água (NORMAL, ATENÇÃO, ALAGADO) por meio de LEDs e envia informações detalhadas para o broker MQTT, facilitando a análise e tomada de decisão.

---

## Imagem do Diagrama de Montagem

![Diagrama de Montagem](diagram.png)

---

## Imagem do Dashboard ThingSpeak

![Dashboard ThingSpeak](dashboard.png)

---

## Estrutura do Projeto

```
Lugia Weather/
├── LICENSE
├── platformio.ini
├── q1/
│   ├── diagram.json
│   ├── wokwi.toml
│   └── src/
│       └── main.cpp
├── diagram.png
├── dashboard.png
└── README.md
```

---

## Instruções de Configuração, Execução e Testes

### 1. Pré-requisitos

- [PlatformIO](https://platformio.org/) instalado (VSCode recomendado)
- Placa ESP32 DevKit C V4
- Sensor ultrassônico HC-SR04
- LEDs (vermelho, amarelo, verde, ciano) e resistores
- Acesso à internet para envio MQTT

### 2. Clonando o Repositório

```sh
git clone https://github.com/seu-usuario/lugia-weather.git
cd lugia-weather
```

### 3. Configuração do Ambiente

- Abra o projeto no VSCode com PlatformIO.
- Confira as configurações em [`platformio.ini`](platformio.ini).
- Ajuste o SSID e senha do Wi-Fi em [`q1/src/main.cpp`](q1/src/main.cpp) se necessário.

### 4. Montagem do Hardware

- Siga o diagrama em [`q1/diagram.json`](q1/diagram.json) ou a imagem ilustrativa.
- Conecte o HC-SR04 aos pinos 21 (TRIG) e 22 (ECHO) do ESP32.
- Conecte os LEDs aos pinos 25 (vermelho), 26 (amarelo), 27 (verde) e 14 (ciano).

### 5. Compilação e Upload

- Conecte o ESP32 ao computador.
- No PlatformIO, clique em **Upload** ou use:
  ```sh
  pio run --target upload
  ```

### 6. Execução e Testes

- Abra o monitor serial (baud 115200) para acompanhar logs.
- O dispositivo irá conectar ao Wi-Fi, enviar dados MQTT e indicar status com LEDs.
- Os dados podem ser visualizados no [ThingSpeak - Canal 2976892](https://thingspeak.mathworks.com/channels/2976892).

---

## Dados Enviados via MQTT

A cada 10 segundos, o dispositivo envia para o ThingSpeak os seguintes dados no payload MQTT:

- **field1**: Nome do projeto (`lugia_weather`)
- **field2**: ID do módulo IoT (`esp32_bueiro_1`)
- **field3**: IP local do Wi-Fi
- **field4**: MAC Address do ESP32
- **field5**: Distância medida pelo sensor (cm)
- **field6**: Nível de água calculado (cm)
- **field7**: Status do nível de água (0 = NORMAL, 1 = ATENÇÃO, 2 = ALAGADO)

Exemplo de payload:

```
field1=lugia_weather&field2=esp32_bueiro_1&field3=192.168.0.10&field4=AA:BB:CC:DD:EE:FF&field5=150.00&field6=50.00&field7=1
```

---

## Dados Impressos no Serial Monitor

O monitor serial exibe, a cada ciclo:

- Nível da água (cm)
- Distância medida (cm)
- Status textual (NORMAL, ATENÇÃO, ALAGADO)
- Payload enviado via MQTT (formatado)
- Mensagens de conexão Wi-Fi e MQTT
- Mensagens de erro em caso de falha de envio

---

## Possíveis Erros Tratados

- **Falha na conexão Wi-Fi**: O sistema tenta reconectar automaticamente.
- **Falha na conexão MQTT**: O sistema tenta reconectar automaticamente ao broker.
- **Falha ao enviar mensagem MQTT**: Mensagem de erro exibida no serial, com código de erro MQTT.
- **Erro de leitura do sensor**: Se não houver resposta do sensor, retorna -1 e o valor não é considerado válido.

---

## Apresentação do Código-Fonte

O código está em [`q1/src/main.cpp`](q1/src/main.cpp), com comentários detalhados em cada função:

- **Conexão Wi-Fi e MQTT**: Funções `initWifi()`, `initMQTT()`, `verificaConexoes()`
- **Leitura do sensor ultrassônico**: Função `leituraDistanciaCm()`
- **Cálculo do nível de água**: Função `calcularNivelAguaCm()`
- **Indicação por LEDs**: Controle dos pinos 25, 26, 27 e 14
- **Envio de dados via MQTT**: Função `enviaEstadoOutputMQTT()`
- **Tratamento de erros**: Mensagens e reconexão automática

O código está organizado, comentado e pronto para replicação.

---

## Licença

Este projeto está licenciado sob a Licença MIT. Veja o arquivo [`LICENSE`](LICENSE) para mais detalhes.

## 👨‍💻 Autor

### **Júlio César Nunes Oliveira - RM557774 2TDSFPF** - [GitHub](https://github.com/jubshereman)

### **Erik Paschoalatto dos Santos - RM554854 2TDSFPF** - [GitHub](https://github.com/ozerikoz)

### **Nathan Magno Gustavo Consolo - RM558987 2TDSFPF** - [GitHub](https://github.com/NathanMagno)
