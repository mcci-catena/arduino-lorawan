/*

Module:  GetCountryCode.cpp

Function:
	Arduino_LoRaWAN::GetCountryCode()

Copyright notice:
	See accompanying LICENSE file.
 
Author:
	Terry Moore, MCCI Corporation	June 2018

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino_LoRaWAN_lmic.h>

Arduino_LoRaWAN::CountryCode
Arduino_LoRaWAN::GetCountryCode(void) const
        {
        return Arduino_LoRaWAN::CountryCode(LMIC_COUNTRY_CODE);
        }
