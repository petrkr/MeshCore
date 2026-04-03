#include "OctopusLabLANRTCClock.h"

#include <time.h>

namespace {
constexpr unsigned long NTP_RETRY_MILLIS = 10000;
constexpr unsigned long NTP_RESYNC_MILLIS = 3600000;
}

void OctopusLabLANRTCClock::begin(TwoWire& wire) {
  AutoDiscoverRTCClock::begin(wire);
  configTzTime("UTC0", "pool.ntp.org", "time.nist.gov", "time.google.com");
  _next_ntp_check = millis() + NTP_RETRY_MILLIS;
}

void OctopusLabLANRTCClock::tick() {
  AutoDiscoverRTCClock::tick();

  if (!ETH.linkUp()) {
    return;
  }

  unsigned long now = millis();
  if ((long)(now - _next_ntp_check) < 0) {
    return;
  }

  if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
    time_t ntp_now;
    time(&ntp_now);
    if (ntp_now > 1715770351 && (uint32_t)ntp_now != _last_ntp_sync) {
      setCurrentTime((uint32_t)ntp_now);
      _last_ntp_sync = (uint32_t)ntp_now;
      _next_ntp_check = now + NTP_RESYNC_MILLIS;
      return;
    }
  }

  _next_ntp_check = now + NTP_RETRY_MILLIS;
}
