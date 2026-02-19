#ifndef NETWORKCONNECTIONSTATUSPROVIDER_H
#define NETWORKCONNECTIONSTATUSPROVIDER_H

#include "../01-interface/02-INetworkConnectionStatusProvider.h"
#include "../01-interface/01-INetworkConnectionStatusStore.h"
#include <StandardDefines.h>

/**
 * Implementation that reads network/WiFi status from INetworkConnectionStatusStore.
 */
/* @Component */
class NetworkConnectionStatusProvider : public INetworkConnectionStatusProvider {
    /* @Autowired */
    Private INetworkConnectionStatusStorePtr store;

    Public Bool IsNetworkConnected() const override {
        return store->IsNetworkConnected();
    }

    Public Bool IsWifiConnected() const override {
        return store->IsWifiConnected();
    }

    Public ULong GetNetworkConnectionId() const override {
        return store->GetNetworkConnectionId();
    }
};

#endif // NETWORKCONNECTIONSTATUSPROVIDER_H
