#ifdef ARDUINO
#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include "../01-interface/03-IWifiConnection.h"
#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include <StandardDefines.h>
#include <Thread.h>
#include <ILogger.h>
#include "service/IWifiService.h"
#include "../../../endpoint/service/WifiService.h"
#include "01-WifiConnectionStatusStore.h"
#include "entity/WifiCredentials.h"
#include <WiFi.h>
#include <Arduino.h>

/* @Component */
class WifiConnection : public IWifiConnection {
    /* @Autowired */
    Private IWifiServicePtr wifiService;
    /* @Autowired */
    Private IWifiConnectionStatusStorePtr statusStore;
    /* @Autowired */
    Private ILoggerPtr logger;

    Private StdString currentMode;
    Private ULong wifiConnectionId_ = 0;
    Private ULong hotspotConnectionId_ = 0;

    Private Void UpdateStore(Bool networkConnected, Bool wifiConnected, ULong wifiConnectionId, Bool hotspotConnected, ULong hotspotConnectionId, ULong networkConnectionId) {
        statusStore->SetState(networkConnected, wifiConnected, hotspotConnected, wifiConnectionId, hotspotConnectionId, networkConnectionId);
    }

    Private Bool TryConnectToWifi(const StdString& ssid, const StdString& password) {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Attempting to connect to WiFi - SSID: " + ssid));
        logger->Info(Tag::Untagged, StdString(password.empty() ? "[WifiConnection] No password (open network)" : "[WifiConnection] Using password"));
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
            hotspotConnectionId_ = 0;
            UpdateStore(true, true, wifiConnectionId_, false, 0, wifiConnectionId_);
            logger->Info(Tag::Untagged, StdString("[WifiConnection] WiFi connected successfully! IP Address: " + StdString(WiFi.localIP().toString().c_str())));
            return true;
        }
        logger->Error(Tag::Untagged, StdString("[WifiConnection] WiFi connection failed or timeout for SSID: " + ssid));
        return false;
    }

    /** Step 1: Try last connected WiFi. Returns true if connected. */
    Private Bool TryLastConnectedWifi() {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Step 1: Checking for last connected WiFi..."));
        optional<WifiCredentials> lastWifi = wifiService->GetLastConnectedWifi();
        if (!lastWifi.has_value() || !lastWifi.value().ssid.has_value() || lastWifi.value().ssid.value().empty()) {
            logger->Info(Tag::Untagged, StdString("[WifiConnection] No last connected WiFi found"));
            return false;
        }
        StdString ssid = lastWifi.value().ssid.value();
        StdString password = lastWifi.value().password.has_value() ? lastWifi.value().password.value() : "";
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Last connected WiFi found - SSID: " + ssid));
        if (!TryConnectToWifi(ssid, password)) {
            logger->Warning(Tag::Untagged, StdString("[WifiConnection] Failed to connect to last connected WiFi, trying other credentials..."));
            return false;
        }
        wifiService->UpdateLastConnectedSsid(ssid);
        UpdateStore(true, true, wifiConnectionId_, false, 0, wifiConnectionId_);
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Successfully connected to last connected WiFi"));
        return true;
    }

    /** Step 2: Try all stored credentials. Returns true if connected. */
    Private Bool TryAllStoredCredentials() {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Step 2: Getting all WiFi credentials from database..."));
        StdVector<WifiCredentials> allCredentials = wifiService->GetAllWifiCredentials();
        if (allCredentials.empty()) {
            logger->Warning(Tag::Untagged, StdString("[WifiConnection] No WiFi credentials found in database"));
            return false;
        }
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Found " + std::to_string(allCredentials.size()) + " WiFi credential(s) in database"));
        for (size_t i = 0; i < allCredentials.size(); i++) {
            const WifiCredentials& cred = allCredentials[i];
            StdString ssid = cred.ssid.value();
            StdString password = cred.password.has_value() ? cred.password.value() : "";
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Trying credential " + std::to_string(i + 1) + " of " + std::to_string(allCredentials.size()) + " - SSID: " + ssid));
            if (TryConnectToWifi(ssid, password)) {
                wifiService->UpdateLastConnectedSsid(ssid);
                UpdateStore(true, true, wifiConnectionId_, false, 0, wifiConnectionId_);
                logger->Info(Tag::Untagged, StdString("[WifiConnection] Successfully connected to WiFi: " + ssid));
                logger->Info(Tag::Untagged, StdString("[WifiConnection] Updated last connected WiFi"));
                return true;
            }
        }
        logger->Warning(Tag::Untagged, StdString("[WifiConnection] All WiFi credentials failed to connect"));
        return false;
    }

    /** Step 3: Start hotspot when no WiFi is available. */
    Private Void StartHotspotFallback() {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Step 3: Starting hotspot (no WiFi connections available or all failed)"));
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Hotspot SSID: SmartBoard (open, no password)"));
        WiFi.disconnect();
        Thread::Sleep(100);
        WiFi.mode(WIFI_AP);
        Bool apStarted = WiFi.softAP("SmartBoard", nullptr);
        if (apStarted) {
            currentMode = "hotspot";
            hotspotConnectionId_ = (ULong)random(1, 2147483647);
            wifiConnectionId_ = 0;
            UpdateStore(true, false, 0, true, hotspotConnectionId_, hotspotConnectionId_);
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Hotspot started successfully! AP IP Address: " + StdString(WiFi.softAPIP().toString().c_str())));
        } else {
            logger->Error(Tag::Untagged, StdString("[WifiConnection] Failed to start hotspot"));
        }
    }

    Public Virtual ~WifiConnection() = default;

    Public Virtual Void ConnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] ConnectNetwork() called"));
        if (TryLastConnectedWifi()) return;
        if (TryAllStoredCredentials()) return;
        StartHotspotFallback();
    }

    Public Virtual Void DisconnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] DisconnectNetwork() called"));
        WiFiMode_t mode = WiFi.getMode();
        if (mode == WIFI_AP || mode == WIFI_AP_STA) {
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Stopping hotspot"));
            WiFi.softAPdisconnect(true);
        }
        if (mode == WIFI_STA || mode == WIFI_AP_STA) {
            if (WiFi.status() == WL_CONNECTED) {
                logger->Info(Tag::Untagged, StdString("[WifiConnection] Disconnecting WiFi - Previous IP: " + StdString(WiFi.localIP().toString().c_str())));
            }
            WiFi.disconnect();
        }
        UpdateStore(false, false, 0, false, 0, 0);
        currentMode = "";
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Network disconnected"));
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
        return wifiConnectionId_ || hotspotConnectionId_;
    }

    Public Virtual Bool EnsureNetworkConnectivity() override {
        if (IsHotspotConnected()) {
            if (!wifiService->GetAllWifiCredentials().empty()) {
                RestartNetwork();
            }
            return IsNetworkConnected();
        }
        if (IsWifiConnected()) {
            return true;
        }
        RestartNetwork();
        return IsNetworkConnected();
    }

    Public Virtual Void RestartNetwork() override {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] RestartNetwork() called"));
        DisconnectNetwork();
        Thread::Sleep(1000);
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Reconnecting..."));
        ConnectNetwork();
    }
};

#endif // WIFICONNECTION_H
#endif // ARDUINO
