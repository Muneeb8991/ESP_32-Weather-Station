//Including Libraries 
#include <WiFi.h>  
#include <HTTPClient.h> 
#include<WebServer.h> 
#include <DHT.h> 
 
//Specifying Network SSID and password const char * ssid = "SSID"; const char * password = "Password"; 
 
//Enter Google script ID 
String script_ID = "AKfycbxPZW7eKR-
SxigzL6IJFW4yh8l1bEtu_0rNYHzL7JKEUcs06pCt9jUI5VNd8tF9b0tO"; 
 
//Initialise Web server 
WebServer server(80); 
 
//Define DHT sensor type, GPIO pin and initialize Object 
#define DHTPIN 18 
#define DHTTYPE DHT22 
DHT dht(DHTPIN,DHTTYPE); 
 
void setup() { 
  //Initialise serial Communication 
  Serial.begin(115200); 
 
  //Connect to WiFi 
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password); 
 
  Serial.print("Connecting to Wi-Fi"); 
  while (WiFi.status() != WL_CONNECTED) {     delay(500);     Serial.print("."); 
  } 
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP()); 
 
    
 
 
 
//Initalise DHT22 sensor 
 dht.begin(); 
   
  //Wait 5 minutes for the sensor to acclimate   delay(30000); 
   
 
  //Start Web server   server.on("/", handle_OnConnect);   server.onNotFound(handle_NotFound);   server.begin(); 
  Serial.println("HTTP server started"); 
 
  send_temp(); 
} 
 
//In loop Function refresh web page every second and send to sheets every 30 min void loop() {   int i=0;   while(i<1790){     server.handleClient();     delay(1000); 
    i++; 
  } 
  send_temp(); 
} 
 
//Function to send weather data to sheets 
void send_temp() 
{ 
  //Read data from DHT22    float h = dht.readHumidity();   float t = dht.readTemperature(); 
  float hic = dht.computeHeatIndex(t, h, false); 
   
  //If no data read from DHT22 display fault message   if (isnan(h) || isnan(t)) { 
       Serial.println(F("Failed to read from DHT sensor!"));        return; 
} 
 
  
 
  //Create JSON object from temperature data  
  String temp;   temp  = "temperature="+String(t, 6);   temp += "&humidity="+String(h, 6);   temp += "&heatindex="+String(hic,6);   send_to_sheet(temp); 
 
} 
 
//Function to send data to sheets void send_to_sheet(String params) { 
   HTTPClient http; 
   //Define URL by attaching g_script ID and JSON object 
   String url="https://script.google.com/macros/s/"+script_ID+"/exec?"+params; 
   Serial.println("Posting weather data to sheets:");    http.begin(url.c_str()); 
   http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
   //Recieve response from Web App    int httpCode = http.GET();      Serial.print("HTTP Status Code: "); 
   Serial.println(httpCode);    String payload;    if (httpCode > 0) { 
        payload = http.getString(); 
        Serial.println("Payload: "+payload);      
   } 
   http.end(); 
} 
//Function for when HTTP response code is 200 or Ok void handle_OnConnect() {   float  t=dht.readTemperature();   float  h=dht.readHumidity(); 
  float hic=dht.computeHeatIndex(t, h, false);   server.send(200, "text/html", SendHTML(t,h,hic)); } 
 
//Function for when HTTP response is 404 not found void handle_NotFound(){ 
  server.send(404, "text/plain", "Not found"); } 
 
 
 
 
//Create HTML string for Web server response 
String SendHTML(float t,float h,float hic){   String ptr = "<!DOCTYPE html> <html>\n"; 
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initialscale=1.0, user-scalable=no\">\n";   ptr +="<title>ESP32 Web Server</title>\n";   ptr +="<style>";   ptr +="html {";   ptr +="font-family: Arial;";   ptr +="display: inline-block;";   ptr +="margin: 0px auto;";   ptr +="text-align: center;}";   ptr +="</style>";   ptr +="<script>\n";   ptr +="setInterval(loadDoc,1000);\n";   ptr +="function loadDoc() {\n";   ptr +="var xhttp = new XMLHttpRequest();\n";   ptr +="xhttp.onreadystatechange = function() {\n";   ptr +="if (this.readyState == 4 && this.status == 200) {\n";   ptr +="document.body.innerHTML =this.responseText}\n";   ptr +="};\n"; 
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";   ptr +="xhttp.send();\n";   ptr +="}\n";   ptr +="</script>\n";   ptr +="</head>\n";   ptr +="<body>\n";   ptr +="<div id=\"webpage\">\n";   ptr +="<h1>Current Weather</h1>\n";   ptr +="<p>Temperature: "; 
  ptr +=t;   ptr +="&deg;C</p>";   ptr +="<p>Humidity: "; 
  ptr +=h;   ptr +="%</p>";   ptr +="<p>Heat Index: ";   ptr +=hic;   ptr +="&deg;C</p>";   ptr +="</div>\n";   ptr +="</body>\n";   ptr +="</html>\n"; 
  return ptr; 
} 
