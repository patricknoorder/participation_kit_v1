#include <SPI.h>
#include <Ethernet.h>
#include <math.h>

#define SOUND_SENSOR A0
#define LIGHT_SENSOR A1
#define TEMP_SENSOR A2

// Enter a MAC address for your controller below. MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xC0, 0x93 };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "data.participationkit.com"; 
IPAddress ip(192,168,0,177);
EthernetClient client;
const int thresholdvalue=10;         //The treshold for which the LED should turn on.
float lightValue;                       //Resistance of sensor in K
int a;
float temperature;
int B=3975;                          //B value of the thermistor
float resistance;
int roundedTemp[35]; 
int roundedSound[35]; 
int roundedLight[35];
int soundCounter = 0;
int lightCounter = 0;
int tempCounter = 0;
int counterSending = 0;
int tempDataValue;
int soundDataValue;
int lightDataValue;


void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    pins_init();

    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      // try to congifure using IP address instead of DHCP:
      Ethernet.begin(mac, ip);
    }
  
    // give the Ethernet shield a second to initialize:
    delay(1000);
    Serial.println("connecting...");
}

void loop() {
  if(counterSending == 300){
    sendData();
    counterSending = 0;
  }
  else{
    Serial.println(counterSending);
    counterSending++;
    
    delay(1000);
  }
}

void sendData(){
  Serial.println("Create connection");
   
  String data;
  data+="";
  data+="temp=";
  data+= getTempData();
  data+="&light=";
  data+= getLightData();
  data+="&sound=";
  data+= getSoundData();
  data+="&device=2";
  data+="&submit=Submit"; // Submitting data

  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.println("POST /save_data.php HTTP/1.1");
    client.println("Host: data.participationkit.com");
    client.println("Content-Type: application/x-www-form-urlencoded"); 
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    client.println();
    
    //Prints your post request out for debugging
    Serial.println("POST /save_data.php HTTP/1.1");
    Serial.println("Host: data.participationkit.com");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("User-Agent: Arduino/1.0");
    Serial.println("Connection: close");
    Serial.print("Content-Length: ");
    Serial.println(data.length());
    Serial.println();
    Serial.print(data);
    Serial.println();
    
    client.stop();
  }
  else {
    Serial.println("connection failed");
  }
}

int getSoundData(){
   int soundValue = analogRead(SOUND_SENSOR); 
   
   return soundValue;
}

float getTempData(){
   a=analogRead(0);
   resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
   temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
   
   return temperature;
}

float getLightData(){
  int sensorValue = analogRead(LIGHT_SENSOR); 
  lightValue = (float)(1023-sensorValue)*10/sensorValue;
  
  return lightValue;
}

void pins_init() {
    pinMode(SOUND_SENSOR, INPUT);
    pinMode(LIGHT_SENSOR, INPUT);
    pinMode(TEMP_SENSOR, INPUT);
}    
