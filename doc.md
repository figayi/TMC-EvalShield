# Documentation

## Hardware Abstraction Layer (HAL)
The HAL is based on STM32Cube HAL with peripheral drivers.  
Additionally, there are TMC wrappers around this, which furthermore simplifies usage.  

### GPIO
TODO

### TMC Connection interface
Modules using the *TMC Connection* interface (`TMC_Connection`) can be used for communication in a more abstract and collective manner.  
This interface is generally stored in communication modules structs as `struct.con`.  
After initialization, `con.rxN` and `con.txN` can be used for communication, so the application does not have to
hassle with multiple interfaces, each with its own communication functions.

### UART
Regular UART communication is made via the *U(S)ART3* module.  
UART utilizes the TMC Connection interface. Core communication functions are:
```C
void UART_rxN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
void UART_txN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
```
Both functions are used to receive/transmit a portion of data (starting at `&data[0]`) of size `size`.
Receiving is made *asynchronous* with *DMA* while transmitting is *synchronous*.
These functions can only be called after initialization of the TMC_UART interface.

### SPI
SPI communication is made via the *SPI1* module.
SPI utilizes the TMC Connection interface. Core communication functions are:
```C
void SPI_rxN(TMC_SPI *interface, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_txN(TMC_SPI *interface, uint8_t *data, uint16_t size, uint32_t timeout);
```
Both functions are used to receive/transmit a portion of data (starting at `&data[0]`) of size `size`.
Receiving and transmitting are both *synchronous*.
These functions can only be called after initialization of the TMC_SPI interface.

### USB
The USB connection is made via the *U(S)ART2* module. It is connected to the onboard ST-LINK,
which can be accessed from host as Virtual COM Port. Therefore, communication semantics are the same
as regular UART.