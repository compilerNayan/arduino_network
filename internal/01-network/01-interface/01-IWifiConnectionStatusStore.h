#ifndef IWIFICONNECTIONSTATUSSTORE_H
#define IWIFICONNECTIONSTATUSSTORE_H

#include <StandardDefines.h>

/**
 * Thread-safe store for network and WiFi connection status.
 */
DefineStandardPointers(IWifiConnectionStatusStore)
class IWifiConnectionStatusStore {
    Public Virtual ~IWifiConnectionStatusStore() = default;

    Public Virtual Bool IsNetworkConnected() const = 0;
    Public Virtual Bool IsWifiConnected() const = 0;
    Public Virtual Bool IsHotspotConnected() const = 0;
    Public Virtual ULong GetWifiConnectionId() const = 0;
    Public Virtual ULong GetHotspotConnectionId() const = 0;
    Public Virtual ULong GetNetworkConnectionId() const = 0;

    /** Set all connection state in one transaction (thread-safe atomic update). */
    Public Virtual Void SetState(Bool networkConnected, Bool wifiConnected, Bool hotspotConnected,
                                 ULong wifiConnectionId, ULong hotspotConnectionId, ULong networkConnectionId) = 0;
};

#endif // IWIFICONNECTIONSTATUSSTORE_H
