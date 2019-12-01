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
 #include <WiFi.h>
#include <ESPmDNS.h>

#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include <WiFiClient.h>
WebServer server(80);
HTTPUpdateServer httpUpdate;
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

void web_setup(const char *update_password) {
//  SPIFFS.begin();                           // Start the SPI Flash Files System

  server.on("/metrics", handle_metrics);

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
