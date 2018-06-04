/*

Module:  GetRegion.cpp

Function:
	Arduino_LoRaWAN::GetRegion()

Copyright notice:
	See accompanying LICENSE file.
 
Author:
	Terry Moore, MCCI Corporation	June 2018

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino_LoRaWAN_lmic.h>

Arduino_LoRaWAN::Region
Arduino_LoRaWAN::GetRegion(void) const
        {
        return Arduino_LoRaWAN::Region(CFG_region);
        }
