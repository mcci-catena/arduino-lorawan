/*

Module:  simple.ino

Function:
    Example app matching the documentation in the project
    README.md, showing how to use built-in LMIC configuration.

Copyright notice and License:
    See LICENSE file accompanying this project.

Author:
    Terry Moore, MCCI Corporation	November 2018

Notes:
    This app is not complete -- it only presents skeleton
    code for the methods you must provide in order to
    use this library. However, it compiles!

*/

#include <Arduino_LoRaWAN_network.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_network {
public:
    cMyLoRaWAN() {};

protected:
    // you'll need to provide implementation for this.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    // if you have persistent storage, you can provide implementations for these:
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    virtual void NetSaveSessionState(const SessionState &State) override;
    virtual bool NetGetSessionState(SessionState &State) override;
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN {};

void setup() {
    // simply call begin() w/o parameters, and the LMIC's built-in
    // configuration for this board will be used.
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
    OtaaProvisioningInfo *pInfo
    ) {
    return false;
}

void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    // save Info somewhere.
}


void
cMyLoRaWAN::NetSaveSessionState(const SessionState &State) {
    // save State somwwhere. Note that it's often the same;
    // often only the frame counters change.
}

bool
cMyLoRaWAN::NetGetSessionState(SessionState &State) {
    // either fetch SessionState from somewhere and return true or...
    return false;
}
