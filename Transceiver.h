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

#pragma once
#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <iostream>
#include <thread>
#include <cstring>

#define BUFFER_SIZE	512

#if defined(_WIN32) ||  defined(_WIN64)		//Windows
#define PLATFORM_NAME 1
#include <windows.h>
#else	//Linux
#define PLATFORM_NAME 0
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

class Transceiver {
public:
	Transceiver(const char* port, uint32_t boudrate, uint8_t byte_size, uint8_t stop_bits);
	bool uart_open();
	bool uart_write(const char* data);
	void uart_read(void (*cb)(char* data));
protected:
	void (*callback)(char* data);

private:
	void uart_accumulator(void);
	char uart_buffer[BUFFER_SIZE];
	uint16_t sofar = 0;
	const char* _port;
	uint32_t _boudrate;
	uint8_t _byte_size;
	uint8_t _stop_bits;
	char received_char;

#if PLATFORM_NAME == 1
	HANDLE h_serial;
	DWORD size;
#else
	int uart0_filestream = -1;
	int16_t size;
#endif
};

#endif