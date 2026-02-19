#ifndef WIFICONNECTIONSTATUSSTORE_H
#define WIFICONNECTIONSTATUSSTORE_H

#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include <StandardDefines.h>
#include <atomic>

/**
 * Thread-safe implementation using atomics for network, WiFi and hotspot status.
 */
class WifiConnectionStatusStore : public IWifiConnectionStatusStore {
    Private std::atomic<bool> networkConnected_{false};
    Private std::atomic<bool> wifiConnected_{false};
    Private std::atomic<bool> hotspotConnected_{false};
    Private std::atomic<ULong> wifiConnectionId_{0};
    Private std::atomic<ULong> hotspotConnectionId_{0};
    Private std::atomic<ULong> networkConnectionId_{0};

    Public Bool IsNetworkConnected() const override {
        return networkConnected_.load(std::memory_order_relaxed);
    }

    Public Bool IsWifiConnected() const override {
        return wifiConnected_.load(std::memory_order_relaxed);
    }

    Public Bool IsHotspotConnected() const override {
        return hotspotConnected_.load(std::memory_order_relaxed);
    }

    Public ULong GetWifiConnectionId() const override {
        return wifiConnectionId_.load(std::memory_order_relaxed);
    }

    Public ULong GetHotspotConnectionId() const override {
        return hotspotConnectionId_.load(std::memory_order_relaxed);
    }

    Public ULong GetNetworkConnectionId() const override {
        return networkConnectionId_.load(std::memory_order_relaxed);
    }

    Public Void SetNetworkConnected(Bool connected) override {
        networkConnected_.store(static_cast<bool>(connected), std::memory_order_relaxed);
    }

    Public Void SetWifiConnected(Bool connected) override {
        wifiConnected_.store(static_cast<bool>(connected), std::memory_order_relaxed);
    }

    Public Void SetHotspotConnected(Bool connected) override {
        hotspotConnected_.store(static_cast<bool>(connected), std::memory_order_relaxed);
    }

    Public Void SetWifiConnectionId(ULong connectionId) override {
        wifiConnectionId_.store(connectionId, std::memory_order_relaxed);
    }

    Public Void SetHotspotConnectionId(ULong connectionId) override {
        hotspotConnectionId_.store(connectionId, std::memory_order_relaxed);
    }

    Public Void SetNetworkConnectionId(ULong connectionId) override {
        networkConnectionId_.store(connectionId, std::memory_order_relaxed);
    }
};

#endif // WIFICONNECTIONSTATUSSTORE_H
