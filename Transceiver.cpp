/*
  *********************************************************
  *                                                       *
  * UART Transceiver for Windows and Linux (Raspberry PI) *
  *                                                       *
  ********************************************************************************************
  *
  * COPYRIGHT(c) 2020 Pavel Evgenyevich Neshumov
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *
  * THIS SOFTWARE WAS DEVELOPED AS PART OF PROJECT ACTIVITY IN GROUP 181-311 AT THE
  * MOSCOW POLYTECHNIC UNIVERCITY. HOWEVER, THE OWNER (Pavel Evgenyevich Neshumov)
  * REMAINS THE RIGHT, WITHOUT NOTICE, TO CHANGE THE PURPOSE AND ENVIRONMENT OF APPLICATION
  * OF THIS SOFTWARE. ALSO THE OWNER (Pavel Evgenyevich Neshumov) MAY PROHIBIT USE OF THIS
  * SOFTWARE FOR ANY ORGANIZATION, PEOPLE, STRUCTURES.
  *
  ********************************************************************************************
*/

#include "Transceiver.h"

Transceiver::Transceiver(const char* port, uint32_t boudrate, uint8_t byte_size, uint8_t stop_bits) {
	_port = port;
	_boudrate = boudrate;
	_byte_size = byte_size;
	_stop_bits = stop_bits;
}

bool Transceiver::uart_open() {
#if PLATFORM_NAME == 1
	//Windows
	wchar_t wport[32];
	mbstowcs(wport, _port, strlen(_port) + 1);
	LPWSTR ptr = wport;

	h_serial = CreateFile(wport, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (h_serial == INVALID_HANDLE_VALUE)
	{
		printf("[UART] Open error.\n");
		return 0;
	}
	DCB dcb_serial_params = { 0 };
	dcb_serial_params.DCBlength = sizeof(dcb_serial_params);
	if (!GetCommState(h_serial, &dcb_serial_params))
	{
		printf("[UART] Getting state error\n");
		return 0;
	}
	dcb_serial_params.BaudRate = _boudrate;
	dcb_serial_params.ByteSize = _byte_size;
	dcb_serial_params.StopBits = _stop_bits;
	dcb_serial_params.Parity = NOPARITY;
	if (!SetCommState(h_serial, &dcb_serial_params))
	{
		printf("[UART] Error setting serial port state\n");
		return 0;
	}
	return 1;
#else
	//Linux
	uart0_filestream = -1;
	uart0_filestream = open(_port, O_RDWR | O_NOCTTY | O_NDELAY);
	if (uart0_filestream == -1)
	{
		printf("[UART] Unable to open UART\n");
		return 0;
	}
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = _boudrate | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	return 1;
#endif
}

bool Transceiver::uart_write(const char* data) {
#if PLATFORM_NAME == 1
	//Windows
	DWORD dwSize = strlen(data);
	DWORD dwBytesWritten;
	return WriteFile(h_serial, data, dwSize, &dwBytesWritten, NULL);
#else
	//Linux
	if (uart0_filestream != -1)
	{
		int16_t count = write(uart0_filestream, data, strlen(data));
		if (count < 0)
			return 0;
	}
	return 1;
#endif
}

void Transceiver::uart_read(void (*cb)(char* data)) {
	callback = cb;

	std::thread t = std::thread(&Transceiver::uart_accumulator, this);
	t.detach();
}

void Transceiver::uart_accumulator(void) {

	for(;;)
	{
#if PLATFORM_NAME == 1
		//Windows
		if (!ReadFile(h_serial, &received_char, 1, &size, 0)) {
			printf("[UART] Reading fatal error\n");
			break;
		}
#else
		//Linux
		if (uart0_filestream != -1)
		{
			size = read(uart0_filestream, &received_char, 1);
		}
#endif
		if (size > 0) {
			if (sofar < BUFFER_SIZE - 1) uart_buffer[sofar++] = received_char;
			if ((received_char == '\n') || (received_char == '\r')) {
				uart_buffer[sofar] = 0;
				Transceiver::callback(uart_buffer);
				sofar = 0;
			}
		}
	}
}