#ifndef ISAMPLE_H
#define ISAMPLE_H

#include <StandardDefines.h>

DefineStandardPointers(ISample)
class ISample {
public:
    virtual ~ISample() = default;
    virtual void foo() = 0;
};

#endif // ISAMPLE_H
