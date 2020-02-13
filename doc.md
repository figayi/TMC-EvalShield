# Documentation

## Hardware Abstraction Layer (HAL)
The HAL is based on STM32Cube HAL with peripheral drivers.  
Additionally, there are TMC wrappers around this, which furthermore simplifies usage.  

### GPIO
GPIOs are managed with the `TMC_Pin` interface. It holds the initial configuration amongst other state information.  
GPIO's static initial configuration is set up in *GPIO.c*.  
After initialization via `GPIO_init(...)`, the initial GPIO configuration is set and the IOs can be accessed via the following functions:  

```C
TMC_Pin *GPIO_getPin(uint8_t number);
void GPIO_setToInput(TMC_Pin *pin);
void GPIO_setToOutput(TMC_Pin *pin);
void GPIO_setHigh(TMC_Pin *pin);
void GPIO_setLow(TMC_Pin *pin);
void GPIO_setFloating(TMC_Pin *pin);
```

Using `GPIO_getPin(...)` the IO for a given MCU pin number can be obtained comfortably.
If there is no IO for that pin number (i.e. not connected), it returns `NULL`.  
The other manipulating functions should be self explainatory.

### TMC Connection interface
Modules using the *TMC Connection* interface (`TMC_Connection`) can be used for communication in a more abstract and collective manner.  
This interface is generally stored in communication modules structs as `struct.con`.  
All general purpose communication functions available via `con.<function>(...)` are mapped to module specific functions `<module>_<function>`.
After initialization, `con.rxRequest(...)`, `con.txRequest(...)`, `con.rxN(...)` and `con.txN(...)`
can be used for communication, so the application does not have to hassle with multiple interfaces, each with its own communication functions.  
In asynchronous connections, `con.dataAvailable(...)` can be used to poll the amount of data available to read since last buffer reset.

#### Limitations / Pitfalls
TMC Connection RXTX buffer size is limited to 1024 bytes by default. It can be adjusted by defining `TMC_RXTX_BUFFER_SIZE`.  
However, this buffer size limits the amount of data that can be handled in one RX request and buffer readout cycle.  
With typical UART baudrates (< 1000000 baud) this limit will likely never be reached.

### UART
Regular UART communication is made via the *U(S)ART3* module.  
Receiving is made *asynchronous* with *DMA* while transmitting is *synchronous*.  
UART utilizes the TMC Connection interface. Core communication functions are:

```C
void UART_rxRequest(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
void UART_rxN(TMC_UART *interface, uint8_t *data, size_t size);
void UART_txRequest(TMC_UART *interface, uint8_t *data, uint16_t size, uint32_t timeout);
```

`UART_rxRequest(...)` is used to request receiving data. If `data == NULL` (recommended),
internal RXTX buffers will be used, with all its benefits (`dataAvailable` polling etc.).
Otherwise, the *DMA* directly writes to the given array, and the user has to manually manage the buffer.  
When using internal buffers, `UART_rxN(...)` can be used to read out a portion of data from the buffer, if available.  
Since transmitting is synchronous, `UART_txRequest(...)` will directly send the given data without buffering.
These functions can only be called after initialization of the TMC_UART interface.

### SPI
SPI communication is made via the *SPI1* module.  
Receiving and transmitting are both *synchronous*.  
SPI utilizes the TMC Connection interface. Core communication functions are:

```C
void SPI_rxRequest(TMC_SPI *interface, uint8_t *data, uint16_t size, uint32_t timeout);
void SPI_txRequest(TMC_SPI *interface, uint8_t *data, uint16_t size, uint32_t timeout);
```

Both functions are used to receive/transmit a portion of data (starting at `&data[0]`) of size `size`.  
Since transmitting and receiving are both synchronous, `SPI_rxRequest(...)` and `SPI_txRequest(...)` will directly
read/write the given data without buffering.
These functions can only be called after initialization of the TMC_SPI interface.  
Currently, there are 3 SPI interfaces, globally accessible with *TMC\_SPI\_Channel[]* array.

### USB
The USB connection is made via the *U(S)ART2* module. It is connected to the onboard ST-LINK,
which can be accessed from host as Virtual COM Port. Therefore, communication semantics are the same
as regular UART.