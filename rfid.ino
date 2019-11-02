#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SS_PIN D4
#define RST_PIN D2

const char* ssid ="Tetraval";
const char* password ="Dat@2019";
char hostname[] ="broker.hivemq.com";
#define TOKEN "bytesofgigabytes"

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

byte nuidPICC[4];

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
String uID;


void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println("ESP8266 AS PUBLISHER");
  client.setServer(hostname, 1883 ); //default port for mqtt is 1883

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}
 
void loop() {

if ( !client.connected() )
{
reconnect();
}

  
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
    
  if ( ! rfid.PICC_ReadCardSerial())
    return;



 for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
  printDec(rfid.uid.uidByte, rfid.uid.size);
 // MQTTPOST();
    Serial.println();
   rfid.PICC_HaltA();

 
  rfid.PCD_StopCrypto1();
}

void reconnect() 
{
while (!client.connected()) {
status = WiFi.status();
if ( status != WL_CONNECTED) {
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("Connected to AP");
}
Serial.print("Connecting to Broker …");
Serial.print("broker.hivemq.com");

if ( client.connect("ESP8266 Device", TOKEN, NULL) )
{
Serial.println("[DONE]" );
}
else {
Serial.println( " : retrying in 5 seconds]" );
delay( 5000 );
}
}
}

void printDec(byte *buffer, byte bufferSize) {
   String rfidUid = "";
  for (byte i = 0; i < bufferSize; i++) {
   // Serial.print(buffer[i], DEC);
    rfidUid += String(buffer[i], DEC);
  }
//String payload ="{"":",uID"}";

char attributes[1000];
//payload.toCharArray( attributes, 1000 );
rfidUid.toCharArray(attributes, 1000);
client.publish("tetravalsub", attributes); 
  
}

void MQTTPOST()
{
//payload formation begins here
client.publish("tetravalsub", "hello"); //topic="test" MQTT data post command.
}
