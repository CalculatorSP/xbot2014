/** Serial.h
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * License: This source code can be used and/or modified without restrictions.
 * It is provided as is and the author disclaims all warranties, expressed 
 * or implied, including, without limitation, the warranties of
 * merchantability and of fitness for any purpose. The user must assume the
 * entire risk of using the Software.
 *
 * @author Hans de Ruiter
 * @author John Miller
 *
 * @version 0.1 -- 28 October 2008
 * @version 0.2 -- 8 February 2015 - Specify com port in constructor call
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <string>
#include <windows.h>

typedef std::basic_string<TCHAR> tstring;

class Serial
{
private:
	HANDLE commHandle;

public:
	Serial(const char *comport, int bitRate = 115200);

	virtual ~Serial();

	/** Writes a NULL terminated string.
	 *
	 * @param buffer the string to send
	 *
	 * @return int the number of characters written
	 */
	int write(const char buffer[]) const;

	/** Writes a string of bytes to the serial port.
	 *
	 * @param buffer pointer to the buffer containing the bytes
	 * @param buffLen the number of bytes in the buffer
	 *
	 * @return int the number of bytes written
	 */
	int write(const unsigned char *buffer, int buffLen) const;

	/** Reads a string of bytes from the serial port.
	 *
	 * @param buffer pointer to the buffer to be written to
	 * @param buffLen the size of the buffer
	 * @param nullTerminate if set to true it will null terminate the string
	 *
	 * @return int the number of bytes read
	 */
	int read(char *buffer, int buffLen, bool nullTerminate = true) const;

	/** Flushes everything from the serial port's read buffer
	 */
	void flush() const;
};

#endif
