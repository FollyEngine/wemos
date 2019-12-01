#ifdef ESP8266
#include <servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>


//#include <WiFiClient.h>
//#include <FS.h>   // Include the SPIFFS library



ESP8266WebServer server(80);

// curl -u "admin:<password>" -F "image=@firmware.bin" esp8266-84f3eb3b7c66.local/folly
ESP8266HTTPUpdateServer httpUpdater;

#else
 //esp32 only
// #include <touch.h>
// #include <WebServer.h>
// WebServer server(80);
#endif

const char* update_path = "/folly";
const char* update_username = "admin";

// SPIFF from https://tttapa.github.io/ESP8266/Chap12%20-%20Uploading%20to%20Server.html
//File fsUploadFile;              // a File object to temporarily store the received file

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

String SendHTML() {
  return "Hostname: \n";
}


void handle_root() {
  Serial.println("OnConnect /");
  server.send(200, "text/html", SendHTML()); 
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

// void handleFileUpload(){ // upload a new file to the SPIFFS
//   HTTPUpload& upload = server.upload();
//   if(upload.status == UPLOAD_FILE_START){
//     String filename = upload.filename;
//     if(!filename.startsWith("/")) filename = "/"+filename;
//     Serial.print("handleFileUpload Name: "); Serial.println(filename);
//     fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
//     filename = String();
//   } else if(upload.status == UPLOAD_FILE_WRITE){
//     if(fsUploadFile)
//       fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
//   } else if(upload.status == UPLOAD_FILE_END){
//     if(fsUploadFile) {                                    // If the file was successfully created
//       fsUploadFile.close();                               // Close the file again
//       Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
//       server.sendHeader("Location","/success.html");      // Redirect the client to the success page
//       server.send(303);
//     } else {
//       server.send(500, "text/plain", "500: couldn't create file");
//     }
//   }
// }

void web_setup(const char *update_password) {
//  SPIFFS.begin();                           // Start the SPI Flash Files System

  server.on("/", handle_root);
  server.on("/metrics", handle_metrics);
//   server.on("/upload", HTTP_GET, []() {                 // if the client requests the upload page
//     if (!handleFileRead("/upload.html"))                // send it if it exists
//       server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
//   });

//   server.on("/upload", HTTP_POST,                       // if the client posts to the upload page
//     [](){ server.send(200); },                          // Send status 200 (OK) to tell the client we are ready to receive
//     handleFileUpload                                    // Receive and save the file
//   );

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  httpUpdater.setup(&server, update_path, update_username, update_password);
  server.begin();                           // Actually start the server

  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTP server started");

}

void web_loop() {
    server.handleClient();
}
