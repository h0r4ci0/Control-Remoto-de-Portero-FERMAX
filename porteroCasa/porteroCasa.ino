/* 
* CREDITS
* Author: Horacio Manuel
* GitHub: h0r4ci0
* Mail: horamalean@proton.me
*
* I hope my code works on your machine :D
*/

#include <WiFi.h>
#include <HTTPClient.h>

#define relayVIDEO 4
#define relayDOOR 5
#define led 2
#define sleep 2500
#define wait 500

const char* ssid = "SSID";
const char* password = "password";
const char* ntfyServer = "http://[ntfy_IP]/doorBell";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(relayVIDEO, OUTPUT);
  pinMode(relayDOOR, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(relayVIDEO, 1);
  digitalWrite(relayDOOR, 1);
  connectToWiFi(); // call the funcion to connect to WiFi
  server.begin();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    // if connection lost, try connecting again
    connectToWiFi();
  }

  WiFiClient client = server.available(); // listen for incoming clients

  if (client) {
    // Resto de tu código
   Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<!DOCTYPE html><html><head><title>Control de la puerta del patio</title><style>body{font-family:Arial,sans-serif;background-color:#f2f2f2;text-align:center;margin:0;padding:0}h1{color:#333}.container{background-color:#fff;border-radius:10px;box-shadow:0 0 10px rgba(0,0,0,0.1);padding:20px;max-width:300px;margin:0 auto;margin-top:50px}button{background-color:#3498db;color:#fff;border:none;border-radius:5px;padding:10px 20px;font-size:16px;cursor:pointer;transition:background-color 0.3s}button:hover{background-color:#297fb8}</style></head><body><h1>Control de la puerta del patio</h1><div class=\'container\'><a href=\'/open\'><button id=\'openButton\'>Abrir la puerta</button></a></div></body></html>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } 
          
          else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } 
          else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check the client requests.
        if (currentLine.equals("GET /open")) {
          digitalWrite(led, 1);
          digitalWrite(relayVIDEO, 0);
          delay(sleep);
          digitalWrite(relayVIDEO, 1);
          delay(wait);
          digitalWrite(relayDOOR, 0);
          delay(wait);
          digitalWrite(relayDOOR, 1);
          delay(wait);
          digitalWrite(relayVIDEO, 0);
          delay(wait);
          digitalWrite(relayVIDEO, 1);
          digitalWrite(led, 0);
          ntfyNotify("Door open");
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(wait);
    Serial.print(".");
    digitalWrite(led, 1);
    delay(1000);
    digitalWrite(led, 0);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  ntfyNotify("Door Bell connected to WiFi");

}

// Send a notification to the server
void ntfyNotify(const char* ntfyMessage) {
  HTTPClient http;
  http.begin(ntfyServer); // starts the HTTP connection
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // header content type
  int httpResponseCode = http.POST(ntfyMessage); // send the request
  http.end();
}