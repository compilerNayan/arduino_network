#ifndef WIFIHEALTHCHECKERTHREAD_H
#define WIFIHEALTHCHECKERTHREAD_H

#include <StandardDefines.h>
#include <IRunnable.h>
#include <osal/Core.h>

#include "../../02-wifi/00-public/02-IWifiConnectionManager.h"

/** Interval between checks (ms). Run() is called every loop; we only run the check when this much time has passed. */
static constexpr ULong kWifiHealthCheckIntervalMs = 2000;

/**
 * Ensures network connectivity periodically. Call Run() every loop from main thread; throttled so the check runs at most every kWifiHealthCheckIntervalMs.
 */
class WiFiHealthCheckerThread : public IRunnable {
    /* @Autowired */
    Private IWifiConnectionManagerPtr wifiConnectionManager;
    Private ULong lastRunMs_{0};

    Public Void Run() override {
        ULong now = OSAL_GetMillis();
        if (now - lastRunMs_ >= kWifiHealthCheckIntervalMs) {
            wifiConnectionManager->EnsureNetworkConnectivity();
            lastRunMs_ = now;
        }
    }
};

#endif // WIFIHEALTHCHECKERTHREAD_H
