#ifndef WIFICONNECTIONSTATUSPROVIDER_H
#define WIFICONNECTIONSTATUSPROVIDER_H

#include "../01-interface/02-IWifiConnectionStatusProvider.h"
#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include "../../01-network/00-public/01-INetworkStatusProvider.h"
#include <StandardDefines.h>

/**
 * Implementation that reads network/WiFi status from IWifiConnectionStatusStore.
 */
/* @Component */
class WifiConnectionStatusProvider : public IWifiConnectionStatusProvider {
    /* @Autowired */
    Private IWifiConnectionStatusStorePtr store;

    /* @Autowired */
    Private INetworkStatusProviderPtr networkStatusProvider;

    Public Bool IsNetworkConnected() const override {
        return networkStatusProvider->IsNetworkConnected();
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

    Public ULong GetNetworkConnectionId() const override {
        ULong w = store->GetWifiConnectionId();
        ULong h = store->GetHotspotConnectionId();
        return w ? w : h;
    }
};

#endif // WIFICONNECTIONSTATUSPROVIDER_H
