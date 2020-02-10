# Operational guide

## UART RX (functional access)

```C
// TMC_UART interface for UART0 is already defined in UART.h.
#include "UART.h"
UART0_init();
// Requesting RX for 256 bytes on interface UART0 using internal buffers and a timeout of 1 second.
UART_rxRequest(&UART0, NULL, 256, 1000);
// Waiting for data available.
while(UART_dataAvailable(&UART0) < 256);
uint8_t data[256];
// Data is available. Copy it from internal data to data array.
UART_rxN(&UART0, &data[0], 256);
```

## UART RX (structural access via TMC_Connection)

```C
// TMC_UART interface for UART0 is already defined in UART.h.
#include "UART.h"
UART0_init();
// TMC_Connection interface for UART0 is now set up.
// Requesting RX for 256 bytes on interface UART0 using internal buffers and a timeout of 1 second.
UART0.con.rxRequest(NULL, 256, 1000);
// Waiting for data available.
while(UART0.con.dataAvailable() < 256);
uint8_t data[256];
// Data is available. Copy it from internal data to data array.
UART0.con.rxN(&data[0], 256);
```

## UART RX with multiple interfaces

Interfaces can be operated as a batch using the TMC_Connection interface.

```C
#include "UART.h"
UART0_init();
UART1_init();
UART2_init();
TMC_Connection *interfaces[3] = { &UART0.con, &UART1.con, &UART2.con };
for(size_t i = 0; i < 3; i++)
	// Do stuff with interfaces
```