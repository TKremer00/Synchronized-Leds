#include "nrf24.h"


#define MOSI_PIN    D11
#define MISO_PIN    D12
#define SCK_PIN     D13
#define CSN_PIN     D8
#define CE_PIN      D9
#define IRQ_PIN     D7

NRF24::NRF24()
    : m_nrf_comm(MOSI_PIN, MISO_PIN, SCK_PIN, CSN_PIN, CE_PIN, IRQ_PIN)
{
    // NOTE: this is to fix the power up, getting stuck
    // Don't know why, but is a fix :)
    ////////////////////////////////////////////
    SPI spi(MOSI_PIN, MISO_PIN, SCK_PIN);
    spi.format(8, 3);
    ////////////////////////////////////////////

    m_nrf_comm.powerUp();

    print_nrf_info();

    m_nrf_comm.setTransferSize(TRANSFER_SIZE);

    m_nrf_comm.setReceiveMode();
    m_nrf_comm.enable();
}

uint8_t NRF24::read_acknowledgement() {
    return receive_incoming_data();
}

int NRF24::send_led_update(uint8_t pin) {
    // NOTE: this could always fail, 
    // but we want to make sure we don't 
    // send more bytes than the NRF can read
    MBED_ASSERT(sizeof(pin) <= TRANSFER_SIZE);

    char pin_number = static_cast<char>(pin);

    return m_nrf_comm.write(NRF24L01P_PIPE_P0, &pin_number, sizeof(pin));
}

uint8_t NRF24::receive_led_update() {
    return receive_incoming_data();
}

void NRF24::print_nrf_info() {
    // Display the (default) setup of the nRF24L01+ chip
    printf( "nRF24L01+ Frequency    : %d MHz\r\n",  m_nrf_comm.getRfFrequency() );
    printf( "nRF24L01+ Output power : %d dBm\r\n",  m_nrf_comm.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\r\n", m_nrf_comm.getAirDataRate() );
    printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", m_nrf_comm.getTxAddress() );
    printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", m_nrf_comm.getRxAddress() );
}

uint8_t NRF24::number_from_char_array(char data[], int size) {
    uint8_t number = 0;

    printf("we are here, with size %d \r\n", size);

    for (int i = size; i >= 0; i--)
    {
        printf("we in loop %d \r\n", i);
        number |= data[i] << i;
    }

    return number;
}

uint8_t NRF24::receive_incoming_data() {
    // FIXME: check the crc, to ensure the data is valid
    char rxData[TRANSFER_SIZE];
    
    if(!m_nrf_comm.readable()) {
        return std::numeric_limits<uint8_t>::max();
    }

    m_nrf_comm.read(NRF24L01P_PIPE_P0, rxData, TRANSFER_SIZE);
    

    auto number = number_from_char_array(rxData, TRANSFER_SIZE);

    printf("Received: %d \r\n", number);

    return number;
}

void NRF24::send_data(int pipe, char *data, int count) {
    m_nrf_comm.write(pipe, data, count);
}
