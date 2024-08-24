#define DEBUG       1
#define BUFFER_SIZE 250
#define SWITCH      5
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Variables for test data
// int int_value;
// float float_value;
// bool bool_value = true;

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xB4, 0x8A, 0x0A, 0x8F, 0x2D, 0x48};

byte index2 = 0;
uint8_t colors [][3] {
  {255,0,0}, // RED
  {0,255,0}, // GREEN
  {0,0,255}, // BLUE
  {255,255,0}, // YELLOW
  {0,255,255}, // CYAN
  {255,0,255}, // MAGENTA
  {255,255,255}, // WHITE
  {255,127,0}, // ORANGE
  {127,0,255}, // PURPLE
  {0,127,255}, // LIGHT BLUE
  {127,255,0}, // LIME
  {255,0,127}, // PINK
  {0,255,127}, // TEAL
  {127,255,255}, // LIGHT CYAN
  {255,127,127}, // LIGHT PINK
  {127,127,255}, // LIGHT PURPLE
  {127,255,127}, // LIGHT GREEN
  {255,255,127}, // LIGHT YELLOW
  {255,127,255}, // LIGHT MAGENTA
  {127,63,0}, // BROWN
  {63,127,0}, // OLIVE
  {0,127,63}, // SEA GREEN
  {0,63,127}, // SKY BLUE
  {63,0,127}, // INDIGO
  {127,0,63}, // MAROON
  {127,63,63}, // ROSE
  {63,127,63}, // MINT
  {63,63,127}, // LAVENDER
  {63,127,127}, // AQUA
  {127,63,127}, // VIOLET
  {127,127,63}, // LIME GREEN
  {127,127,127}, // SILVER
  {127,63,0}, // BROWN
  {63,127,0}, // OLIVE
  {0,127,63}, // SEA GREEN
  {0,63,127}, // SKY BLUE
  {63,0,127}, // INDIGO
  {127,0,63}, // MAROON
  {127,63,63}
};
byte data_buffer [BUFFER_SIZE] = {
};
// // Define a data structure
typedef struct struct_message {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} struct_message;

// // Create a structured object
struct_message myData;

// Peer info
esp_now_peer_info_t peerInfo;
int fail_count = 0;
// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  status == ESP_NOW_SEND_SUCCESS ? fail_count = fail_count : fail_count++;
  if (DEBUG){Serial.print("Fail count: "); Serial.println(fail_count);}
}
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
void switchRead(){
  if (millis() - lastDebounceTime < 250) { return; }

  lastDebounceTime = millis();
  if (digitalRead(SWITCH) == LOW) {
    myData.red = colors[index2][0];
    myData.green = colors[index2][1];
    myData.blue = colors[index2][2];
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    index2 = random(0,sizeof(colors)/3);
  }

}

void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);
  pinMode(SWITCH, INPUT_PULLUP);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

int index_ = 0;

void process_data() {
  // Fill data_buffer based on the current index
  for (int i = 0; i < BUFFER_SIZE; i++) {
    data_buffer[i] = (i == index_) ? 1 : 0;
  }
  index_++;
  if (index_ >= BUFFER_SIZE) {
    index_ = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
      data_buffer[i] = 0;
    }
  }

  // Send data_buffer via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, data_buffer, sizeof(data_buffer));
  if (result == ESP_OK) {
    if (DEBUG){ Serial.println("Sent with success");}
  } else {
    if (DEBUG){ Serial.println("Error sending the data");}
  }
}


void loop(){
  // process_data();
  switchRead();
  delay(50);
}


// void loop() {
//   // Generate a random integer
//   int_value = random(1,20);

//   // Use integer to make a new float
//   float_value = 1.3 * int_value;

//   // Invert the boolean value
//   bool_value = !bool_value;
  
//   // Format structured data
//   strcpy(myData.a, "INTERACTIVE");
//   myData.b = int_value;
//   myData.c = float_value;
//   myData.d = bool_value;
  
//   // Send message via ESP-NOW
//   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
//   if (result == ESP_OK) {
//     Serial.println("Sending confirmed");
//   }
//   else {
//     Serial.println("Sending error");
//   }
//   delay(1000);
// }