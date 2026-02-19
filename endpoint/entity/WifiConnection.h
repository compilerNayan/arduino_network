#ifndef WIFI_CONNECTION_ENTITY_H
#define WIFI_CONNECTION_ENTITY_H

#include <StandardDefines.h>

/* @Entity */
class WifiConnection {

    /* @Id */
    /* @NotNull */
    Public optional<int> id;

    Public optional<StdString> lastConnectedSsid;
};

#endif

