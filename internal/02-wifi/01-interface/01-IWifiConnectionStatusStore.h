#ifndef IWIFICONNECTIONSTATUSSTORE_H
#define IWIFICONNECTIONSTATUSSTORE_H

#include <StandardDefines.h>

/**
 * Thread-safe store for WiFi/hotspot connection status (no network-level getters).
 */
DefineStandardPointers(IWifiConnectionStatusStore)
class IWifiConnectionStatusStore {
    Public Virtual ~IWifiConnectionStatusStore() = default;

    Public Virtual Bool IsWifiConnected() const = 0;
    Public Virtual Bool IsHotspotConnected() const = 0;
    Public Virtual ULong GetWifiConnectionId() const = 0;
    Public Virtual ULong GetHotspotConnectionId() const = 0;

    /** Set all connection state in one transaction (thread-safe atomic update). */
    Public Virtual Void SetState(Bool wifiConnected, Bool hotspotConnected,
                                 ULong wifiConnectionId, ULong hotspotConnectionId) = 0;
};

#endif // IWIFICONNECTIONSTATUSSTORE_H
