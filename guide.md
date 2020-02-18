# Operational guide

## Flashing

Flash any hex file from the release section via the ST-LINK software suite. Power cycle the TMC-EvalShield base board after that.

## Quick start with TMCL-IDE

Get started fast using the [TMCL-IDE](https://www.trinamic.com/support/software/tmcl-ide/).  

1. Attach the shields to your TMC-EvalShield base board.
2. Attach and turn on the power source for the connected shields.
3. Connect the TMC-EvalShield base board via USB to your PC.
4. Open up the TMCL-IDE, open the respective COM port, change the baudrate to 115200 and click connect.
5. Now you can use all the tools the TMCL-IDE provides.

## GPIO manipulation

GPIOs are managed with the `TMC_IO` interface.

```C
#include "HAL.h"
#include "GPIO.h"
GPIO_init();
// All GPIOs are initialized with their static configuration.
TMC_IO *io = GPIO_getPin(14);
// Get IO for MCU pin number 14, which is PA0 for STM32F103.
GPIO_setHigh(io);
// IO is now at high level.
HAL_Delay(1000);
GPIO_setLow(io);
// IO is now at low level.
HAL_Delay(1000);
GPIO_setFloating(io);
// IO is now disconnected (high Z).
```

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

## SPI read / write (functional access)

SPI is synchronous and therefore rx/tx operations are blocking and executed directly.  

```C
// TMC_SPI interfaces are already declared in the TMC_SPI_Channel[] array.
#include "SPI.h"
SPI_init(&TMC_SPI_Channel[0]);
// Using SPI channel 0.
// Creating buffer arrays.
uint8_t data_tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t data_rx[4] = { 0x00, 0x00, 0x00, 0x00 };
// Transmitting 4 bytes without direct read-back on SPI channel 0 with a timeout of 1 second.
SPI_txRequest(&TMC_SPI_Channel[0], &data_rx[0], 4, 1000);
// Transmitting 4 bytes with simultanious read-back on SPI channel 0 with a timeout of 1 second.
SPI_txrx(&TMC_SPI_Channel[0], &data_tx[0], &data_rx[0], 4, 1000);
// Requesting RX for 4 bytes on SPI channel 0 with a timeout of 1 second.
SPI_rxRequest(&TMC_SPI_Channel[0], &data_rx[0], 4, 1000);
```

## SPI read / write (structural access via TMC_Connection)

SPI is synchronous and therefore rx/tx operations are blocking and executed directly.  

```C
// TMC_SPI interfaces are already declared in the TMC_SPI_Channel[] array.
#include "SPI.h"
SPI_init(&TMC_SPI_Channel[0]);
// Using SPI channel 0.
// Creating buffer arrays.
uint8_t data_tx[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
uint8_t data_rx[4] = { 0x00, 0x00, 0x00, 0x00 };
// Transmitting 4 bytes without direct read-back on SPI channel 0 with a timeout of 1 second.
TMC_SPI_Channel[0].con.txRequest(&data_rx[0], 4, 1000);
// Transmitting 4 bytes with simultanious read-back on SPI channel 0 with a timeout of 1 second.
TMC_SPI_Channel[0].con.txrx(&data_tx[0], &data_rx[0], 4, 1000);
// Requesting RX for 4 bytes on SPI channel 0 with a timeout of 1 second.
TMC_SPI_Channel[0].con.rxRequest(&data_rx[0], 4, 1000);
```

## SPI read / write with multiple interfaces

Interfaces can be operated as a batch using the `TMC_Connection` interface.

```C
#include "SPI.h"
SPI_init(&TMC_SPI_Channel[0]);
SPI_init(&TMC_SPI_Channel[1]);
SPI_init(&TMC_SPI_Channel[2]);
TMC_Connection *interfaces[3] = { &TMC_SPI_Channel[0].con, &TMC_SPI_Channel[1].con, &TMC_SPI_Channel[2].con };
for(size_t i = 0; i < 3; i++)
  // Do stuff with interfaces
```