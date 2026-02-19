#ifndef INETWORKCONNECTION_H
#define INETWORKCONNECTION_H

#include <StandardDefines.h>

/**
 * Network connection: connect/disconnect WiFi or hotspot, query status, ensure connectivity.
 * Mirrors INetworkManager for use in arduino_network.
 */
DefineStandardPointers(INetworkConnection)
class INetworkConnection {
    Public Virtual ~INetworkConnection() = default;

    Public Virtual Void ConnectNetwork() = 0;
    Public Virtual Void DisconnectNetwork() = 0;
    Public Virtual Bool IsNetworkConnected() = 0;
    Public Virtual ULong GetWifiConnectionId() = 0;
    Public Virtual ULong GetNetworkConnectionId() = 0;
    Public Virtual Bool EnsureNetworkConnectivity() = 0;
    Public Virtual Void RestartNetwork() = 0;
};

#endif // INETWORKCONNECTION_H
