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

#include "Arduino_LoRaWAN.h"

// count number of defined networks
#define ARDUINO_LORAWAN_NETWORK_COUNT   \
        defined(ARDUINO_LMIC_CFG_NETWORK_TTN) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_ACTILITY) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_HELIUM) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_MACHINEQ) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_SENET) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_SENRA) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_SWISSCOM) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK) + \
        defined(ARDUINO_LMIC_CFG_NETWORK_GENERIC)

#if ARDUINO_LORAWAN_NETWORK_COUNT == 0
// none defined, query legacy symbols
# ifdef ARDUINO_LORAWAN_NETWORK_TTN
#  define ARDUINO_LMIC_CFG_NETWORK_TTN 1
# elif defined(ARDUINO_LORAWAN_NETWORK_MACHINEQ)
#  define ARDUINO_LMIC_CFG_NETWORK_MACHINEQ 1
# else
   // no legacy and no BSP: use TTN.
#  define ARDUINO_LMIC_CFG_NETWORK_TTN 1
# endif
#endif

#if ARDUINO_LORAWAN_NETWORK_COUNT > 1
# error "Multiple ARDUINO_LMIC_CFG_NETWORK_* values defined"
#endif

// so we don't need to #ifdef, give all the ARDUINO_LMIC_CFG_ variables a value.
#if !defined(ARDUINO_LMIC_CFG_NETWORK_TTN)
# define ARDUINO_LMIC_CFG_NETWORK_TTN 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_ACTILITY)
# define ARDUINO_LMIC_CFG_NETWORK_ACTILITY 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_HELIUM)
# define ARDUINO_LMIC_CFG_NETWORK_HELIUM 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_MACHINEQ)
# define ARDUINO_LMIC_CFG_NETWORK_MACHINEQ 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_SENET)
# define ARDUINO_LMIC_CFG_NETWORK_SENET 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_SENRA)
# define ARDUINO_LMIC_CFG_NETWORK_SENRA 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_SWISSCOM)
# define ARDUINO_LMIC_CFG_NETWORK_SWISSCOM 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK)
# define ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK 0
#endif
#if !defined(ARDUINO_LMIC_CFG_NETWORK_GENERIC)
# define ARDUINO_LMIC_CFG_NETWORK_GENERIC 0
#endif

#if ARDUINO_LMIC_CFG_NETWORK_TTN
# include <Arduino_LoRaWAN_ttn.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_ttn;
#elif ARDUINO_LMIC_CFG_NETWORK_ACTILITY
# include <Arduino_LoRaWAN_Actility.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Actility;
#elif ARDUINO_LMIC_CFG_NETWORK_HELIUM
# include <Arduino_LoRaWAN_Helium.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Helium;
#elif ARDUINO_LMIC_CFG_NETWORK_MACHINEQ
# include <Arduino_LoRaWAN_machineQ.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_machineQ;
#elif ARDUINO_LMIC_CFG_NETWORK_SENET
# include <Arduino_LoRaWAN_Senet.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Senet;
#elif ARDUINO_LMIC_CFG_NETWORK_SENRA
# include <Arduino_LoRaWAN_Senra.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Senra;
#elif ARDUINO_LMIC_CFG_NETWORK_SWISSCOM
# include <Arduino_LoRaWAN_Swisscom.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Swisscom;
#elif ARDUINO_LMIC_CFG_NETWORK_CHIRPSTACK
# include <Arduino_LoRaWAN_ChirpStack.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_ChirpStack;
#elif ARDUINO_LMIC_CFG_NETWORK_GENERIC
# include <Arduino_LoRaWAN_Generic.h>
  using Arduino_LoRaWAN_network = Arduino_LoRaWAN_Generic;
#else
# error "Internal error defining ARDUINO_LMIC_CFG_NETWORK_*"
#endif

#endif /* _ARDUINO_LORAWAN_NETWORK_H_ */
