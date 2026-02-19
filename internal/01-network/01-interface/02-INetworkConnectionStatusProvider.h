#ifndef INETWORKCONNECTIONSTATUSPROVIDER_H
#define INETWORKCONNECTIONSTATUSPROVIDER_H

#include <StandardDefines.h>

/**
 * Provides read-only network and WiFi connection status (e.g. from a store).
 */
DefineStandardPointers(INetworkConnectionStatusProvider)
class INetworkConnectionStatusProvider {
    Public Virtual ~INetworkConnectionStatusProvider() = default;

    Public Virtual Bool IsNetworkConnected() const = 0;
    Public Virtual Bool IsWifiConnected() const = 0;
    Public Virtual ULong GetNetworkConnectionId() const = 0;
};

#endif // INETWORKCONNECTIONSTATUSPROVIDER_H
