#include <SPI.h>
#include <WiFi.h>
#include <dht11.h>

#define aref_voltage 3.3
dht11 DHT11;
char ssid[] = "SSID"; //  your network SSID (name) 
char pass[] = "PWD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 1;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:

//IPAddress server(173,194,73,105);  // numeric IP for Google (no DNS)
char server[] = "www.nirmankarta.com";    // or URL of the server

String ID_val = "UNIQUE_ID";
String ID_col = "id:";
int  t_val;
String t_col = "t:";
int  h_val;
String h_col = "h:";
int d_val;
String d_col = "d:";
String data ;
// Initialize the WiFi client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  
  DHT11.attach(2);
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connect to WEP network:    
    status = WiFi.begin(ssid, keyIndex, pass);
    // Connect to WPA/WPA2 network
    //status = WiFi.begin(ssid, keyIndex, pass);
    
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
  send_data();
}

void loop() {

  // if there are incoming bytes available 
  // from the server, read them and print them:
  delay(45000);
  send_data();
  
  // if the server's disconnected, stop the client:
  /*if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    while(true);
  }*/
  
}

void send_data()
{
  double t =DHT11.celcius();
  double h =(float)DHT11.humidity+20;
  double d =DHT11.dewPoint();
  delay(5000);
  Serial.println();
  Serial.println(t);
  Serial.println(h);
  Serial.println(d);
  
  Serial.println("\nStarting connection to server...");
  Serial.println();
  h_val = h*100;
  t_val= t*100;
  d_val = d*100;
  data = "data={json:{";
  data = data + ID_col + ID_val + ", ";
  data = data + t_col + t_val + ", ";
  data = data + h_col + h_val + ", ";
  data = data + d_col + d_val + "}}";
  
  Serial.println(data);
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
  Serial.println();
    client.println("POST /api/909/json.php HTTP/1.1");
    client.println("Host:www.nirmankarta.com");
    client.println("User-Agent: Arduino/1.3");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
  }
  else
  {
    Serial.println("Died Out");
  Serial.println();
  }
  delay(10000);
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
