// Your main code
#include "bitmapUtils.h"
#include "aftermarket.h"
#include "premarket.h"
#include "animations.h"
#include "stockMarket.h"
#include <Adafruit_ST7735.h>
#include <SparkJson.h>

#define TFT_CLK  A3
#define TFT_MOSI A5
#define TFT_CS   D6
#define TFT_DC   D5
#define TFT_RST  D7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

enum SetScreenState {
    SYNCHRONIZING,
    TRACKING,
    PRE_MARKET,
    AFTER_MARKET
};


bool playAnimation = true;

SetScreenState setScreenState = SYNCHRONIZING;

unsigned long lastMarketFetchTime = 0; // Time when we last fetched market status
unsigned long lastPriceFetchTime = 0; // Time when we last fetched price

const unsigned long marketFetchInterval = 60000; // Fetch market status every 1 minute
const unsigned long priceFetchInterval = 30000; // Fetch price every 30 seconds when market is open

unsigned long lastExchangeRateFetchTime = 0;
const unsigned long exchangeRateFetchInterval = 17280000; // Fetch exchange rate every 4.8 hours

unsigned long lastAnimationTime = 0;
const unsigned long animationInterval = 400; // Change as per your animation speed


struct MyWiFiCredentials {
    const char *ssid;
    const char *password;
};

MyWiFiCredentials wifiCredentials[] = {
    {"Wifi_1", "Wifi_Password_1"},
    {"Wifi_2", "Wifi_Password_2"}
};

void setup() {
    // Subscribe to the event
    initDisplay();
    
    WiFi.on();
    WiFi.clearCredentials();  // Clear any previous WiFi credentials
    for (size_t i = 0; i < sizeof(wifiCredentials) / sizeof(MyWiFiCredentials); ++i) {
        WiFi.setCredentials(wifiCredentials[i].ssid, wifiCredentials[i].password);
        WiFi.connect();
        waitFor(WiFi.ready, 5000);  // Wait up to 5 seconds for the WiFi to be ready
        if (WiFi.ready()) {
            Serial.printlnf("Connected to %s", wifiCredentials[i].ssid);
            break;
        }
        else {
            Serial.printlnf("Failed to connect to %s", wifiCredentials[i].ssid);
            // If failed to connect, clear credentials and move to next network
            WiFi.clearCredentials();
        }
    }
    
    Particle.subscribe("hook-response/fetchPrice", handleStockWebhookResponse, MY_DEVICES);
    Particle.subscribe("hook-response/fetchExchangeRate", handleExchangeRateWebhookResponse, MY_DEVICES);
    Particle.subscribe("hook-response/fetchMarketStatus", handleMarketStatusWebhookResponse, MY_DEVICES);

    playAnimation = false;

    fetchMarketStatus();
    fetchExchangeRate();
    Particle.process();

    lastMarketFetchTime = millis();

    if (marketOpen) {
        setScreenState = TRACKING;
        Serial.println("Market is open");
    } else if (earlyHours) {
        setScreenState = PRE_MARKET;
        Serial.println("Early Hours");
    } else if (afterHours) {
        setScreenState = AFTER_MARKET;
        Serial.println("After Hours");
    }
}

void loop() {
    unsigned long currentTime = millis();
    static int previousScreenState = -1;
    static unsigned long lastAnimationTime = 0; // keep track of last animation time
    const unsigned long animationInterval = 400; // time between animation frames
    static int animationStep = 0; // keep track of animation step
    const int animationSteps = 5; // total steps in the animation

    if (currentTime - lastMarketFetchTime > marketFetchInterval) {
        fetchMarketStatus();
        lastMarketFetchTime = currentTime;

        if (marketOpen) {
            setScreenState = TRACKING;
            Serial.println("Market is open");
        } else if (earlyHours) {
            setScreenState = PRE_MARKET;
            Serial.println("Early Hours");
        } else if (afterHours) {
            setScreenState = AFTER_MARKET;
            Serial.println("After Hours");
        }
    }
    
    if (currentTime - lastExchangeRateFetchTime > exchangeRateFetchInterval) {
    fetchExchangeRate();
    lastExchangeRateFetchTime = currentTime;
    Serial.println("Exchange rate fetched");
    }

    if (setScreenState == TRACKING) {
        if (currentTime - lastPriceFetchTime > priceFetchInterval) {
            fetchStockPrice();
            lastPriceFetchTime = currentTime;
        }
    }

    if (setScreenState != previousScreenState) {
        switch (setScreenState) {
            case SYNCHRONIZING:
                // Display "Synchronizing..." again in case the setup display was overwritten
                tft.fillScreen(ST7735_BLACK);
                tft.setRotation(3);
                tft.setTextColor(ST7735_WHITE);
                tft.setCursor(11, 60); // adjust these values as necessary
                tft.println("Connecting to the cloud");
                playAnimation = false;
                break;
            case TRACKING:
                trackingScreen();
                playAnimation = false;
                break;
            case PRE_MARKET:
                playAnimation = true;
                preScreen(tft);
                break;
            case AFTER_MARKET:
                playAnimation = true;
                aftScreen(tft);
                break;
        }
        previousScreenState = setScreenState;
        Serial.println("Screenstate changed");
    }

    if (playAnimation && currentTime - lastAnimationTime > animationInterval) {
        // More debug prints
        Serial.println("Animating...");

        int16_t x = 65 + animationStep;
        animateBitmap(x, 80, mouth, MOUTH_WIDTH, MOUTH_HEIGHT, tft);
        lastAnimationTime = currentTime;
        animationStep++;

        if (animationStep >= animationSteps) {
            animationStep = 0;
        }
    }

    Particle.process();
}


