/*

Module:  GetRegionString.cpp

Function:
	Arduino_LoRaWAN::GetRegionString()

Copyright notice:
	See accompanying LICENSE file.
 
Author:
	Terry Moore, MCCI Corporation	June 2018

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino_LoRaWAN_lmic.h>
#include <mcciadk_baselib.h>

const char *
Arduino_LoRaWAN::GetRegionString(char *pBuf, size_t nBuf) const
        {
        if (pBuf == nullptr || nBuf == 0)
		{
		static const char sNull[] = "<<null>>";
                return sNull;
		}

        const char *pString;

        switch (CFG_region)
                {
        case LMIC_REGION_eu868: pString = "eu868"; break;
        case LMIC_REGION_us915: pString = "us915"; break;
        case LMIC_REGION_cn783: pString = "cn783"; break;
        case LMIC_REGION_eu433: pString = "eu433"; break;
        case LMIC_REGION_au915: pString = "au915"; break;
        case LMIC_REGION_cn490: pString = "cn490"; break;
        case LMIC_REGION_as923:
                if (LMIC_COUNTRY_CODE == LMIC_COUNTRY_CODE_JP)
                        pString = "as923jp";
                else
                        pString = "as923";
                break;
        case LMIC_REGION_kr920: pString = "kr920"; break;
        case LMIC_REGION_in866: pString = "in866"; break;
        default:                pString = "<<unknown>>"; break;
                }

        (void) McciAdkLib_SafeCopyString(pBuf, nBuf, 0, pString);
        return pBuf;
        }
