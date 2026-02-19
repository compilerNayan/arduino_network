#ifndef INETWORKCONNECTIONSTATUSSTORE_H
#define INETWORKCONNECTIONSTATUSSTORE_H

#include <StandardDefines.h>

/**
 * Thread-safe store for network and WiFi connection status.
 */
DefineStandardPointers(INetworkConnectionStatusStore)
class INetworkConnectionStatusStore {
    Public Virtual ~INetworkConnectionStatusStore() = default;

    Public Virtual Bool IsNetworkConnected() const = 0;
    Public Virtual Bool IsWifiConnected() const = 0;
    Public Virtual Void SetNetworkConnected(Bool connected) = 0;
    Public Virtual Void SetWifiConnected(Bool connected) = 0;
};

#endif // INETWORKCONNECTIONSTATUSSTORE_H
