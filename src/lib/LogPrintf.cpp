/*

Module:  LogPrintf.cpp

Function:
	Arduino_LoRaWAN::LogPrintf()

Copyright notice:
	This file copyright (C) 2016, 2019 by

		MCCI Corporation
		3520 Krums Corners Road
		Ithaca, NY  14850

	An unpublished work.  All rights reserved.
	
	See accompanying LICENSE file.
 
Author:
	Terry Moore, MCCI Corporation	November 2016

*/

#include <Arduino_LoRaWAN.h>

#include <Arduino.h>
#include <stdio.h>
#include <stdarg.h>
#include <type_traits>

// Some Adafruit BSPs implement Serial.dtr(), which immediately returns true if
// the USB port is connected and enabled. Others don't.  If Serial.dtr() is provided,
// we want to use it; if not, we want to fall back to the boolean approach (!! Serial).
//
// To do this, we use the approach given by https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature;
// a pair of templates are given, only one of which will actualy be implemented, based
// on whether dtr() is a valid method.
//
// This first template will tell us if a given type has a dtr() method.
// it works because "substitution failure is not an error".
template<typename T>
struct HasDtrMethod
	{
	template<typename U, size_t (U::*)() const> struct SFINAE {};
	template<typename U> static char Test(SFINAE<U, &U::dtr>*);
	template<typename U> static int Test(...);
	static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
	};

// this template will be instantiated if we discover that TMap has
// a dtr() method.
template<typename TMap>
static bool CheckDtrConcrete(TMap& m, std::true_type)
	{
	// We may call dtr() on m here.
	return m.dtr();
	}

// this template will be instantiated if we discover that TMap lacks
// a dtr() method.
template<typename TMap>
static bool CheckDtrConcrete(TMap& m, std::false_type)
	{
	// if no dtr() method, just do the boolean trick.
	return !!m;
	}

// this template expands to the appropriate version of the specific
//  CheckDtr function.
template<typename TMap>
static bool CheckDtr(TMap& m)
	{
	return CheckDtrConcrete(
		m,
		std::integral_constant<bool, HasDtrMethod<TMap>::Has>()
		);
	}

void
Arduino_LoRaWAN::LogPrintf(
	const char *fmt, 
	...
	)
	{
	if (! CheckDtr(Serial))
		return;

	char buf[128];
	va_list ap;

	va_start(ap, fmt);
	(void) vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	va_end(ap);

	// in case we overflowed:
	buf[sizeof(buf) - 1] = '\0';
	if (CheckDtr(Serial)) Serial.print(buf);
	}
