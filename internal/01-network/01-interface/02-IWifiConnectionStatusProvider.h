#ifndef IWIFICONNECTIONSTATUSPROVIDER_H
#define IWIFICONNECTIONSTATUSPROVIDER_H

#include <StandardDefines.h>

/**
 * Provides read-only network and WiFi connection status (e.g. from a store).
 */
DefineStandardPointers(IWifiConnectionStatusProvider)
class IWifiConnectionStatusProvider {
    Public Virtual ~IWifiConnectionStatusProvider() = default;

    Public Virtual Bool IsNetworkConnected() const = 0;
    Public Virtual Bool IsWifiConnected() const = 0;
    Public Virtual Bool IsHotspotConnected() const = 0;
    Public Virtual ULong GetWifiConnectionId() const = 0;
};

#endif // IWIFICONNECTIONSTATUSPROVIDER_H
