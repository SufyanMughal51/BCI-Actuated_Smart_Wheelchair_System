#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define DEBUGOUTPUT 0

// Define your SSIDs and passwords
const char* ssid = "Nayatel1062.4G";
const char* password = "usman106";
const char* serverName = "http://www.techiloft.com/BCI/submitReadings.php";  // Replace with your server URL

void sendData(int attention, int distance, String status) {

  // Create the payload with the data
  String payload = "reading=" + String(attention) + "&distance=" + String(distance) + "&status=" + status;

  // Send the data to the server
  WiFiClient client;
  HTTPClient http;
  Serial.print("Sending data to server...");
  if (http.begin(client, serverName)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(payload);
    if (httpCode == 200) {
      String response = http.getString();
      Serial.println("Data sent successfully");
      Serial.println("Server response: " + response);
    }else{
      Serial.println("Error on sending data");
    }
     http.end();
  }else{
    Serial.println("Error on connecting to server");
  } 
 
  // // Generate the HTML response
  // String html = "<html><body><h1>Data Sent to Database!</h1></body></html>";
  // server.send(200, "text/html", html);
}


const int activeMotors = 4; //For Active Motors
const int LeftLPWM = 13; //Left Motor 
const int LeftRPWM = 12; //Left Motor
const int RightLPWM = 14; //Right Motor 
const int RightRPWM = 12; //Right Motor

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0; 
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

int Speed = 130;  

/**
* For Ultra Sonic Sensor Setup
*/
const int trigPin = 16;
const int echoPin = 5;
// defines variables
long duration;
int distance;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup() {// put your setup code here, to run once
pinMode(activeMotors, OUTPUT); // declare as output for L298 Pin enA 
pinMode(LeftLPWM, OUTPUT); //Left Motor
pinMode(LeftRPWM, OUTPUT); //Left Motor
pinMode(RightLPWM, OUTPUT); //Right Motor 
pinMode(RightRPWM, OUTPUT); //Right Motor 
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

analogWrite(activeMotors, 255); // Motors Activated

Serial.println("yahan a raha ha");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

Serial.begin(57600);           // USB
}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte() 

{
  int ByteRead;
  while(!Serial.available());
  ByteRead = Serial.read();

#if DEBUGOUTPUT  
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/////////////
//MAIN LOOP/./
/////////////
void loop() {
  // Look for sync bytes
  if(ReadOneByte() == 170) 
  {
    if(ReadOneByte() == 170) 
    {
        payloadLength = ReadOneByte();
      
        if(payloadLength > 169)                      //Payload length can not be greater than 169
        return;
        generatedChecksum = 0;        
        for(int i = 0; i < payloadLength; i++) 
        {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
        }   

        checksum = ReadOneByte();                      //Read checksum byte from stream      
        generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

        if(checksum == generatedChecksum) 
        {    
          poorQuality = 200;
          attention = 0;
          meditation = 0;

          for(int i = 0; i < payloadLength; i++) 
          {                                          // Parse the payload
          switch (payloadData[i]) 
          {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        // *** Add your code here ***

        if(bigPacket) 
        {
          // if(poorQuality == 0)
          //    digitalWrite(LED, HIGH);
          // else
          //   digitalWrite(LED, LOW);
          
          Serial.print("PoorQuality: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Attention: ");
          Serial.print(attention, DEC);
          Serial.print(" Time since last packet: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");
          
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
          // Sets the trigPin on HIGH state for 10 micro seconds
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
          // Reads the echoPin, returns the sound wave travel time in microseconds
          duration = pulseIn(echoPin, HIGH);
          // Calculating the distance
          distance = duration * 0.034 / 2;
          // Prints the distance on the Serial Monitor
          Serial.print("Distance: ");
          Serial.println(distance);          
if(attention>40 && attention<60 && distance > 70){

  sendData(attention , distance, "Forward");
forword();         
}

if(attention>60 && attention<70){
  sendData(attention , distance, "Backword");
backword();
}

if(attention>30 && attention<40){
  sendData(attention , distance, "Left");
turnLeft();
}

if(attention>20 && attention<30){ 
  sendData(attention , distance, "Right");           
turnRight();           
}

if(attention<20 || attention>70){
  sendData(attention , distance, "Stop");
Stop();            
}              
        }
#endif        
        bigPacket = false;        
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}

`

void forword(){  //forword
  digitalWrite(LeftLPWM, LOW);
  digitalWrite(RightLPWM, LOW);
  digitalWrite(LeftRPWM, HIGH);
  digitalWrite(RightRPWM, HIGH);
  analogWrite(activeMotors, 255);
}

void backword(){ //backword
  digitalWrite(LeftLPWM, HIGH);
  digitalWrite(RightLPWM, HIGH);
  digitalWrite(LeftRPWM, LOW);
  digitalWrite(RightRPWM, LOW);
  analogWrite(activeMotors, 255);
}

void turnRight(){ //turnRight
  digitalWrite(LeftLPWM, LOW);
  digitalWrite(RightLPWM, HIGH);
  digitalWrite(LeftRPWM, LOW);
  digitalWrite(RightRPWM, HIGH);
  analogWrite(activeMotors, 255);
}

void turnLeft(){ //turnLeft
  digitalWrite(LeftLPWM, HIGH);
  digitalWrite(RightLPWM, LOW);
  digitalWrite(LeftRPWM, LOW);
  digitalWrite(RightRPWM, HIGH);
  analogWrite(activeMotors, 255);
}

void Stop(){ //stop
  digitalWrite(LeftLPWM, LOW);
  digitalWrite(RightLPWM, LOW);
  digitalWrite(LeftRPWM, LOW);
  digitalWrite(RightRPWM, LOW);
  analogWrite(activeMotors, 0);
}
