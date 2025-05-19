#include <WiFi.h>
#include <ESPmDNS.h>

// WiFi credentials for network connection
const char* ssid = "NETGEAR15";
const char* password = "helpfulunicorn456";

// Pin declarations for actuator and vibration
const int forwardPin = 4;    // Pin to extend the actuator
const int backwardPin = 5;   // Pin to retract the actuator
const int vibrationPin = 19; // Pin to control vibration (e.g., motor)

WiFiServer server(80);       // Web server on port 80

// Global state variables
bool isConnected = false;            // Tracks WiFi connection status
char lastCommand = '\0';             // Stores the last received command ('e', 'r', 's', 'c', 'v')
String serialLog = "";               // Logs messages for the Mini Terminal
int minutes = 0;                     // Duration of massage mode from UI (max 2)
unsigned long modeStartTime = 0;     // Timestamp when massage mode starts
bool modeRunning = false;            // Flag indicating if massage mode is active
bool vibrationOn = false;            // Tracks vibration state (on/off)

void logMessage(String msg) {
  Serial.println(msg);
  serialLog += msg + "<br>";         // Add line break for HTML formatting
  if (serialLog.length() > 1000) {   // Trim log to last 1000 chars to prevent memory overflow
    serialLog = serialLog.substring(serialLog.length() - 1000);
  }
}

// Actuator and vibration control functions
void extendActuator() {
  digitalWrite(forwardPin, HIGH);    // Activate forward motion
  digitalWrite(backwardPin, LOW);
  logMessage("Forward");
}

void retractActuator() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);   // Activate backward motion
  logMessage("Backward");
}

void stopActuator() {
  digitalWrite(forwardPin, LOW);     // Stop all motion
  digitalWrite(backwardPin, LOW);
  logMessage("Stop");
}

void toggleVibration() {
  vibrationOn = !vibrationOn;        // Toggle vibration state
  digitalWrite(vibrationPin, vibrationOn ? HIGH : LOW); // Set pin state
  logMessage("Vibration " + String(vibrationOn ? "ON" : "OFF"));
}

