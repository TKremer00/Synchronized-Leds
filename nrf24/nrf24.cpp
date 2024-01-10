#include "nrf24.h"

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

bool NRF24::read_incoming_data(char rxData[], int size) {
    if(size != TRANSFER_SIZE) {
        error("Transfer size does not match the intended transfer size of the nRF24L01P");
    }

    if(!m_nrf_comm.readable()) {
        return false;
    }

    m_nrf_comm.read(NRF24L01P_PIPE_P0, rxData, size);
    printf("RXDATA in the read_incomming_data function %s \r\n", rxData);

    return true;
}

void NRF24::send_led_update(uint8_t pin) {
    printf("TODO: send the pin number to the other nrf, and acknowledge that message before updating my own led");
}

void NRF24::print_nrf_info() {
    // Display the (default) setup of the nRF24L01+ chip
    printf( "nRF24L01+ Frequency    : %d MHz\r\n",  m_nrf_comm.getRfFrequency() );
    printf( "nRF24L01+ Output power : %d dBm\r\n",  m_nrf_comm.getRfOutputPower() );
    printf( "nRF24L01+ Data Rate    : %d kbps\r\n", m_nrf_comm.getAirDataRate() );
    printf( "nRF24L01+ TX Address   : 0x%010llX\r\n", m_nrf_comm.getTxAddress() );
    printf( "nRF24L01+ RX Address   : 0x%010llX\r\n", m_nrf_comm.getRxAddress() );
}

void NRF24::send_data(int pipe, char *data, int count) {
    m_nrf_comm.write(pipe, data, count);
}
