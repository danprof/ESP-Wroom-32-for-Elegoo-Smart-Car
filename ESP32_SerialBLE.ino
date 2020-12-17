// ESP32 BLE to USB
// on Elegoo Smart Car
// BLE module that comes with car uses 9600 baud
// and connects to RX & TX on Arduino.

char discard ;
//----------------------------------------------------
void setup() {
  Serial.begin(115200);       // Arduino Serial (pins 0 & 1) to ESP32 (pins 34 & 35)
  Serial2.begin(9600);        // BLE module to ESP32 (pins 16 & 17)
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
   
  Serial.println("(This is Serial 0)");
  Serial2.write("(This is Serial 2)");

}
//------------------------------------------------------
void loop() {
  int x=0;

  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial2.write("From Serial 0 - ");
    Serial2.write(Serial.read());
    x = Serial.available();
    if(x == 2){
       discard = Serial.read();       // get rid of CR & LF
       discard = Serial.read();
    }
  }
  if (Serial2.available()) {     // If anything comes in Serial2 (pins 16 & 17)
    Serial.print("From Serial 2 - ");
    Serial.println(Serial2.read());
    x = Serial2.available();
    if(x == 2){
       discard = Serial2.read();       // get rid of CR & LF
       discard = Serial2.read();
    }
  }
}
