#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef ARDUINO
#include <Arduino.h>
#include <ILogger.h>

#include "../internal/01-network/01-interface/01-IWifiConnectionStatusStore.h"
#include "../endpoint/service/IWifiService.h"

#include "ISample.h"

/* @Component */
class Sample : public ISample {

    /* @Autowired */
    Private ILoggerPtr logger;

    /* @Autowired */
    Private IWifiServicePtr wifiService;
    /* @Autowired */
    Private IWifiConnectionStatusStorePtr statusStore;



public:
    void foo() override {
        Serial.println("hello");
    }
};
#endif

#endif // SAMPLE_H