void sendHTML(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // HTML structure begins
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en\">");
  client.println("<head>");
  client.println("<meta charset=\"utf-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  
  // JavaScript for UI interaction and real-time terminal updates
  client.println("<script>");
  client.println("function setMode(mode) {");           // Triggers massage mode (still reloads page)
  client.println("  var minutes = document.getElementById('minutes').value;");
  client.println("  window.location.href = '/' + mode + '?minutes=' + minutes;");
  client.println("}");
  client.println("function extend() {");                // Sends extend command without reload
  client.println("  fetch('/extend');");
  client.println("}");
  client.println("function retract() {");               // Sends retract command without reload
  client.println("  fetch('/retract');");
  client.println("}");
  client.println("function stop() {");                  // Sends stop command without reload
  client.println("  fetch('/stop');");
  client.println("}");
  client.println("function toggleVibration() {");       // Toggles vibration without reload
  client.println("  fetch('/vibration');");
  client.println("}");
  client.println("function updateTerminal() {");        // Fetches log updates
  client.println("  fetch('/log')");
  client.println("    .then(response => response.text())");
  client.println("    .then(data => {");
  client.println("      document.getElementById('terminal').innerHTML = data;");
  client.println("      var terminal = document.getElementById('terminal');");
  client.println("      terminal.scrollTop = terminal.scrollHeight;");
  client.println("    });");
  client.println("}");
  client.println("setInterval(updateTerminal, 1000);"); // Updates terminal every 1 second
  client.println("window.onload = updateTerminal;");    // Initial terminal update on page load
  client.println("</script>");
  
  // CSS for styling the interface
  client.println("<style>");
  client.println("body { font-family: Arial; text-align: center; }");
  client.println(".button { padding: 10px 20px; margin: 5px; text-decoration: none; color: white; display: inline-block; }");
  client.println(".btn-primary { background-color: #007bff; }"); // Blue for control buttons
  client.println(".btn-danger { background-color: #dc3545; }");  // Red for stop button
  client.println(".btn-success { background-color: #28a745; }"); // Green for massage mode
  client.println(".btn-purple { background-color: #6f42c1; }");  // Purple for vibration toggle
  client.println(".container { margin: 20px auto; max-width: 800px; }");
  client.println(".terminal { background-color: #000; color: #0f0; padding: 10px; height: 150px; overflow-y: scroll; text-align: left; font-family: monospace; border: 1px solid #ccc; width: 80%; margin: 20px auto; }");
  client.println("</style>");
  
  client.println("<title>Scalp Masseuse</title>");
  client.println("</head>");
  
  client.println("<body>");
  
  // Header section
  client.println("<div style=\"background-color: #343a40; padding: 20px;\">");
  client.println("<h1 style=\"color: white;\">Scalp Masseuse</h1>");
  client.println("</div>");
  
  client.println("<div class=\"container\">");
  
  // WiFi status display
  client.println("<h3>Wi-Fi Status</h3>");
  if (isConnected) {
    client.println("<p>Status: <span style=\"background-color: #28a745; color: white; padding: 5px;\">Connected</span>");
    String ipString = WiFi.localIP().toString();
    client.print(" | IP: ");
    client.print(ipString);
    client.println("</p>");
  } else {
    client.println("<p>Status: <span style=\"background-color: #dc3545; color: white; padding: 5px;\">Disconnected</span></p>");
  }
  
  // Manual control buttons (now without page reload)
  client.println("<div style=\"margin: 20px;\">");
  client.println("<h3>Massage Controls:</h3>");
  client.println("<button onclick=\"extend()\" class=\"button btn-primary\">Extend</button>");     // Extend button
  client.println("<button onclick=\"retract()\" class=\"button btn-primary\">Retract</button>");   // Retract button
  client.println("<button onclick=\"stop()\" class=\"button btn-danger\">Stop</button>");         // Stop button
  client.println("</div>");
  
  // Massage mode and vibration controls
  client.println("<div style=\"margin: 20px;\">");
  client.println("<h3>Massage Mode:</h3>");
  client.println("<p>Type minutes (max 2) and press continuous massage:</p>");
  client.println("<input type=\"number\" id=\"minutes\" value=\"0\" min=\"0\" max=\"2\" style=\"width: 100px; margin: 10px;\">");
  client.println("<br>");
  client.println("<a href=\"javascript:setMode('continuous')\" class=\"button btn-success\">Continuous Massage</a>");
  client.println("<br><br>");
  client.println("<button onclick=\"toggleVibration()\" class=\"button btn-purple\">Toggle Vibration</button>");
  client.println("</div>");
  
  // Mini Terminal for log display
  client.println("<h3>Mini Terminal</h3>");
  client.println("<div class=\"terminal\" id=\"terminal\">");
  client.print(serialLog); // Initial log content
  client.println("</div>");
  
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");
  
  client.println();
}

// Initial setup: configure pins and connect to WiFi
void setup() {
  pinMode(forwardPin, OUTPUT);    // Set forward pin as output
  pinMode(backwardPin, OUTPUT);   // Set backward pin as output
  pinMode(vibrationPin, OUTPUT);  // Set vibration pin as output
  digitalWrite(vibrationPin, LOW); // Start with vibration off
  
  Serial.begin(115200);           // Start serial communication for debugging
  delay(1000);                    // Brief delay to stabilize

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);     // Attempt WiFi connection

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Retry up to 20 times
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) { // Successful connection
    isConnected = true;
    logMessage("\nWiFi connected");
    logMessage(WiFi.localIP().toString());
    server.begin();                    // Start the web server
    logMessage("Server started");
    if (MDNS.begin("scalpmasseuse")) { // Start mDNS for local access
      logMessage("mDNS started: http://scalpmasseuse.local");
    } else {
      logMessage("Error setting up mDNS");
    }
  } else { // Connection failed
    isConnected = false;
    logMessage("\nFailed to connect to WiFi");
  }
}

