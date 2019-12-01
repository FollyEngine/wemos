#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClient.h>
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate;

// curl -u "admin:<password>" -F "image=@firmware.bin" esp8266-84f3eb3b7c66.local/folly
//#include <FS.h>   // Include the SPIFFS library

#else
 //esp32 only
// #include <touch.h>
// #include <WebServer.h>
// WebServer server(80);
#endif
#include <AutoConnect.h>
AutoConnect portal(server);
AutoConnectAux update("/update", "Update");
AutoConnectAux hello;                       // Step #8


static const char HELLO_PAGE[] PROGMEM = R"(
{ "title": "Hello world", "uri": "/", "menu": true, "element": [
    { "name": "caption", "type": "ACText", "value": "<h2>Hello, world</h2>",  "style": "text-align:center;color:#2f4f4f;padding:10px;" },
    { "name": "content", "type": "ACText", "value": "In this page, place the custom web page handled by the sketch application." } ]
}
)";

const char* update_path = "/folly";
const char* update_username = "admin";

String SendMetrics() {
  return "# HELP http_requests_total The total number of HTTP requests.\n"
  "# TYPE http_requests_total counter\n"
  "http_requests_total{method=\"post\",code=\"200\"} 1027 1395066363000\n"
  "http_requests_total{method=\"post\",code=\"400\"}    3 1395066363000\n";

}

void handle_metrics() {
  Serial.println("OnConnect /metrics");
  server.send(200, "text/plain; version=0.0.4", SendMetrics()); 
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}


void web_setup(const char *update_password) {
//  SPIFFS.begin();                           // Start the SPI Flash Files System

  server.on("/metrics", handle_metrics);

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  httpUpdate.setup(&server, "folly", update_password);
  hello.load(HELLO_PAGE);                                // Step #9.b
  portal.join({hello, update});
  if (portal.begin()) {                                  // Step #9.d
    MDNS.addService("http", "tcp", 80);              // Step #9.e
  }
  Serial.println("HTTP server started");

}

void web_loop() {
    portal.handleClient();
}
