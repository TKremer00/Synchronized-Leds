#ifndef NRF24_CPP
#define NRF24_CPP

#include "nRF24L01P.h"

class NRF24 {
public:
    // The nRF24L01+ supports transfers from 1 to 32 bytes, but Sparkfun's
    // "Nordic Serial Interface Board" (http://www.sparkfun.com/products/9019)
    // only handles 4 byte transfers in the ATMega code.
    static constexpr int TRANSFER_SIZE = 4;

    NRF24();

    
    /**
    * @brief Read the acknowledgment of the other NRF24.
    * @return the pin number of the other NRF24, if fails return max size of uint8_t.
    */
    uint8_t read_acknowledgement();

    int send_led_update(uint8_t pin);
    
    /**
    * @brief receive the led update of the NRF24.
    * @return the pin number of the other NRF24, if fails return max size of uint8_t.
    */
    uint8_t receive_led_update();
    
    // FIXME: remove this code, is only to test the hello world.
    void send_data(int pipe, char *data, int count);
private:
    void print_nrf_info();

    uint8_t number_from_char_array(char data[], int size);
    
    uint8_t receive_incoming_data();
    
    // Members
    nRF24L01P m_nrf_comm;

};


#endif