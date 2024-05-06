#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//----------------------------------
// DEFINE LED PINS
#define DEBUG 2
#define LED 19
#define RED 23
#define GREEN 22
#define BLUE 21   
//----------------------------------

#define high 255 //analog HIGH state
#define low 0    //analog LOW state


//----------------------------------
//IMPORTANT GLOBAL VARIABLES
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
      //-----------------------------
      //Connecting to each topic
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

void switch_led(char* message){ //Change led state using a switch
  //-------SWITCH_ON_OFF---------
  if (strcmp(message, "on") == 0){
    analogWrite(LED, high);
    led_state = high;
  }
  else if (strcmp(message, "off") == 0){
    analogWrite(LED, low);
    led_state = low;
  }

}

void rgb_led(char* message){ //Control RGB LED
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
  else { //error
    Serial.print("\n\r RGB ERROR r\n");
    for (int i = 0 ; i < 5 ; i++){
      digitalWrite(DEBUG, low);
      delay(20);
      digitalWrite(DEBUG, high);
      delay(20);
    }
  }
}

void slide_led(char* message){ //Led PWM command
  int brightness = atoi(message);
  analogWrite(LED, brightness);
  led_state = brightness;
}

void button_led(){ //Press button and change led state
  if (led_state > low){
    led_state = low;
    analogWrite(LED, low);
      }
  else if (led_state == low){ 

    led_state = high;
    analogWrite(LED,high);
  }
}

void choose_topic(char* topic , char* message){
  if (strcmp(topic, switchTopic) == 0){
    switch_led(message);
  }
  else if (strcmp(topic, rgbTopic) == 0){
    rgb_led(message);
  }
  else if (strcmp(topic, slideTopic) == 0){
    slide_led(message);
  }
  else if (strcmp(topic, buttonTopic) == 0){
    button_led();
  }
}

void callback(char* topic, byte* payload, unsigned int length){
  payload[length] = 0;
  Serial.print("\n\rReceived messages; ");
  Serial.print(topic);
  Serial.print("\n\rMessage :");
  for (int i = 0 ; i < length ; i++ ){
    Serial.print((char) payload[i]);
  } 
  choose_topic(topic, (char*)payload);
}

void setup_led(){
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(DEBUG,OUTPUT);
  analogWrite(RED, low);
  analogWrite(GREEN, low);
  analogWrite(BLUE, low);
  analogWrite(LED, low);
  digitalWrite(DEBUG,LOW);
}

void setup() {
  Serial.begin(115200);
  setupWifi();
  setup_led();
  client.setServer(broker,1883); //MQTT PORT
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
}

