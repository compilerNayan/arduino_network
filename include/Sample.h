#ifndef SAMPLE_H
#define SAMPLE_H

#ifdef ARDUINO
#include <Arduino.h>
#include "ISample.h"

class Sample : public ISample {
public:
    void foo() override {
        Serial.println("hello");
    }
};
#endif

#endif // SAMPLE_H
