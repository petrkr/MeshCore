#pragma once

#include <ETH.h>
#include <ESPAsyncWebServer.h>
#include <helpers/ESP32Board.h>

#if defined(ESP_PLATFORM)

class OctopusLabLANBoard : public ESP32Board {
  AsyncWebServer* _ota_server = nullptr;
  bool _eth_started = false;

public:
  void begin() {
    ESP32Board::begin();

    if (!_eth_started) {
      ETH.begin(LAN_PHY_ADDR, LAN_POWER, LAN_MDC, LAN_MDIO, ETH_PHY_TYPE, LAN_CLOCK_MODE);
      _eth_started = true;
      inhibit_sleep = true;
    }
  }

  const char* getManufacturerName() const override {
    return "OctopusLab LAN LoRa";
  }

  bool startOTAUpdate(const char* id, char reply[]) override;
};

#endif
