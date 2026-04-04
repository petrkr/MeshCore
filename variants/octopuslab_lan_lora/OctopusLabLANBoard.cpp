#ifdef ESP_PLATFORM

#include "OctopusLabLANBoard.h"
#include "target.h"

void onEthNetworkEvent(arduino_event_id_t event) {
  if (event == ARDUINO_EVENT_ETH_CONNECTED) {
    MESH_DEBUG_PRINTLN("ETH Connected");
  } else if (event == ARDUINO_EVENT_ETH_GOT_IP) {
    board.setNetworkConnected(true);
    MESH_DEBUG_PRINTLN("ETH Got IP: %s", ETH.localIP().toString().c_str());
#ifdef ARDUINO_EVENT_ETH_LOST_IP
  } else if (event == ARDUINO_EVENT_ETH_LOST_IP) {
    board.setNetworkConnected(false);
    MESH_DEBUG_PRINTLN("ETH Lost IP");
#endif
  } else if (event == ARDUINO_EVENT_ETH_DISCONNECTED) {
    board.setNetworkConnected(false);
    MESH_DEBUG_PRINTLN("ETH Disconnected");
  } else if (event == ARDUINO_EVENT_ETH_STOP) {
    board.setNetworkConnected(false);
    MESH_DEBUG_PRINTLN("ETH Stopped");
  }
}

#if defined(ADMIN_PASSWORD) && !defined(DISABLE_WIFI_OTA)

#include <ETH.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SPIFFS.h>

bool OctopusLabLANBoard::startOTAUpdate(const char* id, char reply[]) {
  if (_ota_server == nullptr) {
    static char id_buf[60];
    snprintf(id_buf, sizeof(id_buf), "%s (%s)", id, getManufacturerName());

    static char home_buf[96];
    snprintf(home_buf, sizeof(home_buf), "<H2>Hi! I am a MeshCore Repeater. ID: %s</H2>", id);

    _ota_server = new AsyncWebServer(80);
    _ota_server->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "text/html", home_buf);
    });
    _ota_server->on("/log", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(SPIFFS, "/packet_log", "text/plain");
    });

    AsyncElegantOTA.setID(id_buf);
    AsyncElegantOTA.begin(_ota_server);
    _ota_server->begin();
  }

  IPAddress ip = ETH.localIP();
  snprintf(reply, 160, "Started: http://%u.%u.%u.%u/update", ip[0], ip[1], ip[2], ip[3]);
  MESH_DEBUG_PRINTLN("startOTAUpdate: %s", reply);
  return true;
}

#else
bool OctopusLabLANBoard::startOTAUpdate(const char* id, char reply[]) {
  (void)id;
  (void)reply;
  return false; // not supported
}

#endif
#endif
