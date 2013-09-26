# Asysob_GPS
A simple - and hopefully very fast - Arduino library to access UART-based GPS devices.

At its core, a single method `GPSMessage()` is needed to access the GPS device.  The two arguments of `GPSMessage()` define the details on how the raw NMEA sequences are processed:

## `GPSMessage(pattern,cooked)`
### Argument `pattern`
If NULL the next complete NMEA or device-specific message is received whatever message is first. if pattern is a given string, the method awaits the receipt of this message only; e.g. if a GPRMC message is requested the pattern should be 'GPRMC' without the leading '$'. Only messages received completely with correct checksum are parsed and returned.

### Argument `cooked`
If true, the message is split into individual entries. The number of entries and the values of these entries can be retrieved using `EntryCount()` and `MessageEntry(0<=i<EntryCount())`. With cooked equal to false, the complete NMEA or device-specific message is returned (without the leading '$' but with the checksum). In this case, `EntryCount()` returns 1 and the only `MessageEntry()` available is the whole message.