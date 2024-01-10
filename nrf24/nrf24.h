#ifndef NRF24_CPP
#define NRF24_CPP

#include "nRF24L01P.h"

// The nRF24L01+ supports transfers from 1 to 32 bytes, but Sparkfun's
// "Nordic Serial Interface Board" (http://www.sparkfun.com/products/9019)
// only handles 4 byte transfers in the ATMega code.
#define MOSI_PIN    D11
#define MISO_PIN    D12
#define SCK_PIN     D13
#define CSN_PIN     D8 // D10 <- MCP
#define CE_PIN      D9
#define IRQ_PIN     D7


class NRF24 {

public:
    static constexpr int TRANSFER_SIZE = 4;

    NRF24();

    bool read_incoming_data(char rxData[], int size);

    void send_led_update(uint8_t pin);

    // FIXME: remove this code, is only to test the hello world.
    void send_data(int pipe, char *data, int count);
private:
    void print_nrf_info();
    
    // Members
    nRF24L01P m_nrf_comm;

};


#endif