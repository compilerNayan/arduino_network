#ifndef WIFIHEALTHCHECKERTHREAD_H
#define WIFIHEALTHCHECKERTHREAD_H

#include <StandardDefines.h>
#include <IRunnable.h>
#include <OSAL_Core.h>

#include "../../02-wifi/00-public/02-IWifiConnectionManager.h"

/**
 * Periodically ensures network connectivity, then sleeps (e.g. for use in a thread loop).
 */
class WiFiHealthCheckerThread : public IRunnable {
    /* @Autowired */
    Private IWifiConnectionManagerPtr wifiConnectionManager;

    Public Void Run() override {
        wifiConnectionManager->EnsureNetworkConnectivity();
        OSAL_DelayMs(2000);
    }
};

#endif // WIFIHEALTHCHECKERTHREAD_H
