#include <Arduino.h>
#include <SFE_BMP180.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define DELAY 5000

String URL = "http://192.168.0.107/test.php"; // use your local IP address

// paste yout WiFi params
char ssid[] = "";
char password[] = "";

WiFiServer server(80);
WiFiClient client;

SFE_BMP180 bmp180;
HTTPClient http;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Initialization started");
  bmp180.begin();
  http.begin(URL);  

  Serial.println("Connceting to WiFi");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("...");
    delay(100);
  }

  Serial.print("\nConnected\nIP = ");
  Serial.println(WiFi.localIP());
  
  // Serial.println("Made by Frundin and Ershov");
}

void loop() 
{
  double T, P;
  char status = 0;
  status = bmp180.startTemperature();
  if(status)
  {
    delay(status);
    status = bmp180.getTemperature(T);
    if(status)
    {
      Serial.print("Temp: ");
      Serial.println(T);

      status = bmp180.startPressure(1);
      if(status)
      {
        delay(status);

        status = bmp180.getPressure(P,T);
        if(status)
        {
          P = (P*760.0*100) / 101325.0;
          Serial.print("Pressure: ");
          Serial.println(P);
        }

      }
    }
  }
  // double T = 29.11;  // for test without BMP180
  // double P = 111.2;

  String postData = "Temp=" + String(T) + "&Press=" + String(P);
  int httpCode = http.POST(postData);
  String payload = http.getString();
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  //Debug info
  Serial.println("URL: " + String(URL));
  Serial.println("Data: " + postData);
  Serial.println("httpCode: " + String(httpCode));
  Serial.println("payload: " + payload);
  Serial.println("--------------------------");
  delay(DELAY);
}

