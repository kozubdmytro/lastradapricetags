#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include <string.h>


// WiFi
const char* ssid = "ok"; // "ok"; // TakeAway
const char* password = "12345612"; // "12345612"; // lastrada

ESP8266WebServer server(80);

#define EPD_WIDTH   EPD_2IN13B_V4_WIDTH
#define EPD_HEIGHT  EPD_2IN13B_V4_HEIGHT

char text1[32] = "AKTION";
char text2[32] = "IP";
char text3[32] = "";
char text4[32] = "1,00";
char text5[32] = "";

bool redModeEnabled = false;

UBYTE *BlackImage = NULL;
UBYTE *RedImage   = NULL;
UWORD ImageSize   = 0;


void updateDisplay() {
    if (BlackImage != NULL) {
      free(BlackImage);
      BlackImage = NULL;
    }
    if (RedImage != NULL) {
      free(RedImage);
      RedImage = NULL;
    }

    ImageSize = ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
  
    BlackImage = (UBYTE *)malloc(ImageSize);
    if (BlackImage == NULL) return;
    RedImage = (UBYTE *)malloc(ImageSize);
    if (RedImage == NULL) {
      free(BlackImage);
      return;
    }

    char priceWithEuro[32];

    strncpy(priceWithEuro, text4, sizeof(priceWithEuro) - 1);
    priceWithEuro[sizeof(priceWithEuro) - 1] = '\0';

    strncat(priceWithEuro, "$", sizeof(priceWithEuro) - strlen(priceWithEuro) - 1);

    int textWidth = strlen(text4) * Font24.Width; 
    int textWidth5 = strlen(text5) * Font20.Width;
    int textHeight = Font20.Height;    
    int x0 = 220 - textWidth - textWidth5 - 20;
    int y0 = 90;

    if (strcmp(text2, "IP") == 0) {
       strncpy(text3, WiFi.localIP().toString().c_str(), sizeof(text3) - 1);
       text3[sizeof(text3) - 1] = '\0';
    }
    else {}
  
    if (!redModeEnabled) {
      Paint_NewImage(BlackImage, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
      Paint_Clear(WHITE);
      Paint_NewImage(RedImage, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
      Paint_Clear(WHITE);

      Paint_SelectImage(BlackImage);
      Paint_DrawString_EN(100,  5,  "La Strada", &Font12, WHITE, BLACK); //12
      Paint_DrawString_EN(5,   35, text2, &Font20, WHITE, BLACK); //20
       if (text2 == "IP") {
      Paint_DrawString_EN(5, 55, WiFi.localIP().toString().c_str(), &Font20, WHITE, BLACK);
    }
    else {}
      Paint_DrawString_EN(5,   55, text3, &Font20, WHITE, BLACK); //20
      Paint_DrawString_EN(220 - textWidth, 90, priceWithEuro, &Font24, WHITE, BLACK); //24
    }
    else {
      Paint_NewImage(BlackImage, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
      Paint_Clear(WHITE);
      Paint_NewImage(RedImage, EPD_WIDTH, EPD_HEIGHT, 270, BLACK);
      Paint_Clear(BLACK);
      
      Paint_SelectImage(RedImage);
      Paint_DrawString_EN(70,  5,  text1, &Font24, WHITE, WHITE); //24
      Paint_DrawString_EN(5,   35, text2, &Font20, WHITE, WHITE); //20
      Paint_DrawString_EN(5,   55, text3, &Font20, WHITE, WHITE); //20
      Paint_DrawString_EN(220 - textWidth, 90, priceWithEuro, &Font24, WHITE, WHITE); //24
      Paint_DrawString_EN(x0, y0, text5, &Font20, WHITE, WHITE); //20
      if (strcmp(text5, "") == 0) {} 
        else {
          Paint_DrawLine(x0, y0 + textHeight, x0 + textWidth, y0, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
       }
      
    }
  
    EPD_2IN13B_V4_Display(BlackImage, RedImage);
    DEV_Delay_ms(4000);

    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
}

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  String html = file.readString();
  file.close();

  html.replace("{text1}", String(text1));
  html.replace("{text2}", String(text2));
  html.replace("{text3}", String(text3));
  html.replace("{text4}", String(text4));
  html.replace("{text5}", String(text5));
  
  server.send(200, "text/html", html);
}

void handleUpdate() {
  if (server.hasArg("text1")) {
    strncpy(text1, server.arg("text1").c_str(), sizeof(text1) - 1);
    text1[sizeof(text1) - 1] = '\0';
  }
  if (server.hasArg("text2")) {
    strncpy(text2, server.arg("text2").c_str(), sizeof(text2) - 1);
    text2[sizeof(text2) - 1] = '\0';
  }
  if (server.hasArg("text3")) {
    strncpy(text3, server.arg("text3").c_str(), sizeof(text3) - 1);
    text3[sizeof(text3) - 1] = '\0';
  }
  if (server.hasArg("text4")) {
    strncpy(text4, server.arg("text4").c_str(), sizeof(text4) - 1);
    text4[sizeof(text4) - 1] = '\0';
  }
  if (server.hasArg("text5")) {
    strncpy(text5, server.arg("text5").c_str(), sizeof(text5) - 1);
    text5[sizeof(text5) - 1] = '\0';
  }
  
  if (server.hasArg("redMode")) {
    redModeEnabled = true;
  } else {
    redModeEnabled = false;
  }
  
  updateDisplay();
  
server.send(200, "text/html", 
  "<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<meta charset='UTF-8'>"
      "<title>Aktualisiert</title>"
      "<style>"
        "body { margin:0; padding:0; font-family: Arial, sans-serif; background:#f2f2f2; }"
        ".container { display: flex; justify-content: center; align-items: center; min-height: 100vh; }"
        ".message-box { background:#fff; padding:40px; border-radius:20px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); text-align: center; width:100%; max-width:fit-content; scale: 160%; }"
        "a.button { display:inline-block; margin-top:20px; padding:12px 20px; background:#007BFF; color:#fff; text-decoration:none; border-radius:4px; }"
      "</style>"
    "</head>"
    "<body>"
      "<div class='container'>"
        "<div class='message-box'>"
          "<h1>Bildschirm wurde aktualisiert</h1>"
          "<a class='button' href='/'>Zurück</a>"
        "</div>"
      "</div>"
    "</body>"
  "</html>"
);
}

void setup() {
    Serial.begin(115200);

    DEV_Module_Init();
    EPD_2IN13B_V4_Init();
    EPD_2IN13B_V4_Clear();
    DEV_Delay_ms(500);
    printf("Started\r\n");

    WiFi.begin(ssid, password);
    pinMode(LED_BUILTIN, OUTPUT);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.print(".");
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print(".");
      delay(200);
    }
    Serial.println("\nWiFi connected");
    Serial.println("IP address: " + WiFi.localIP().toString());

    if (!LittleFS.begin()) {
      Serial.println("Ошибка монтирования LittleFS");
      return;
    }

    server.on("/", handleRoot);
    server.on("/update", handleUpdate);
    server.begin();
    Serial.println("HTTP server running");

    digitalWrite(LED_BUILTIN, LOW);
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
  
    updateDisplay();
}

void loop() {
    server.handleClient();
}
