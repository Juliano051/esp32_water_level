
/*******************************************************************
    A telegram bot for your ESP8266 that controls the
    onboard LED. The LED in this example is active low.
    Parts:
    D1 Mini ESP8266 * - http://s.click.aliexpress.com/e/uzFUnIe
    (or any ESP8266 board)
      = Affilate
    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// The version of ESP8266 core needs to be 2.5 or higher
// or else your bot will not connect.

// ----------------------------
// Standard ESP8266 Libraries
// ----------------------------

//#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// A função de manipulação do json no script usa a versão 5 do ArduinoJson.h
// Testes feitos com a UniversalTelegramBot versão 1.1
// ----------------------------
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "SALA";     // your network SSID (name)
char password[] = "S3V3rino"; // your network key

// Initialize Telegram BOT
#define BOTtoken "1259208372:AAGeauufh6eV4SvOmTY7P2pdIo5DZFCB4pY"  // sua chave Token Bot

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
const char* TELEGRAM_CERTIFICATE_ROOT = \
          "-----BEGIN CERTIFICATE-----\n" \
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\n" \
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\n" \
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\n" \
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\n" \
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\n" \
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\n" \
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\n" \
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\n" \
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\n" \
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\n" \
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\n" \
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\n" \
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\n" \
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\n" \
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\n" \
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\n" \
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\n" \
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\n" \
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\n" \
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\n" \
"4uJEvlz36hz1\n" \
"-----END CERTIFICATE-----\n";

const int ledPin = LED_BUILTIN;
int ledStatus = 0;

//---------------------------------------------//
/// Basta definir nesta linha a quantidade  ////
//  de sensores a serem utilizados;        ////
//  o programa em si calcula o percentual ////
// ----------------------------------------//
// >>>>>>>>> Portas D1 a D8 <<<<<<<<<<<< ///
int sensorPorts[] = {5, 4, 0, 2, 14, 12, 13, 15}; // Portas D1 a D8
//int sensorPorts[] = {32, 34}; // 39, 34, 35, 32};
//int sensorPorts[] = {16, 5, 4, 0};
const int lenPorts ((int)sizeof(sensorPorts) / (int)sizeof(sensorPorts[0])); // quantidade de portas usadas
static int reading[lenPorts];

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.print("Message received - Chat ID: ");
    Serial.println(chat_id);
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/Led On" || text == "/ledon") {
      digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/Led Off" || text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, HIGH);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status" || text == "/Status") {
      if (ledStatus) {
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/Nivel" || text == "/nivel") {
      int percentil = 0;
      char temp [2];
      String msg_nivel = "";
      // Preenche uma msg de 10 caracteres simulando o nível da caixa. ex: █████████▒▒▒▒ 60%
      //static int reading[lenPorts];
      //Serial.println("reading<i> antes do reading");
      for (int i = 0; i < lenPorts; i++) {
        //Serial.println(reading[i]);
        
      }
      readSensors(); //<<<<<<<<<<<<<<<<<<<<<< descomentar essa linha para produção
      //int reading[] = {1, 1, 1, 1, 1, 0, 1, 0, 0, 0} ; //<<<<<<<<<<<<<<<<<<<<<< simulação de falha no sensor nº 6
      //int reading[] = {1, 1, 1, 1, 0} ; //<<<<<<<<<<<<<<<<<<<<<< simulação de falha no sensor nº 6
      //Serial.println("reading<i> depois do reading");
      for (int i = 0; i < lenPorts; i++) {
        //Serial.println(reading[i]);
        if (reading[i] == 0) {
          percentil++;
          msg_nivel = msg_nivel + "█";
        } else msg_nivel = msg_nivel + "▒";
      }
      msg_nivel = msg_nivel + " " + itoa(percentil * (100 / lenPorts), temp, 10) + "%";
      Serial.println(msg_nivel);
      Serial.println(chat_id);
      bot.sendMessage(chat_id, msg_nivel, "");
    }

    if (text == "/start") {
      String welcome = "Bem-vindo, " + from_name + ".\n";
      welcome += "Este é o bot de monitoramento do seu reservatório de água.\n\n";
      welcome += "/ledon : Liga o LED\n";
      welcome += "/ledoff : Desliga o LED\n";
      welcome += "/status : Retorna o estado atual do LED\n";
      welcome += "/Nivel : Exibe o nível de água no reservatório\n";
      String keyboardJson = "[[\"/Led On\", \"/Led Off\"],[\"/Status\"],[\"/Nivel\"]]";
      bot.sendMessage(chat_id, welcome, "Markdown");
      bot.sendMessageWithReplyKeyboard(chat_id, "Escolha uma opção abaixo:", "", keyboardJson, true);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // This is the simplest way of getting this working
  // if you are passing sensitive information, or controlling
  // something important, please either use certStore or at
  // least client.setFingerPrint

  //const uint8_t fingerprint[20] = {0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32 , 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90};
  //client.setFingerprint(fingerprint);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  //client.setInsecure();

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < lenPorts; i++) {
    pinMode(sensorPorts[i], INPUT_PULLUP); // Set all ports as inputs
    Serial.println(sensorPorts[i]);

  }

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off (active high)
}


int readSensors() {
  //Lê as portas do sensor de nível e retorna um array binário
  int r[lenPorts];
  for (int i = 0; i < lenPorts; i++) {
    reading[i] = digitalRead(sensorPorts[i]);
  }
  return 0;
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}
