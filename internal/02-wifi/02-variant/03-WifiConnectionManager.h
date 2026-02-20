#ifdef ARDUINO
#ifndef WIFICONNECTIONMANAGER_H
#define WIFICONNECTIONMANAGER_H

#include "../01-interface/03-IWifiConnectionManager.h"
#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include <StandardDefines.h>
#include <cpp_utils/StringUtils.h>
#include <ILogger.h>
#include "../../../endpoint/service/IWifiService.h"
#include "../01-interface/01-IWifiConnectionStatusStore.h"
#include "../../../endpoint/entity/WifiCredentials.h"
#include <OSAL_Core.h>
#include <OSAL_WiFi.h>

/* @Component */
class WifiConnectionManager : public IWifiConnectionManager {
    /* @Autowired */
    Private IWifiServicePtr wifiService;
    /* @Autowired */
    Private IWifiConnectionStatusStorePtr statusStore;
    /* @Autowired */
    Private ILoggerPtr logger;

    Private StdString currentMode;
    Private ULong wifiConnectionId_ = 0;
    Private ULong hotspotConnectionId_ = 0;

    Private Void UpdateStore(Bool wifiConnected, Bool hotspotConnected, ULong wifiConnectionId, ULong hotspotConnectionId) {
        statusStore->SetState(wifiConnected, hotspotConnected, wifiConnectionId, hotspotConnectionId);
    }

    Private Bool TryConnectToWifi(const StdString& ssid, const StdString& password) {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Attempting to connect to WiFi - SSID: " + ssid));
        logger->Info(Tag::Untagged, StdString(password.empty() ? "[WifiConnection] No password (open network)" : "[WifiConnection] Using password"));
        OSAL_WifiDisconnect();
        OSAL_DelayMs(100);
        OSAL_WifiSetModeSta();
        OSAL_WifiBegin(ssid, password);
        currentMode = "wifi";
        int attempts = 0;
        while (!OSAL_WifiIsConnected() && attempts < 5) {
            OSAL_DelayMs(500);
            attempts++;
        }
        if (OSAL_WifiIsConnected()) {
            wifiConnectionId_ = OSAL_GenerateConnectionId();
            hotspotConnectionId_ = 0;
            UpdateStore(true, false, wifiConnectionId_, 0);
            logger->Info(Tag::Untagged, StdString("[WifiConnection] WiFi connected successfully! IP Address: " + OSAL_WifiGetLocalIpString()));
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
        UpdateStore(true, false, wifiConnectionId_, 0);
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
        logger->Info(Tag::Untagged, cpp_utils::Format("[WifiConnection] Found {} WiFi credential(s) in database", allCredentials.size()));
        for (size_t i = 0; i < allCredentials.size(); i++) {
            const WifiCredentials& cred = allCredentials[i];
            StdString ssid = cred.ssid.value();
            StdString password = cred.password.has_value() ? cred.password.value() : "";
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Trying credential " + cpp_utils::ToString(i + 1) + " of " + cpp_utils::ToString(allCredentials.size()) + " - SSID: " + ssid));
            if (TryConnectToWifi(ssid, password)) {
                wifiService->UpdateLastConnectedSsid(ssid);
                UpdateStore(true, false, wifiConnectionId_, 0);
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
        OSAL_WifiDisconnect();
        OSAL_DelayMs(100);
        OSAL_WifiSetModeAp();
        Bool apStarted = OSAL_WifiStartSoftAp("SmartBoard", nullptr);
        if (apStarted) {
            currentMode = "hotspot";
            hotspotConnectionId_ = OSAL_GenerateConnectionId();
            wifiConnectionId_ = 0;
            UpdateStore(false, true, 0, hotspotConnectionId_);
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Hotspot started successfully! AP IP Address: " + OSAL_WifiGetSoftApIpString()));
        } else {
            logger->Error(Tag::Untagged, StdString("[WifiConnection] Failed to start hotspot"));
        }
    }

    Public Virtual ~WifiConnectionManager() = default;

    Public Virtual Void ConnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] ConnectNetwork() called"));
        if (TryLastConnectedWifi()) return;
        if (TryAllStoredCredentials()) return;
        StartHotspotFallback();
    }

    Public Virtual Void DisconnectNetwork() override {
        logger->Info(Tag::Untagged, StdString("[WifiConnection] DisconnectNetwork() called"));
        WifiMode mode = OSAL_GetWifiMode();
        if (mode == WifiMode::Ap || mode == WifiMode::ApSta) {
            logger->Info(Tag::Untagged, StdString("[WifiConnection] Stopping hotspot"));
            OSAL_WifiSoftApDisconnect();
        }
        if (mode == WifiMode::Sta || mode == WifiMode::ApSta) {
            if (OSAL_WifiIsConnected()) {
                logger->Info(Tag::Untagged, StdString("[WifiConnection] Disconnecting WiFi - Previous IP: " + OSAL_WifiGetLocalIpString()));
            }
            OSAL_WifiDisconnect();
        }
        UpdateStore(false, false, 0, 0);
        currentMode = "";
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Network disconnected"));
    }

    Public Virtual Bool IsNetworkConnected() override {
        if (OSAL_WifiIsConnected()) return true;
        WifiMode mode = OSAL_GetWifiMode();
        if (mode == WifiMode::Ap || mode == WifiMode::ApSta) return true;
        return false;
    }

    Public Virtual Bool IsWifiConnected() override {
        return OSAL_WifiIsConnected();
    }

    Public Virtual Bool IsHotspotConnected() override {
        WifiMode mode = OSAL_GetWifiMode();
        return (mode == WifiMode::Ap || mode == WifiMode::ApSta);
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
        OSAL_DelayMs(1000);
        logger->Info(Tag::Untagged, StdString("[WifiConnection] Reconnecting..."));
        ConnectNetwork();
    }
};

#endif // WIFICONNECTIONMANAGER_H
#endif // ARDUINO
