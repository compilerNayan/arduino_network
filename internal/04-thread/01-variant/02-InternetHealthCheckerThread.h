#ifndef INTERNETHEALTHCHECKERTHREAD_H
#define INTERNETHEALTHCHECKERTHREAD_H

#include <StandardDefines.h>
#include <IRunnable.h>
#include <OSAL_Core.h>

#include "../../03-internet/00-public/02-IInternetConnectionManager.h"

/**
 * Periodically verifies internet connectivity, then sleeps (e.g. for use in a thread loop).
 */
class InternetHealthCheckerThread : public IInternetHealthCheckerThread {
    /* @Autowired */
    Private IInternetConnectionManagerPtr internetConnectionManager;

    Public Void Run() override {
        internetConnectionManager->VerifyInternetConnectivity();
        OSAL_DelayMs(5000);
    }
};

#endif // INTERNETHEALTHCHECKERTHREAD_H
