//Inside stockmarket.h
#ifndef STOCKMARKET_H
#define STOCKMARKET_H

#include <Adafruit_ST7735.h>
#include <SparkJson.h>

extern Adafruit_ST7735 tft;

extern float exchange_rate;
extern bool earlyHours;
extern bool afterHours;
extern bool marketOpen;

void initDisplay();
void tftDisplay(int cursorPos, const String& label, const String& value, int textSize, bool isIncreaseValue, bool isWhiteValue);
float fetchFloatData(const char* data, const char* key);
String fetchStringData(const char* data, const char* key); 
void fetchExchangeRateData(const char* data);
void fetchPriceData(const char* data);
void fetchMarketStatusData(const char* data);
void handleStockWebhookResponse(const char* event, const char* data); 
void handleExchangeRateWebhookResponse(const char* event, const char* data);
void handleMarketStatusWebhookResponse(const char* event, const char* data);
void fetchStockPrice();
void fetchExchangeRate();
void fetchMarketStatus();
void trackingScreen();

#endif // STOCKMARKET_H
