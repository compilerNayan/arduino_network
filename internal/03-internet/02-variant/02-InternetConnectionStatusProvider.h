#ifndef INTERNETCONNECTIONSTATUSPROVIDER_H
#define INTERNETCONNECTIONSTATUSPROVIDER_H

#include "../01-interface/02-IInternetConnectionStatusProvider.h"
#include "../01-interface/01-IInternetConnectionStatusStore.h"
#include "../../02-wifi/01-interface/02-IWifiConnectionStatusProvider.h"
#include <StandardDefines.h>

/**
 * Implementation that reads network/WiFi status from IWifiConnectionStatusProvider
 * and internet status from IInternetConnectionStatusStore.
 */
/* @Component */
class InternetConnectionStatusProvider : public IInternetConnectionStatusProvider {
    /* @Autowired */
    Private IWifiConnectionStatusProviderPtr wifiStatusProvider;
    /* @Autowired */
    Private IInternetConnectionStatusStorePtr store;

    Public Bool IsNetworkConnected() const override {
        return wifiStatusProvider->IsNetworkConnected();
    }

    Public Bool IsWifiConnected() const override {
        return wifiStatusProvider->IsWifiConnected();
    }

    Public Bool IsHotspotConnected() const override {
        return wifiStatusProvider->IsHotspotConnected();
    }

    Public Bool IsInternetConnected() const override {
        return store->IsInternetConnected();
    }

    Public ULong GetWifiConnectionId() const override {
        return wifiStatusProvider->GetWifiConnectionId();
    }

    Public ULong GetHotspotConnectionId() const override {
        return wifiStatusProvider->GetHotspotConnectionId();
    }

    Public ULong GetNetworkConnectionId() const override {
        return wifiStatusProvider->GetNetworkConnectionId();
    }

    Public ULong GetInternetConnectionId() const override {
        return store->GetInternetConnectionId();
    }
};

#endif // INTERNETCONNECTIONSTATUSPROVIDER_H
