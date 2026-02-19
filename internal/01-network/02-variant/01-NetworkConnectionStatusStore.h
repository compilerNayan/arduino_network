#ifndef NETWORKCONNECTIONSTATUSSTORE_H
#define NETWORKCONNECTIONSTATUSSTORE_H

#include "../01-interface/01-INetworkConnectionStatusStore.h"
#include <StandardDefines.h>
#include <atomic>

/**
 * Thread-safe implementation using two atomic booleans for network and WiFi status.
 */
class NetworkConnectionStatusStore : public INetworkConnectionStatusStore {
    Private std::atomic<bool> networkConnected_{false};
    Private std::atomic<bool> wifiConnected_{false};
    Private std::atomic<ULong> networkConnectionId_{0};

    Public Bool IsNetworkConnected() const override {
        return networkConnected_.load(std::memory_order_relaxed);
    }

    Public Bool IsWifiConnected() const override {
        return wifiConnected_.load(std::memory_order_relaxed);
    }

    Public Void SetNetworkConnected(Bool connected) override {
        networkConnected_.store(static_cast<bool>(connected), std::memory_order_relaxed);
    }

    Public Void SetWifiConnected(Bool connected) override {
        wifiConnected_.store(static_cast<bool>(connected), std::memory_order_relaxed);
    }

    Public ULong GetNetworkConnectionId() const override {
        return networkConnectionId_.load(std::memory_order_relaxed);
    }

    Public Void SetNetworkConnectionId(ULong connectionId) override {
        networkConnectionId_.store(connectionId, std::memory_order_relaxed);
    }
};

#endif // NETWORKCONNECTIONSTATUSSTORE_H
