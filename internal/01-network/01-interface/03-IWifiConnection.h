#ifndef IWIFICONNECTION_H
#define IWIFICONNECTION_H

#include <StandardDefines.h>

/**
 * WiFi connection: connect/disconnect WiFi or hotspot, query status, ensure connectivity.
 * Mirrors INetworkManager for use in arduino_network.
 */
DefineStandardPointers(IWifiConnection)
class IWifiConnection {
    Public Virtual ~IWifiConnection() = default;

    Public Virtual Void ConnectNetwork() = 0;
    Public Virtual Void DisconnectNetwork() = 0;
    Public Virtual Bool IsNetworkConnected() = 0;
    Public Virtual Bool IsWifiConnected() = 0;
    Public Virtual Bool IsHotspotConnected() = 0;
    Public Virtual ULong GetWifiConnectionId() = 0;
    Public Virtual ULong GetNetworkConnectionId() = 0;
    Public Virtual Bool EnsureNetworkConnectivity() = 0;
    Public Virtual Void RestartNetwork() = 0;
};

#endif // IWIFICONNECTION_H
