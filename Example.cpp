/*
  *********************************************************
  *                                                       *
  * UART Transceiver for Windows and Linux (Raspberry PI) *
  *                                                       *
  *********************************************************
  */

#include "Transceiver.h"	//Include library

Transceiver transceiver(SERIAL_PORT, SERIAL_BODRATE, SERIAL_BITS, SERIAL_STOP_BITS);	//Init UART settings

void uart_callback(char* data) {	//This function is called every time the symbol "\n" or "\r" appears in the incoming message.
	printf(data);	//Print message to console
	printf("\n");	//Print new line
}

int main()
{
	if (!transceiver.uart_open())	//Open port
		return 0;
	transceiver.uart_write("Hello World!");	//Send a message
	transceiver.uart_read(uart_callback);	//Define a reading callback

	for (;;)
	{
		//Doing smth...
	}
	exit(0);	//Kill all threads
	return 0;
}

