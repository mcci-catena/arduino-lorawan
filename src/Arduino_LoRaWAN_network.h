/*

Module:  Arduino_LoRaWAN_network.h

Function:
        The upper-edge class for the network-specific LoRaWAN object.

Copyright notice:
        See LICENSE file accompanying this project.

Author:
        Terry Moore, MCCI Corporation   October 2019

*/

#ifndef _ARDUINO_LORAWAN_NETWORK_H_             /* prevent multiple includes */
#define _ARDUINO_LORAWAN_NETWORK_H_


//#define ARDUINO_LORAWAN_NETWORK_MACHINEQ 1

#ifdef ARDUINO_LORAWAN_NETWORK_TTN
# include <Arduino_LoRaWAN_ttn.h>
using Arduino_LoRaWAN_network = Arduino_LoRaWAN_ttn;
#elif defined(ARDUINO_LORAWAN_NETWORK_MACHINEQ)
# include <Arduino_LoRaWAN_machineQ.h>
using Arduino_LoRaWAN_network = Arduino_LoRaWAN_machineQ;
#else
// **** default **** : assume TTN
# include <Arduino_LoRaWAN_ttn.h>
using Arduino_LoRaWAN_network = Arduino_LoRaWAN_ttn;
#endif

#endif /* _ARDUINO_LORAWAN_NETWORK_H_ */
