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
    Public Virtual Void SetNetworkConnected(Bool connected) = 0;
    Public Virtual Void SetWifiConnected(Bool connected) = 0;
    Public Virtual Void SetHotspotConnected(Bool connected) = 0;
    Public Virtual Void SetWifiConnectionId(ULong connectionId) = 0;
    Public Virtual Void SetHotspotConnectionId(ULong connectionId) = 0;
    Public Virtual Void SetNetworkConnectionId(ULong connectionId) = 0;
};

#endif // IWIFICONNECTIONSTATUSSTORE_H
