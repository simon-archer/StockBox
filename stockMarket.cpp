//Inside stockmarket.cpp
#include "stockMarket.h"

#define TFT_CLK  A3
#define TFT_MOSI A5
#define TFT_CS   D6
#define TFT_DC   D5
#define TFT_RST  D7

#define stocks 10
#define gav_price 1.60
#define purchase_price_nok 17.15

float old_price = 0.0;
float old_price_nok = 0.0;
float exchange_rate = 10;
bool earlyHours;
bool afterHours;
bool marketOpen;


void initDisplay() {
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
}

void tftDisplay(int cursorPos, const String& label, const String& value, int textSize, bool isIncreaseValue, bool isWhiteValue) {
    tft.setRotation(3);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(textSize);
    tft.fillRect(0, cursorPos, tft.width(), textSize * 8, ST7735_BLACK);
    tft.setCursor(0, cursorPos);
    tft.print(label); // Use print instead of println
    tft.setTextColor(isWhiteValue ? ST7735_WHITE : (isIncreaseValue ? ST7735_GREEN : ST7735_RED));
    tft.println(isWhiteValue ? value : (isIncreaseValue ? value + "\x1E" : value + "\x1F")); // println is used to move cursor to new line for next text
}

float fetchFloatData(const char* data, const char* key) {
    // Create a temporary modifiable buffer
    char json[256];
    strncpy(json, data, sizeof(json));
    json[sizeof(json) - 1] = '\0';

    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
        // Handle parsing error
        return 0.0;
    }

    return root[key].as<float>();
}

String fetchStringData(const char* data, const char* key) {
    // Create a temporary modifiable buffer
    char json[256];
    strncpy(json, data, sizeof(json));
    json[sizeof(json) - 1] = '\0';

    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
        // Handle parsing error
        return "";
    }

    return root[key].asString();
}

void fetchExchangeRateData(const char* data) {
    char json[256];
    strncpy(json, data, sizeof(json));
    json[sizeof(json) - 1] = '\0';

    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
        // Handle parsing error
        return;
    }

    exchange_rate = root["data"]["NOK"]["value"].as<float>(); // Access each nested object separately
    Serial.println("Fetched exchange rate");
}

void fetchPriceData(const char* data) {
    float price = fetchFloatData(data, "price");
    float change_point = fetchFloatData(data, "change_point");
    float change_percentage = fetchFloatData(data, "change_percentage");
    String total_vol = fetchStringData(data, "total_vol");
    
    float change_point_nok = change_point * exchange_rate;
    float price_nok = price * exchange_rate; // Calculate the price in NOK
    float total_value = stocks * price_nok; // Calculate total value
    float yield_percentage = (price - gav_price) / gav_price * 100; // Calculate percentage yield
    float yield_nok = stocks * (price_nok - purchase_price_nok); // Calculate yield in KR
    
    float old_price_temp = old_price;
    float old_price_nok_temp = old_price_nok;

    old_price = price;
    old_price_nok = price_nok;

    tft.fillScreen(ST7735_BLACK);
    tftDisplay(0, "Lilium               $LILM", "", 1, false, true);
    tftDisplay(10, "Price($) ", String(price, 2), 2, price > old_price_temp, false);
    tftDisplay(30, "Price(KR)", String(price_nok, 1), 2, price_nok > old_price_nok_temp, false);
    tftDisplay(50, "Change(%)           ", String(change_percentage, 1), 1, false, true);
    tftDisplay(60, "Change(KR)          ", String(change_point_nok, 1), 1, false, true);
    tftDisplay(70, "Total volume        ", total_vol, 1, false, true);
    tftDisplay(90, "My Stocks            10", "", 1, false, true); // Hard-coded, replace if necessary
    tftDisplay(100, "Total value(KR)       ", String(total_value, 0), 1, false, true);
    tftDisplay(110, "Return(%)          ", String(yield_percentage, 1), 1, false, true);
    tftDisplay(120, "Return(KR)         ", String(yield_nok, 0), 1, false, true);
    Serial.println("Fetched stock price data");
}

void fetchMarketStatusData(const char* data) {
    const char* nasdaqStatusStart = strstr(data, "\"nasdaq\":");
    if (nasdaqStatusStart == nullptr) {
        tftDisplay(50, "Parsing error - nasdaq status", String(data), 1, false, true);
        return;
    }

    const char* nasdaqStatusEnd = strchr(nasdaqStatusStart, ',');
    if (nasdaqStatusEnd == nullptr) {
        tftDisplay(50, "Parsing error - nasdaq status", String(data), 1, false, true);
        return;
    }

    const char* nasdaqStatus = nasdaqStatusStart + strlen("\"nasdaq\":") + 1;
    int nasdaqStatusLength = nasdaqStatusEnd - nasdaqStatus - 1;

    char nasdaqStatusValue[11]; // Maximum length for "extended-hours" which is the longest possible value
    strncpy(nasdaqStatusValue, nasdaqStatus, nasdaqStatusLength);
    nasdaqStatusValue[nasdaqStatusLength] = '\0'; // Null-terminate the string

    // Check if the market is open
    marketOpen = strcmp(nasdaqStatusValue, "open") == 0;

    //Fetch "afterHours"
    const char* afterHoursStart = strstr(data, "\"afterHours\":");
    if (afterHoursStart != nullptr) {
        const char* afterHoursValueStart = afterHoursStart + strlen("\"afterHours\":");
        const char* afterHoursValueEnd = strchr(afterHoursValueStart, ',');
        if (afterHoursValueEnd != nullptr) {
            int length = afterHoursValueEnd - afterHoursValueStart;
            char afterHoursValue[length + 1];
            strncpy(afterHoursValue, afterHoursValueStart, length);
            afterHoursValue[length] = '\0'; // Null-terminate the string
            afterHours = strcmp(afterHoursValue, "true") == 0;
        }
    }

    // Fetch "earlyHours"
    const char* earlyHoursStart = strstr(data, "\"earlyHours\":");
    if (earlyHoursStart != nullptr) {
        const char* earlyHoursValueStart = earlyHoursStart + strlen("\"earlyHours\":");
        const char* earlyHoursValueEnd = strchr(earlyHoursValueStart, ',');
        if (earlyHoursValueEnd != nullptr) {
            int length = earlyHoursValueEnd - earlyHoursValueStart;
            char earlyHoursValue[length + 1];
            strncpy(earlyHoursValue, earlyHoursValueStart, length);
            earlyHoursValue[length] = '\0'; // Null-terminate the string
            earlyHours = strcmp(earlyHoursValue, "true") == 0;
        }
    }
    Serial.println("Fetched market status data");
}

void handleStockWebhookResponse(const char* event, const char* data) {
    fetchPriceData(data);
}

void handleExchangeRateWebhookResponse(const char* event, const char* data) {
    fetchExchangeRateData(data);
}

void handleMarketStatusWebhookResponse(const char* event, const char* data) {
    fetchMarketStatusData(data);
}

void fetchStockPrice() {
    Particle.publish("fetchPrice", "", PRIVATE);
}

void fetchExchangeRate() {
    Particle.publish("fetchExchangeRate", "", PRIVATE);
}

void fetchMarketStatus() {
    Particle.publish("fetchMarketStatus", "", PRIVATE);
}
// trackingScreen() function stays the same
void trackingScreen() {
    tft.fillScreen(ST7735_BLACK);
    fetchStockPrice();
    fetchExchangeRate();
    fetchMarketStatus();
}
