#pragma once

#include <ETH.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <helpers/ESP32Board.h>

#if defined(ESP_PLATFORM)

void onEthNetworkEvent(arduino_event_id_t event);

class OctopusLabLANBoard : public ESP32Board {
  AsyncWebServer* _ota_server = nullptr;
  bool _eth_started = false;
  bool _connected = false;
  wifi_event_id_t _event_got_ip = 0;
  wifi_event_id_t _event_connected = 0;
  wifi_event_id_t _event_disconnected = 0;
  wifi_event_id_t _event_lost_ip = 0;
  wifi_event_id_t _event_stop = 0;

public:
  void ethBegin(SPIClass& spi) {
    if (_event_got_ip == 0) {
      _event_connected = WiFi.onEvent(onEthNetworkEvent, ARDUINO_EVENT_ETH_CONNECTED);
      _event_got_ip = WiFi.onEvent(onEthNetworkEvent, ARDUINO_EVENT_ETH_GOT_IP);
      _event_disconnected = WiFi.onEvent(onEthNetworkEvent, ARDUINO_EVENT_ETH_DISCONNECTED);
#ifdef ARDUINO_EVENT_ETH_LOST_IP
      _event_lost_ip = WiFi.onEvent(onEthNetworkEvent, ARDUINO_EVENT_ETH_LOST_IP);
#endif
      _event_stop = WiFi.onEvent(onEthNetworkEvent, ARDUINO_EVENT_ETH_STOP);
    }

    if (!_eth_started) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        // SDK 5.x (Arduino SDK 3.x)
        #pragma message("Compiling ETH init: SDK 5.x (Arduino core 3.x)")
        #ifdef LAN_MDC
        #pragma message("Compiling EMAC ETH")
        ETH.begin(ETH_PHY_TYPE, LAN_PHY_ADDR, LAN_MDC, LAN_MDIO, LAN_POWER, LAN_CLOCK_MODE);
        #endif

        #ifdef LAN_CS
        #pragma message("Compiling SPI ETH")
        Serial.println("Init ETH");
        ETH.begin(ETH_PHY_TYPE, LAN_PHY_ADDR, LAN_CS, LAN_IRQ, LAN_RST, spi);
        #endif
#else
        // SDK 4.x (Arduino SDK 2.x)
        #pragma message("Compiling ETH init: SDK 4.x (Arduino core 2.x)")
        ETH.begin(LAN_PHY_ADDR, LAN_POWER, LAN_MDC, LAN_MDIO, ETH_PHY_TYPE, LAN_CLOCK_MODE);
#endif
      _eth_started = true;
      inhibit_sleep = true;
    }
  }

  const char* getManufacturerName() const override {
    return "OctopusLab S3 LAN LoRa";
  }

  bool isNetworkConnected() const {
    return _connected;
  }

  void setNetworkConnected(bool connected) {
    _connected = connected;
  }

  bool startOTAUpdate(const char* id, char reply[]) override;
};

#endif
