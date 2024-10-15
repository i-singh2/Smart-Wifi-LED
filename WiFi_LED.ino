#include "WiFiEsp.h"  // Fixed Library
#include "SoftwareSerial.h"

// Define the pin for the LED
const int ledPin = 10;

// Wi-Fi credentials
const char* ssid = "";         // Your Wi-Fi SSID
const char* password = "";  // Your Wi-Fi password

// Initialize SoftwareSerial for communication with the WiFi shield
SoftwareSerial softSerial(4, 5); // RX, TX for Osoyoo Shield

// Initialize the WiFiEspClient
WiFiEspClient client;

void setup() {
  // Start serial communication with the computer
  Serial.begin(9600);
  
  // Initialize the SoftwareSerial for WiFi communication
  softSerial.begin(115200); 
  softSerial.write("AT+RST\r\n");  // Reset the WiFi shield
  softSerial.begin(9600); // Set the baud rate to 9600 for normal communication
  
  // Initialize the WiFi shield
  WiFi.init(&softSerial);

  // Attempt to connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address to the Serial Monitor
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Get and print the local IP address

  // Start the server on port 80
  WiFiEspServer server(80);
  server.begin();
  Serial.println("Server started");

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Ensure the LED is off initially
}

void loop() {
  // Create an instance of the server to listen for incoming connections
  WiFiEspServer server(80);
  WiFiEspClient client = server.available();

  if (client) {
    // Wait until the client sends some data
    Serial.println("New client connected");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Handle LED ON request
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED ON");
    }
    
    // Handle LED OFF request
    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }

    // Send the HTML response to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<h1>WiFi LED Control</h1>");
    client.println("<p><a href=\"/LED=ON\"><button>Turn LED ON</button></a></p>");
    client.println("<p><a href=\"/LED=OFF\"><button>Turn LED OFF</button></a></p>");
    client.println("</html>");
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
