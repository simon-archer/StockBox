# Stock Market Tracker for Particle Photon

This repository contains code for a real-time stock market tracker using the Particle Photon, a popular Wi-Fi-enabled development board.

## Overview

The application connects to a Wi-Fi network and fetches data from various APIs. It then displays this data on an Adafruit ST7735 TFT screen and can animate according to market status. It fetches market status every minute and stock prices every 30 seconds when the market is open.

## Setup

To run this code, you will need:

1. A Particle Photon
2. An Adafruit ST7735 TFT screen
3. Credentials for a Wi-Fi network

To run the code, follow these steps:

1. Connect the TFT screen to your Particle Photon as follows:
   * TFT_CLK to A3
   * TFT_MOSI to A5
   * TFT_CS to D6
   * TFT_DC to D5
   * TFT_RST to D7
2. Edit the Wi-Fi credentials in the `MyWiFiCredentials` struct in the code.
3. Upload the code to your Particle Photon using the Particle IDE.

The program will attempt to connect to each of the provided Wi-Fi networks in the `MyWiFiCredentials` struct. Once a successful connection is established, it will start fetching market status, exchange rate, and stock price data and displaying it on the TFT screen.

## Screens

The screen can display in four states:
* `SYNCHRONIZING`: While connecting to the cloud
* `TRACKING`: While the market is open
* `PRE_MARKET`: During early trading hours
* `AFTER_MARKET`: After trading hours have ended

The screen will animate during the `PRE_MARKET` and `AFTER_MARKET` states.

## Data Sources

This project fetches data from the following APIs:

1. **Exchange Rate**: [Currency API](https://api.currencyapi.com) - The free tier allows for 300 requests per month.

2. **Market Status**: [Polygon](https://api.polygon.io) - The free tier allows for 5 API calls every minute (not all endpoints).

3. **Stock Price**: [Real Stonks API](https://realstonks.p.rapidapi.com/) - The free tier allows for 100,000 requests each month. An additional 1,000,000 requests can be purchased for $5 per month.

## Libraries

This project uses the following libraries:
* `Adafruit_ST7735.h` : To handle the display
* `SparkJson.h` : To parse the JSON data received from the APIs

Please ensure you have these libraries installed before compiling.

## Contact

For any questions or issues, please open an issue on this GitHub repository.
