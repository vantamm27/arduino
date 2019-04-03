

/* ESP8266 AWS IoT example by Evandro Luis Copercini
   Public Domain - 2017
    It connects to AWS IoT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
  -WARNING: this example doesn't verify the server CA due low heap, this can be a security issue
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //Get it from here: https://github.com/knolleary/pubsubclient
#include <Wire.h>
#include "DHT.h"
#include <BH1750.h>
#include <ArduinoJson.h>
#include "DHT.h"




/* convert and replace with your keys
  $ openssl x509 -in aaaaaaaaa-certificate.pem.crt -out certificates/cert.der -outform DER
  $ openssl rsa -in aaaaaaaaaa-private.pem.key -out certificates/private.der -outform DER
  $ xxd -i certificates
*/



unsigned char certificates_cert_der[] = { 
};
unsigned int certificates_cert_der_len = 1140;



unsigned char certificates_key_der[] = {
};
unsigned int certificates_key_der_len = 1192;


unsigned char certificates_ca_der[] = {
};
unsigned int certificates_ca_der_len = 953;


#define RELAY_LIGHT  12     // den
#define RELAY_FAN  13     // quat

#define DHTPIN 14     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

StaticJsonDocument<200> doc;
BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

const char* endpoint = "iot-s9.vinadata.vn"; //MQTT broker ip
const char* ssid = "carot";
const char* passWord = "carotcarot";
bool LED_VALUE = HIGH;


String g_macAddr = "";

const String g_hubid = "";

String varTem = "nhiet_do";
//String varHim = "do_am";
//String varLux = "anh_sang";




void callback(char* topic, byte* payload, unsigned int length) {

  //Print values.

}


WiFiClientSecure espClient;
PubSubClient client(endpoint, 8883, callback, espClient); //set  MQTT port number to 8883
long lastMsg = 0;
char msg[50];
float lastValue = 0.0;

void setup() {
  Serial.begin (115200);
  WiFi.begin(ssid, passWord);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // set update mqtt client
  espClient.setCertificate(certificates_cert_der, certificates_cert_der_len);
  espClient.setPrivateKey(certificates_key_der, certificates_key_der_len);
  espClient.setCACert(certificates_ca_der, certificates_ca_der_len);

  Wire.begin();
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  delay(500);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    g_macAddr = getMacAddress();
    String clientId = "5bed2e156ba5bb40868b792b_D-3nPXPmg:client/device/" + g_macAddr;

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      
      Serial.println("connected");
      String  topicSub = g_hubid + ":topic/device/" + g_macAddr + "/sub";

      client.subscribe(topicSub.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      Serial.println(getMacAddress());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();

  if (now - lastMsg > 1000) {
    
      //den leb
     
      //get value
      float t = 34.456; //hard code

      if (t != lastValue || now - lastMsg > 3000) {
        //reset flag
        lastValue = t;
        lastMsg = now;
        LED_VALUE = !LED_VALUE;
        digitalWrite(LED_BUILTIN, LED_VALUE);

        String data = "";
        data = "{\"mac\":\"" + g_macAddr + "\"," +  "\"" + varTem + "\":" + String(t, DEC) + "";
        data += "}}";
        Pub(data);
      }
    
  }
}

void Pub(String data) {
  // last pub

  // topic
  String  topicPub = g_hubid + ":topic/device/" + g_macAddr + "/pub";

  Serial.print("Pub: "); Serial.println(topicPub); 
  Serial.print("data: "); Serial.println(data);
  char buff[1024] = { '\0' };
  data.toCharArray(buff, data.length());
  bool result = client.publish(topicPub.c_str(), buff, data.length());
  Serial.println(result);

}

String getMacAddress() {
  String macAddress = "";
  byte mac[6];                     // the MAC address of your Wifi shield
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
  macAddress = String(mac[0], HEX) + ":" +
               String(mac[1], HEX) + ":" +
               String(mac[2], HEX) + ":" +
               String(mac[3], HEX) + ":" +
               String(mac[4], HEX) + ":" +
               String(mac[5], HEX) ;
  return macAddress;
}




