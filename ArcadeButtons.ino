#include <PinChangeInt.h>
#include <UIPEthernet.h>
// #include <HttpClient.h>

// Arduino NANO
//
//  D12 D11 D10 D09 D08 D07 D06 D05 D04 D03 D02 GND RST RX0 TX1
//
//  D13 3V3 ARF A00 A01 A02 A03 A04 A05 A06 A07 +5V RST GND VIN

// PWM enabled:   D3 D5 D6 D9 D10 D11
// Ethernet uses: D10-D13


// 
// Button/Led: B2 L2 L1 B1 L0 B0
// Arduino:    D7 D6 D5 D4 D3 D2

// Definitions

// Button and LED mapping
int nbuttons = 3;
int ledPin[] = {PD3, PD5, PD6};    
int buttonPin[] = {PD2, PD4, PD7};  

// Message interface
// replace with some moderately real URL
// char url[] = "http://localhost:9000?kitcode=944A0CE6F141&eventCode=";
// "http://lighting.dev.smartgaiacloud.com/diagCode/demo2_api.php?kitcode=944A0CE6F141&eventCode="
char server[] = "192.168.6.2";
char path[] = "/test";
int eventCodeOn[] = {900101, 900102, 900103};
int eventCodeOff[] = {0,0,0};

// State
int state[] = {0, 0, 0};
int level[] = {0, 0, 0};


// MAC address
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0x01, 0xFE, 0x01 };

EthernetClient client;
IPAddress ip(192,168,6,10);


void setup() {
  Serial.begin(9600);
  Serial.println("I'm awake!");
  pinMode(13, OUTPUT);
  status(1);
  Serial.print("Configuring pins ...");
  configure_input_pins();
  Serial.println(" done");
  Serial.println("Configuring Ethernet ...");
  if ( Ethernet.begin(mac) == 0 ) {
    Serial.println("DHCP failed - static address.");
    Ethernet.begin(mac, ip);
  }
  Serial.print("Ethernet configured: ");
  Serial.println(Ethernet.localIP());
  status(0);
  on(0);
  on(1);
  off(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  manageLights();
  delay(10);
}

void status(int on) {
  digitalWrite(13,on);
}

void manageLights() {
  for ( int i = 0; i<nbuttons; i++) {
    if ( (not state[i]) and level[i] > 0 ) {
      analogWrite(ledPin[i], --level[i]);
    }
  }
}

void configure_input_pins() {
  for (int button = 0 ; button < nbuttons ; button++) {
    pinMode(buttonPin[button], INPUT_PULLUP);
    // attachPinChangeInterrupt(buttonPin[button], handle_press, RISING);
  }
}


volatile uint8_t latest_interrupted_pin;
void handle_press() {
  status(1);
  for (int i=0; i<nbuttons; i++) {
    latest_interrupted_pin=PCintPort::arduinoPin;
    if ( buttonPin[i] == latest_interrupted_pin ) {
      toggle_lights(i);
      call_api(i);
    }
  }
  status(0);
}

void toggle_lights(int pin) {
  if ( state[pin] ) {
    off(pin);
  } 
  else {
    on(pin);
  }
}

void call_api(int pin) {
  char buffer[256];
  if ( int event = state[pin] ? eventCodeOff[pin] : eventCodeOn[pin] ) {
    if ( client.connect(server,80) ) {
      client.println("GET /test");
      client.print("Host: ");
      client.println(server);
      client.println("Connection: close");
      client.println();
    }
  }
}

void on(int led) {
  if (led<0 or led>nbuttons) { return; };
  state[led] = 1;
  level[led] = 255;
  analogWrite(ledPin[led],255);
}

void off(int led) {
  if (led<0 or led>nbuttons) { return; };
  state[led] = 0;
}


