# Transceiver-AMLS-Library
This library is a part of AMLS project

Library usage example:
```cpp
#include "Transceiver.h"	//Include library

Transceiver transceiver("\\\\.\\COM9", 9600, 8, 0);	//Init UART settings

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

```
