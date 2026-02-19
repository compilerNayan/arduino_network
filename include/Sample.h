#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef ARDUINO
#include <Arduino.h>
#include "ISample.h"


/* @Component */
class Sample : public ISample {

    /* @Autowired */
    Private ILoggerPtr logger;


public:
    void foo() override {
        Serial.println("hello");
    }
};
#endif

#endif // SAMPLE_H