// Main loop: handle WiFi, client requests, and massage mode
void loop() {
  isConnected = (WiFi.status() == WL_CONNECTED); // Check WiFi status

  if (!isConnected) { // Attempt reconnection if disconnected
    logMessage("WiFi disconnected, attempting to reconnect...");
    WiFi.reconnect();
    delay(2000);
    return;
  }

  // Handle incoming client requests
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";       // Buffer for HTTP request line
    bool commandProcessed = false; // Flag for processed commands
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) { // Empty line signals end of request
            sendHTML(client);
            break;
          } 
          else if (currentLine.startsWith("GET /extend")) { // Manual extend command
            lastCommand = 'e';
            commandProcessed = true;
          }
          else if (currentLine.startsWith("GET /retract")) { // Manual retract command
            lastCommand = 'r';
            commandProcessed = true;
          }
          else if (currentLine.startsWith("GET /stop")) { // Stop command
            lastCommand = 's';
            commandProcessed = true;
          }
          else if (currentLine.startsWith("GET /continuous")) { // Start continuous mode
            lastCommand = 'c';
            commandProcessed = true;
            int minutesIndex = currentLine.indexOf("minutes=");
            if (minutesIndex != -1) {
              minutes = currentLine.substring(minutesIndex + 8).toInt();
              if (minutes > 2) minutes = 2; // Cap at 2 minutes
            }
          }
          else if (currentLine.startsWith("GET /vibration")) { // Toggle vibration command
            lastCommand = 'v';
            commandProcessed = true;
          }
          else if (currentLine.startsWith("GET /log")) { // Serve log for terminal updates
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(serialLog);
            client.println();
            break;
          }
          currentLine = ""; // Reset buffer for next line
        } else if (c != '\r') {
          currentLine += c; // Build request line
        }
      }
    }
    if (commandProcessed) { // Execute the received command
      switch (lastCommand) {
        case 'r': // Retract actuator for 500ms
          retractActuator();
          delay(500);
          stopActuator();
          delay(500);
          logMessage("Client disconnected");
          break;
        case 'e': // Extend actuator for 500ms
          extendActuator();
          delay(500);
          stopActuator();
          delay(500);
          logMessage("Client disconnected");
          break;
        case 's': // Stop actuator and any running mode
          stopActuator();
          delay(500);
          modeRunning = false;
          logMessage("Client disconnected");
          break;
        case 'c': // Start continuous massage mode
          if (minutes > 0) {
            modeStartTime = millis();
            modeRunning = true;
            logMessage("Continuous Massage mode started for " + String(minutes) + " minutes");
          }
          logMessage("Client disconnected");
          break;
        case 'v': // Toggle vibration state
          toggleVibration();
          logMessage("Client disconnected");
          break;
      }
      lastCommand = '\0'; // Clear command after processing
    }
    client.stop(); // Close client connection
  }

  // Handle continuous massage mode
  if (modeRunning && minutes > 0) {
    unsigned long elapsedTime = (millis() - modeStartTime) / 1000; // Time elapsed in seconds
    int totalSeconds = minutes * 60;                              // Total duration in seconds
    int remainingSeconds = totalSeconds - elapsedTime;            // Remaining time

    if (remainingSeconds <= 0) { // Mode completed
      stopActuator();
      logMessage("Continuous Massage mode completed");
      modeRunning = false;
      minutes = 0;
    } else {
      static unsigned long lastLogTime = 0;
      if (millis() - lastLogTime >= 5000) { // Log status every 5 seconds
        logMessage("Continuous Massage mode: " + String(remainingSeconds / 60) + "m " + String(remainingSeconds % 60) + "s remaining");
        lastLogTime = millis();
      }

      // Cycle actuator every 2 seconds
      static bool extending = true;         // Direction flag
      static unsigned long lastCycleTime = 0;
      int cycleDelay = 2000;                // 2-second cycle time

      if (millis() - lastCycleTime >= cycleDelay) {
        if (extending) {
          extendActuator();
        } else {
          retractActuator();
        }
        extending = !extending;            // Toggle direction
        lastCycleTime = millis();          // Update cycle timestamp
      }
    }
  }
}
