#pragma once

#include <ETH.h>
#include <esp_sntp.h>
#include <helpers/AutoDiscoverRTCClock.h>

class OctopusLabLANRTCClock : public AutoDiscoverRTCClock {
  unsigned long _next_ntp_check = 0;
  uint32_t _last_ntp_sync = 0;

public:
  OctopusLabLANRTCClock(mesh::RTCClock& fallback) : AutoDiscoverRTCClock(fallback) {}

  void begin(TwoWire& wire);
  void tick() override;
};
