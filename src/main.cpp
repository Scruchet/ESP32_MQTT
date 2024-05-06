#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define LED 19
#define RED 23
#define GREEN 22
#define BLUE 21
#define HIGH 255
#define LOW 0

const char* ssid = "Rang Dong Lab_2G";
const char* passwd = "ktdt123456";
const char* broker = "192.168.31.5";
const char* rgbTopic = "/topic/simon/rgb";
const char* switchTopic = "/topic/simon/switch";
const char* slideTopic = "/topic/simon/slide";
const char* buttonTopic = "/topic/simon/button";



WiFiClient espClient;
PubSubClient client(espClient);
uint8_t led_state = 0;
long currentTime, lasTime;
int count = 0;
char message[50];

void setupWifi(){
  Serial.println("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid,passwd);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print("-");
  }
  Serial.print("\nConnected to ");
  Serial.print(ssid);
}

void reconnect(){
  while(!client.connected()){
    Serial.print("\nConnecting to ");
    Serial.println(broker);
    if(client.connect("ESP_SIMON")){
      Serial.print("\nConnected to ");
      Serial.println(broker);
      client.subscribe(rgbTopic);
      client.subscribe(switchTopic);
      client.subscribe(slideTopic);
      client.subscribe(buttonTopic);
    }
    else{
      Serial.println("\nTrying to connect again");
      delay(5000);
    }
  }
}

void switch_led(char* message){
  //-------SWITCH_ON_OFF---------
  if (strcmp(message, "on") == 0){
    analogWrite(LED, HIGH);
    led_state = HIGH;
  }
  else if (strcmp(message, "off") == 0){
    analogWrite(LED, LOW);
    led_state = LOW;
  }
  // else{
  //   Serial.print("\n\rErreur switch\r\n");
  //   for (int i = 0 ; i < 5 ; i++){
  //     digitalWrite(2, LOW);
  //     delay(20);
  //     digitalWrite(2, HIGH);
  //     delay(20);
  //   }
  // }
}

void rgb_led(char* message){
  //------RGB_LED-------
  int r, g, b;
  if (sscanf(message, "rgb(%d, %d, %d)", &r, &g, &b) == 3){
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
    analogWrite(RED,r);
    analogWrite(GREEN,g);
    analogWrite(BLUE,b);
  }
  else { //erreur
    Serial.print("\n\rErreur rgb\r\n");
    for (int i = 0 ; i < 5 ; i++){
      digitalWrite(2, LOW);
      delay(20);
      digitalWrite(2, HIGH);
      delay(20);
    }
  }
}

void slide_led(char* message){
  //------RGB_LED-------
  int brightness = atoi(message);
  analogWrite(LED, brightness);
  led_state = brightness;
}

void button_led(){
  Serial.print("\n");
  Serial.print(led_state);
  Serial.print(" ---> ");
  if (led_state > LOW){
    led_state = LOW;
    analogWrite(LED, LOW);
      }
  else if (led_state == LOW){ 

    led_state = HIGH;
    analogWrite(LED,HIGH);
    
  }
  Serial.print(led_state);
}

void callback(char* topic, byte* payload, unsigned int length){

  payload[length] = 0;

  Serial.print("\n\rReceived messages; ");
  Serial.print(topic);
  Serial.print("\n\rMessage :");
  for (int i = 0 ; i < length ; i++ ){
    Serial.print((char) payload[i]);
  } 
  if (strcmp(topic, switchTopic) == 0){
    switch_led((char*)payload);
  }
  else if (strcmp(topic, rgbTopic) == 0){
    rgb_led((char*)payload);
  }
  else if (strcmp(topic, slideTopic) == 0){
    slide_led((char*)payload);
  }
  else if (strcmp(topic, buttonTopic) == 0){
    button_led();
  }
  else {
    Serial.print("\n\rErreur topic\r\n");
    for (int i = 0 ; i < 5 ; i++){
      digitalWrite(2, LOW);
      delay(20);
      digitalWrite(2, HIGH);
      delay(20);
    }
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupWifi();
  pinMode(RED,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(BLUE,OUTPUT);
  client.setServer(broker,1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}
