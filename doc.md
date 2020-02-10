# Documentation

## Hardware Abstraction Layer (HAL)
The HAL is based on STM32Cube HAL with peripheral drivers.  
Additionally, there are TMC wrappers around this, which furthermore simplifies usage.  

### GPIO
TODO

### TMC Connection interface
Modules using the *TMC Connection* interface (`TMC_Connection`) can be used for communication in a more abstract and collective manner.  
This interface is generally stored in communication modules structs as `struct.con`.  
After initialization, `con.rxN(...)` and `con.txN(...)` can be used for communication, so the application does not have to
hassle with multiple interfaces, each with its own communication functions.  
In asynchronous connections, `con.dataAvailable(...)` can be used to poll the amount of data available to read since last buffer reset.

#### Limitations / Pitfalls
TMC Connection RXTX buffer size is limited to 1024 bytes by default. It can be adjusted by defining `TMC_RXTX_BUFFER_SIZE`.  
However, this buffer size limits the amount of data that can be handled in one RX request and buffer readout cycle.  
With typical UART baudrates (< 1000000 baud) this limit will likely never be reached.

### UART
Regular UART communication is made via the *U(S)ART3* module.  
UART utilizes the TMC Connection interface. Core communication functions are:
```C
void UART_rxN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
void UART_txN(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
```
Both functions are used to receive/transmit a portion of data (starting at `&data[0]`) of size `size`.  
When `data == NULL` (recommended), internal RXTX buffers will be used, with all its benefits (`dataAvailable` polling etc.).
Otherwise, the *DMA* directly writes to the given array, and the user has to manually manage the buffer.  
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