#ifndef INTERNETHEALTHCHECKERTHREAD_H
#define INTERNETHEALTHCHECKERTHREAD_H

#include <StandardDefines.h>
#include <IRunnable.h>
#include <osal/Core.h>

#include "../../03-internet/00-public/02-IInternetConnectionManager.h"

/** Interval between checks (ms). Run() is called every loop; we only run the check when this much time has passed. */
static constexpr ULong kInternetHealthCheckIntervalMs = 5000;

/**
 * Verifies internet connectivity periodically. Call Run() every loop from main thread; throttled so the check runs at most every kInternetHealthCheckIntervalMs.
 */
class InternetHealthCheckerThread : public IRunnable {
    /* @Autowired */
    Private IInternetConnectionManagerPtr internetConnectionManager;
    Private ULong lastRunMs_{0};

    Public Void Run() override {
        ULong now = OSAL_GetMillis();
        if (now - lastRunMs_ >= kInternetHealthCheckIntervalMs) {
            internetConnectionManager->VerifyInternetConnectivity();
            lastRunMs_ = now;
        }
    }
};

#endif // INTERNETHEALTHCHECKERTHREAD_H
