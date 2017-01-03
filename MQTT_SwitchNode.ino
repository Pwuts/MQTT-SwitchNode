#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int relay1 = D0;
const int relay2 = D5;
const int relay3 = D6;
const int relay4 = D7;

// Do WiFi and MQTT stuff
const char* ssid      = "whiskeygrid";
const char* password  = "7A6U6QM0RQ0Z";
const char* mqttServ  = "192.168.1.31";
const char* clientID  = "SwitchNode0_0";
const char* debugTopic= "whiskeygrid/debug";
const char* connTopic = "whiskeygrid/debug/connect";
//String callTopics      = "whiskeygrid/switch/node0_0";
const char* listenTopic= "whiskeygrid/switch/node0_0";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  String topicstr(topic);

  Serial.print("Message arrived on topic: [");
  Serial.print(topicstr);
  Serial.print("], ");
  Serial.println(message);
  
  if (length == 2) {
    int switchnr = (int)(payload[0] - '0');
    int onoroff = (int)(payload[1] - '0');
    Serial.print("Switch no.: ");
    Serial.print(switchnr);
    Serial.print(", on/off: ");
    Serial.println(onoroff);
 
    switch (switchnr) {
      case 1:                            //A message "31" will indicate for switch number 3 to turn on (1).
        digitalWrite(relay1, !onoroff);  //The relays are turned on by a low signal, so the 1 is turned into a 0
        break;                           //  (or vice versa) and written to the relay's digital input.
      case 2:
        digitalWrite(relay2, !onoroff);
        break;
      case 3:
        digitalWrite(relay3, !onoroff);
        break;
      case 4:
        digitalWrite(relay4, !onoroff);
        break;
      default:
        sprintf(msg, "%s, ERROR: invalid trigger received: \"%s\"", clientID, payload);
        Serial.println(msg);
        client.publish(debugTopic, msg);
        break;
    }
  } else {
    Serial.println("ERROR: invalid trigger format");
    sprintf(msg, "%s, ERROR: invalid trigger format: \"%s\"", clientID, payload);
    client.publish(debugTopic, msg);
  }
}

void reconnect() {
  int connLoseMillis = millis();
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      int connectMillis = millis();
      float lostSeconds = (connectMillis - connLoseMillis) / 1000;
      char lostSecStr[6];
      dtostrf(lostSeconds, 1, 1, lostSecStr);
      sprintf(msg, "%s (re)connected after %ss", clientID, lostSecStr);
      client.publish(connTopic, msg);
      Serial.println(msg);
      // ... and resubscribe
      client.subscribe(listenTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServ, 1883);
  client.setCallback(callback);
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  delay(10);
  digitalWrite(D0, HIGH);
  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);
  delay(10);
  digitalWrite(D5, HIGH);
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);
  delay(10);
  digitalWrite(D6, HIGH);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, LOW);
  delay(10);
  digitalWrite(D7, HIGH);
  
  delay(300);
  
  digitalWrite(D0, LOW);  //just a sign of life :)
  delay(150);
  digitalWrite(D0, HIGH);
  digitalWrite(D5, LOW);
  delay(150);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, LOW);
  delay(150);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, LOW);
  delay(150);
  digitalWrite(D7, HIGH);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
