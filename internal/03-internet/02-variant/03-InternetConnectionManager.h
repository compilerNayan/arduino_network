#ifdef ARDUINO
#ifndef INTERNETCONNECTIONMANAGER_H
#define INTERNETCONNECTIONMANAGER_H

#include "../01-interface/03-IInternetConnectionManager.h"
#include "../01-interface/01-IInternetConnectionStatusStore.h"
#include "../../02-wifi/01-interface/03-IWifiConnectionManager.h"
#include <StandardDefines.h>
#include <ILogger.h>
#include <OSAL_Core.h>
#include <OSAL_WiFiClient.h>

namespace {
struct InternetCheckPair {
    const char* ip1;
    const char* ip2;
};
const InternetCheckPair kInternetCheckPairs[] = {
    { "208.67.222.123", "1.0.0.1" },       // FamilyShield, Cloudflare
    { "8.8.4.4", "208.67.220.220" },       // Google, OpenDNS
    { "156.154.70.1", "64.6.64.6" },       // Neustar, Verisign
    { "195.46.39.39", "199.85.127.10" },   // SafeDNS, Norton
    { "76.76.2.0", "94.140.15.15" },       // Control D, AdGuard
    { "208.67.220.123", "156.154.71.1" },   // FamilyShield, Neustar
    { "1.1.1.1", "208.67.222.222" },       // Cloudflare, OpenDNS
    { "8.8.8.8", "64.6.65.6" },            // Google, Verisign
    { "195.46.39.40", "76.76.10.0" },      // SafeDNS, Control D
    { "199.85.126.10", "76.76.19.19" },    // Norton, Alternate
};
const size_t kNumInternetCheckPairs = sizeof(kInternetCheckPairs) / sizeof(kInternetCheckPairs[0]);
}

/* @Component */
class InternetConnectionManager : public IInternetConnectionManager {
    /* @Autowired */
    Private IWifiConnectionManagerPtr wifiConnectionManager;
    /* @Autowired */
    Private IInternetConnectionStatusStorePtr internetStatusStore;
    /* @Autowired */
    Private ILoggerPtr logger;

    Private size_t nextInternetCheckPairIndex_ = 0;

    /** Check internet reachability via DNS server TCP connect (port 53). Updates store with result. */
    Private Bool HasInternet() {
        const InternetCheckPair& pair = kInternetCheckPairs[nextInternetCheckPairIndex_];
        nextInternetCheckPairIndex_ = (nextInternetCheckPairIndex_ + 1) % kNumInternetCheckPairs;

        OSAL_WiFiClient client;
        if (client.Connect(pair.ip1, 53, 2000)) {
            client.Stop();
            internetStatusStore->SetState(true, OSAL_GenerateConnectionId());
            return true;
        }
        logger->Warning(Tag::Untagged, StdString("[InternetConnection] Internet check failed for " + StdString(pair.ip1)));
        if (client.Connect(pair.ip2, 53, 2000)) {
            client.Stop();
            internetStatusStore->SetState(true, OSAL_GenerateConnectionId());
            return true;
        }
        logger->Warning(Tag::Untagged, StdString("[InternetConnection] Internet check failed for " + StdString(pair.ip2)));
        internetStatusStore->SetState(false, 0);
        return false;
    }

    Public Virtual ~InternetConnectionManager() = default;

    Public Virtual Void ConnectNetwork() override {
        wifiConnectionManager->ConnectNetwork();
    }

    Public Virtual Void DisconnectNetwork() override {
        wifiConnectionManager->DisconnectNetwork();
        internetStatusStore->SetState(false, 0);
    }

    Public Virtual Bool IsNetworkConnected() override {
        return wifiConnectionManager->IsNetworkConnected();
    }

    Public Virtual Bool IsWifiConnected() override {
        return wifiConnectionManager->IsWifiConnected();
    }

    Public Virtual Bool IsHotspotConnected() override {
        return wifiConnectionManager->IsHotspotConnected();
    }

    Public Virtual Bool IsInternetConnected() override {
        if (!wifiConnectionManager->IsNetworkConnected()) return false;
        return HasInternet();
    }

    Public Virtual ULong GetWifiConnectionId() override {
        return wifiConnectionManager->GetWifiConnectionId();
    }

    Public Virtual ULong GetNetworkConnectionId() override {
        return wifiConnectionManager->GetNetworkConnectionId();
    }

    Public Virtual Bool EnsureNetworkConnectivity() override {
        return wifiConnectionManager->EnsureNetworkConnectivity();
    }

    Public Virtual Bool VerifyInternetConnectivity() override {
        return HasInternet();
    }

    Public Virtual Void RestartNetwork() override {
        wifiConnectionManager->RestartNetwork();
        internetStatusStore->SetState(false, 0);
    }
};

#endif // INTERNETCONNECTIONMANAGER_H
#endif // ARDUINO
