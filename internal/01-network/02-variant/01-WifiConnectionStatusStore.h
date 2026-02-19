#ifndef WIFICONNECTIONSTATUSSTORE_H
#define WIFICONNECTIONSTATUSSTORE_H

#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include <StandardDefines.h>
#include <mutex>

/**
 * Thread-safe implementation: all state is updated under a mutex so reads/writes are consistent.
 */
/* @Component */
class WifiConnectionStatusStore : public IWifiConnectionStatusStore {
    Private mutable std::mutex mutex_;
    Private Bool networkConnected_{false};
    Private Bool wifiConnected_{false};
    Private Bool hotspotConnected_{false};
    Private ULong wifiConnectionId_{0};
    Private ULong hotspotConnectionId_{0};
    Private ULong networkConnectionId_{0};

    Public Bool IsNetworkConnected() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return networkConnected_;
    }

    Public Bool IsWifiConnected() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return wifiConnected_;
    }

    Public Bool IsHotspotConnected() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return hotspotConnected_;
    }

    Public ULong GetWifiConnectionId() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return wifiConnectionId_;
    }

    Public ULong GetHotspotConnectionId() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return hotspotConnectionId_;
    }

    Public ULong GetNetworkConnectionId() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return networkConnectionId_;
    }

    Public Void SetState(Bool networkConnected, Bool wifiConnected, Bool hotspotConnected,
                        ULong wifiConnectionId, ULong hotspotConnectionId, ULong networkConnectionId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        networkConnected_ = static_cast<bool>(networkConnected);
        wifiConnected_ = static_cast<bool>(wifiConnected);
        hotspotConnected_ = static_cast<bool>(hotspotConnected);
        wifiConnectionId_ = wifiConnectionId;
        hotspotConnectionId_ = hotspotConnectionId;
        networkConnectionId_ = networkConnectionId;
    }
};

#endif // WIFICONNECTIONSTATUSSTORE_H
