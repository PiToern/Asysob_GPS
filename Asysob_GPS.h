#ifndef _Asysob_GPS_h
#define _Asysob_GPS_h

// ************************************************************************
// Asysob_GPS - concentrating on gathering all data sent by the GPS
// device into a single buffer with pointers to the last relevant entries.
// Addresses performance issues that might arise when using SoftwareSerial
// as it happens to be in the Uno case.
//
// (c) 2013 Peter Sturm, AG SysSoft University of Trier, Germany
// BSD license, check license.txt for more information
// All text above must be included in any redistribution.
// ************************************************************************

#include "Arduino.h"

// ************************************************************************
// Asysob_GPS
//
// For more details, check the cpp source file.
// ************************************************************************
class Asysob_GPS {
	public:
		Asysob_GPS ( HardwareSerial *_gps_channel );
		void Init ( uint16_t baud );
		
		void SendCommand ( char *command );
					
		char *GPSMessage ( char *pattern, bool cooked );
		char *MessageEntry ( uint8_t index );
		uint8_t EntryCount ();

	private:
		void GPSEmptyChannel ();
		HardwareSerial *gps_channel;
		static const int max_message = 128;
		char message[max_message];
		static const int max_entries = 20;
		char *entry[max_entries];
		uint8_t n_entries;
};

#endif