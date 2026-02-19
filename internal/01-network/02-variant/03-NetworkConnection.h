#ifdef ARDUINO
#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include "../01-interface/03-INetworkConnection.h"
#include "../01-interface/01-INetworkConnectionStatusStore.h"
#include <StandardDefines.h>
#include <Thread.h>
#include <ILogger.h>
#include "service/IWifiService.h"
#include "entity/WifiCredentials.h"
#include <WiFi.h>
#include <Arduino.h>

/* @Component */
class NetworkConnection : public INetworkConnection {
    /* @Autowired */
    Private IWifiServicePtr wifiService;
    /* @Autowired */
    Private INetworkConnectionStatusStorePtr statusStore;
    /* @Autowired */
    Private ILoggerPtr logger;

    Private StdString currentMode;
    Private ULong wifiConnectionId_ = 0;

    Private Void UpdateStore(Bool networkConnected, Bool wifiConnected, ULong connectionId, Bool hotspotConnected) {
        statusStore->SetNetworkConnected(networkConnected);
        statusStore->SetWifiConnected(wifiConnected);
        statusStore->SetHotspotConnected(hotspotConnected);
        statusStore->SetNetworkConnectionId(connectionId);
    }

    Private Bool TryConnectToWifi(const StdString& ssid, const StdString& password) {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Attempting to connect to WiFi - SSID: " + ssid));
        logger->Info(Tag::Untagged, StdString(password.empty() ? "[NetworkConnection] No password (open network)" : "[NetworkConnection] Using password"));
        WiFi.disconnect();
        Thread::Sleep(100);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.empty() ? nullptr : password.c_str());
        currentMode = "wifi";
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 5) {
            Thread::Sleep(500);
            attempts++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnectionId_ = (ULong)random(1, 2147483647);
            UpdateStore(true, true, wifiConnectionId_, false);
            logger->Info(Tag::Untagged, StdString("[NetworkConnection] WiFi connected successfully! IP Address: " + StdString(WiFi.localIP().toString().c_str())));
            return true;
        }
        logger->Error(Tag::Untagged, StdString("[NetworkConnection] WiFi connection failed or timeout for SSID: " + ssid));
        return false;
    }

    /** Step 1: Try last connected WiFi. Returns true if connected. */
    Private Bool TryLastConnectedWifi() {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Step 1: Checking for last connected WiFi..."));
        optional<WifiCredentials> lastWifi = wifiService->GetLastConnectedWifi();
        if (!lastWifi.has_value() || !lastWifi.value().ssid.has_value() || lastWifi.value().ssid.value().empty()) {
            logger->Info(Tag::Untagged, StdString("[NetworkConnection] No last connected WiFi found"));
            return false;
        }
        StdString ssid = lastWifi.value().ssid.value();
        StdString password = lastWifi.value().password.has_value() ? lastWifi.value().password.value() : "";
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Last connected WiFi found - SSID: " + ssid));
        if (!TryConnectToWifi(ssid, password)) {
            logger->Warning(Tag::Untagged, StdString("[NetworkConnection] Failed to connect to last connected WiFi, trying other credentials..."));
            return false;
        }
        wifiService->UpdateLastConnectedSsid(ssid);
        UpdateStore(true, true, wifiConnectionId_, false);
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Successfully connected to last connected WiFi"));
        return true;
    }

    /** Step 2: Try all stored credentials. Returns true if connected. */
    Private Bool TryAllStoredCredentials() {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Step 2: Getting all WiFi credentials from database..."));
        StdVector<WifiCredentials> allCredentials = wifiService->GetAllWifiCredentials();
        if (allCredentials.empty()) {
            logger->Warning(Tag::Untagged, StdString("[NetworkConnection] No WiFi credentials found in database"));
            return false;
        }
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Found " + std::to_string(allCredentials.size()) + " WiFi credential(s) in database"));
        for (size_t i = 0; i < allCredentials.size(); i++) {
            const WifiCredentials& cred = allCredentials[i];
            StdString ssid = cred.ssid.value();
            StdString password = cred.password.has_value() ? cred.password.value() : "";
            logger->Info(Tag::Untagged, StdString("[NetworkConnection] Trying credential " + std::to_string(i + 1) + " of " + std::to_string(allCredentials.size()) + " - SSID: " + ssid));
            if (TryConnectToWifi(ssid, password)) {
                wifiService->UpdateLastConnectedSsid(ssid);
                UpdateStore(true, true, wifiConnectionId_, false);
                logger->Info(Tag::Untagged, StdString("[NetworkConnection] Successfully connected to WiFi: " + ssid));
                logger->Info(Tag::Untagged, StdString("[NetworkConnection] Updated last connected WiFi"));
                return true;
            }
        }
        logger->Warning(Tag::Untagged, StdString("[NetworkConnection] All WiFi credentials failed to connect"));
        return false;
    }

    /** Step 3: Start hotspot when no WiFi is available. */
    Private Void StartHotspotFallback() {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Step 3: Starting hotspot (no WiFi connections available or all failed)"));
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Hotspot SSID: SmartBoard (open, no password)"));
        WiFi.disconnect();
        Thread::Sleep(100);
        WiFi.mode(WIFI_AP);
        Bool apStarted = WiFi.softAP("SmartBoard", nullptr);
        if (apStarted) {
            currentMode = "hotspot";
            wifiConnectionId_ = (ULong)random(1, 2147483647);
            UpdateStore(true, false, wifiConnectionId_, true);
            logger->Info(Tag::Untagged, StdString("[NetworkConnection] Hotspot started successfully! AP IP Address: " + StdString(WiFi.softAPIP().toString().c_str())));
        } else {
            logger->Error(Tag::Untagged, StdString("[NetworkConnection] Failed to start hotspot"));
        }
    }

    Public Virtual ~NetworkConnection() = default;

    Public Virtual Void ConnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] ConnectNetwork() called"));
        if (TryLastConnectedWifi()) return;
        if (TryAllStoredCredentials()) return;
        StartHotspotFallback();
    }

    Public Virtual Void DisconnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] DisconnectNetwork() called"));
        WiFiMode_t mode = WiFi.getMode();
        if (mode == WIFI_AP || mode == WIFI_AP_STA) {
            logger->Info(Tag::Untagged, StdString("[NetworkConnection] Stopping hotspot"));
            WiFi.softAPdisconnect(true);
        }
        if (mode == WIFI_STA || mode == WIFI_AP_STA) {
            if (WiFi.status() == WL_CONNECTED) {
                logger->Info(Tag::Untagged, StdString("[NetworkConnection] Disconnecting WiFi - Previous IP: " + StdString(WiFi.localIP().toString().c_str())));
            }
            WiFi.disconnect();
        }
        UpdateStore(false, false, 0, false);
        currentMode = "";
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Network disconnected"));
    }

    Public Virtual Bool IsNetworkConnected() override {
        if (WiFi.status() == WL_CONNECTED) return true;
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) return true;
        return false;
    }

    Public Virtual Bool IsWifiConnected() override {
        return (WiFi.status() == WL_CONNECTED);
    }

    Public Virtual Bool IsHotspotConnected() override {
        return (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA);
    }

    Public Virtual ULong GetWifiConnectionId() override {
        return wifiConnectionId_;
    }

    Public Virtual ULong GetNetworkConnectionId() override {
        return wifiConnectionId_;
    }

    Public Virtual Bool EnsureNetworkConnectivity() override {
        return false;  // empty for now
    }

    Public Virtual Void RestartNetwork() override {
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] RestartNetwork() called"));
        DisconnectNetwork();
        Thread::Sleep(1000);
        logger->Info(Tag::Untagged, StdString("[NetworkConnection] Reconnecting..."));
        ConnectNetwork();
    }
};

#endif // NETWORKCONNECTION_H
#endif // ARDUINO
