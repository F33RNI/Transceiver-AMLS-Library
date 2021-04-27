

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
