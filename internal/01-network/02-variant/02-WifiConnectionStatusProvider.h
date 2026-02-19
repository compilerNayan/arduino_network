#ifndef WIFICONNECTIONSTATUSPROVIDER_H
#define WIFICONNECTIONSTATUSPROVIDER_H

#include "../01-interface/02-IWifiConnectionStatusProvider.h"
#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include <StandardDefines.h>

/**
 * Implementation that reads network/WiFi status from IWifiConnectionStatusStore.
 */
/* @Component */
class WifiConnectionStatusProvider : public IWifiConnectionStatusProvider {
    /* @Autowired */
    Private IWifiConnectionStatusStorePtr store;

    Public Bool IsNetworkConnected() const override {
        return store->IsNetworkConnected();
    }

    Public Bool IsWifiConnected() const override {
        return store->IsWifiConnected();
    }

    Public Bool IsHotspotConnected() const override {
        return store->IsHotspotConnected();
    }

    Public ULong GetWifiConnectionId() const override {
        return store->GetWifiConnectionId();
    }

    Public ULong GetHotspotConnectionId() const override {
        return store->GetHotspotConnectionId();
    }
};

#endif // WIFICONNECTIONSTATUSPROVIDER_H
