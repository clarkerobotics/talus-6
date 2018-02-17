#include <i2c_t3.h>
//#include <ams_as5048b.h>
//AMS_AS5048B amsInstance;

// Function prototypes
void receiveEvent(size_t count);
void requestEvent(void);

int ledPin = 13;
int address = 0x60;
String addrs = "0x60";

#define MEM_LEN 50
int counter = 0;
volatile uint8_t received;
const byte numChars = MEM_LEN;
char receivedData[numChars];
char* strArgs[6];

boolean newData = false;


//
// Setup
void setup() {
   pinMode(LED_BUILTIN,OUTPUT); // LED

   // Setup for Slave mode, address 0x66, pins 18/19, external pullups, 400kHz
//    Wire.begin(I2C_SLAVE, 0x06, I2C_PINS_16_17);
 
//     Wire.begin(I2C_SLAVE, 0x06, I2C_PINS_18_19);
//     Wire1.begin(I2C_SLAVE, 0x06, I2C_PINS_22_23);
//    amsInstance.begin();
   Wire1.begin(address);

   // Data init
   received = 0;
   memset(receivedData, 0, sizeof(receivedData));

   // register events
   Wire1.onReceive(receiveEvent);
   Wire1.onRequest(requestEvent);

   Serial.begin(9600);
}

void loop() {
  recvWithStartEndMarkers();
  assignNewData();
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedData[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedData[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


void recvWireDataWithMarkers(size_t count) {
  boolean recvInProgress = false;
  int ndx = 0;
  char wireData[MEM_LEN];
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // copy Rx data to receivedData
  Wire1.read(wireData, count);

  // Loop through and only find data that we want
  for (size_t ra = 0; ra < count; ra++) {
    rc = wireData[ra];
    if (rc == endMarker) break;
    if (recvInProgress == true && rc != endMarker) {
      receivedData[ndx] = rc;
      ndx++;
    }
    if (rc == startMarker) recvInProgress = true;
  }
  newData = true;
  ndx = 0;

  // Let's do this
  if (newData) assignNewData();
}

void assignNewData() {
  if (newData == true) {
    Serial.print("Received: ");
    Serial.println(receivedData);
    newData = false;

    // Splitting strings... or splitting hairs?!
    char *token = strtok(receivedData, ",");
    if (token) {
       // You've got to COPY the data pointed to
       strArgs[counter++] = strdup(token);
       // Keep parsing the same string
       token = strtok(NULL, ",");

       while(token) {
          // You've got to COPY the data pointed to
          strArgs[counter++] = strdup(token);
          token = strtok(NULL, ",");
       }

       String addy = String(strArgs[0]);
       Serial.print("addy: ");
       Serial.println(addy);
       if (addy == addrs) {
         // TODO: Execute function!
         Serial.print("ADD: ");
         Serial.print(strArgs[0]);
         Serial.print(", CMD: ");
         Serial.print(strArgs[1]);
         Serial.print(", ANGL: ");
         Serial.print(strArgs[2]);
         Serial.print(", DUR: ");
         Serial.print(strArgs[3]);
         Serial.print(", CB: ");
         Serial.print(strArgs[4]);
         Serial.print(", TS: ");
         Serial.println(strArgs[5]);
         // for fun, set duration of LED to the duration of cmd
         digitalWrite(ledPin, HIGH);
         delay(atoi(strArgs[3]));
         digitalWrite(ledPin, LOW);
       }
    }

    // Reset for future use
    memset(receivedData, 0, sizeof(receivedData));
    counter = 0;
  }
}

//
// handle Rx Event (incoming I2C data)
void receiveEvent(size_t count) {
  if (count < 1) return;
  memset(receivedData, 0, sizeof(receivedData));
  recvWireDataWithMarkers(count);
}

//
// handle Tx Event (outgoing I2C data)
void requestEvent(void) {
  // fill Tx buffer (send full mem)
  Wire1.write(receivedData, MEM_LEN);
}
