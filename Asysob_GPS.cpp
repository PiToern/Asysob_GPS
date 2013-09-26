#include "Asysob_GPS.h"

// ************************************************************************
Asysob_GPS::Asysob_GPS ( HardwareSerial *_gps_channel ) {
	gps_channel = _gps_channel;
	n_entries = 0;
}

// ************************************************************************
void Asysob_GPS::Init ( uint16_t baud ) {
	gps_channel->begin(baud);
}

// ************************************************************************
// SendCommand: A device specific command can be sent. The correct
// checksum is calculated and appended.
// ************************************************************************
void Asysob_GPS::SendCommand ( char *command ) {
	static char hexdigits[] = "0123456789ABCDEF";
	message[0] = '$';
	byte check = 0;
	char *dest = message+1;
	while (*command != (char) 0) {
		char c = *command++;
		*dest++ = c;
		check ^= c;
	}
	*dest++ = '*';
	*dest++ = hexdigits[(check >> 4) & 0xf];
	*dest++ = hexdigits[check & 0x0f];
	*dest++ = (char) 0x0d;
	*dest++ = (char) 0x0a;
	*dest = (char) 0;
	gps_channel->println(message);
}

// ************************************************************************
void Asysob_GPS::GPSEmptyChannel () {
	char c;
	// Cleaning up chars already in the pipeline
	int skip = gps_channel->available();
	while (--skip >= 0)
		c = gps_channel->read();
	// Awaiting the next '$' before logging starts ...
	do {
		while (!gps_channel->available()) ;
		c = gps_channel->read();
	} while (c != '$');
}

enum State {
	WAIT_FOR_DOLLAR,
	NMEA_TYPE,
	ENTRY_START,
	CHECKSUM_1,
	CHECKSUM_2
};

// ************************************************************************
// GPSMessage
//
// Argument pattern: if NULL the next complete NMEA or device-specific
// message is received whatever message is first. if pattern is a given
// string, the method awaits the receipt of this message only; e.g. if
// a GPRMC message is requested the pattern should be 'GPRMC' without
// the leading '$'. Only messages received completely with correct
// checksum are parsed and returned.
//
// Argument cooked: if true, the message is split into individual entries.
// The number of entries and the values of these entries can be retrieved
// using EntryCount() and MessageEntry(0<=i<EntryCount()). With cooked
// equal to false, the complete NMEA or device-specific message is returned
// (without the leading '$' but with the checksum). In this case, EntryCount()
// returns 1 and the only MessageEntry() available is the whole message.
//
// ************************************************************************
char *Asysob_GPS::GPSMessage ( char *pattern, bool cooked = true ) {
	byte check = 0;
	byte check_received = 0;
	uint16_t i = 0;
	State state = WAIT_FOR_DOLLAR;
	char *entry_begin = NULL;
	char hc;

	GPSEmptyChannel();
	char c = '$';
	while (true) {
		// Serial.print(c); // DEBUG
		switch (state) {
			case WAIT_FOR_DOLLAR:
				if (c == '$') {
					state = NMEA_TYPE;
					n_entries = 0;
					entry_begin = message+1;
					check = 0;
					i = 0;
				}
				break;
			case NMEA_TYPE:
				// Does c fit into current search pattern if there is one?
				hc = (c == ',') ? (char) 0 : c;
				if ((pattern != NULL) && (pattern[i-1] != hc)) {
					state = WAIT_FOR_DOLLAR;
					break;
				}
			case ENTRY_START:	
				if (c != '*') check ^= (byte) c;	
				if ((c == ',') || (c == '*')) {
					state = (c == ',') ? ENTRY_START : CHECKSUM_1;
					if (cooked) {
						c = (char) 0;
						entry[n_entries++] = entry_begin;
						entry_begin = message+i+1;
					}
				}
				break;
			case CHECKSUM_1:
				check_received = (((c < 'A') ? c-'0' : c-'A'+10) << 4);
				state = CHECKSUM_2;
				break;
			case CHECKSUM_2:
				check_received |= ((c < 'A') ? c-'0' : c-'A'+10);
				if (check == check_received) {
					// Serial.println(); // DEBUG
					if (!cooked) {
						message[i++] = c;
						message[i] = (char) 0;
						return message;
					}
					return entry[0];
				}
				else
					state = WAIT_FOR_DOLLAR;
				break;
		}
		message[i++] = c;
		while (!gps_channel->available()) ;
		c = gps_channel->read();
		if (c == '$') {
			state = WAIT_FOR_DOLLAR;
			continue;
		}
	}
	// Serial.println(" --- Ops, wrong place?"); // DEBUG
	return entry[0];
}

// ************************************************************************
char *Asysob_GPS::MessageEntry ( uint8_t index ) {
	if (index < n_entries)
		return entry[index];
	return NULL;
}

// ************************************************************************
uint8_t Asysob_GPS::EntryCount () {
	return n_entries;
}
