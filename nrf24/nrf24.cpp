#include "nrf24.h"
#include <cstdio>


#define MOSI_PIN    D11
#define MISO_PIN    D12
#define SCK_PIN     D13
#define CSN_PIN     D8
#define CE_PIN      D9
#define IRQ_PIN     D7
#define MAX_BYTES_NRF_CAN_HANDLE    32

NRF24::NRF24()
    : m_nrf_comm(MOSI_PIN, MISO_PIN, SCK_PIN, CSN_PIN, CE_PIN, IRQ_PIN)
{
    MBED_ASSERT(TRANSFER_SIZE <= MAX_BYTES_NRF_CAN_HANDLE);
    // NOTE: this is to fix the power up, getting stuck
    // Don't know why, but is a fix :)
    ////////////////////////////////////////////
    SPI spi(MOSI_PIN, MISO_PIN, SCK_PIN);
    spi.format(8, 3);
    ////////////////////////////////////////////

    m_nrf_comm.powerUp();



    m_nrf_comm.setTransferSize(TRANSFER_SIZE);
    m_nrf_comm.setRfFrequency(2500);
    m_nrf_comm.setRfOutputPower(-12);

    print_nrf_info();

    m_nrf_comm.setReceiveMode();
    m_nrf_comm.enable();
}

LedPackage NRF24::read_acknowledgement() {
    return receive_incoming_data();
}

int NRF24::send_led_update(uint8_t pin) {
    char* data = new char[TRANSFER_SIZE];
    encode_package(pin, data);

    printf("Sending pin_number %d \r\n", pin);

    auto send_bytes = m_nrf_comm.write(NRF24L01P_PIPE_P0, data, sizeof(TRANSFER_SIZE));

    m_nrf_comm.setReceiveMode();

    return send_bytes;
}

int NRF24::send_acknowledgement(LedPackage acknowledgement_package) {
    char* data = new char[TRANSFER_SIZE];
    encode_package(acknowledgement_package, data);

    printf("Sending acknowledgement with pin number %d \r\n", acknowledgement_package.pin_number);

    auto send_bytes = m_nrf_comm.write(NRF24L01P_PIPE_P0, data, sizeof(TRANSFER_SIZE));

    m_nrf_comm.setReceiveMode();

    return send_bytes;
}

LedPackage NRF24::receive_led_update() {
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

    for (int i = 0; i < size; i++) 
    {
        number |= data[i] << i;
    }

    return number;
}

LedPackage NRF24::receive_incoming_data() {
    char rxData[TRANSFER_SIZE];
    
    if(!m_nrf_comm.readable()) {
        return LedPackage {
            false,
            std::numeric_limits<uint8_t>::max(),
        };
    }
    printf("Receiving bytes \r\n");
    m_nrf_comm.read(NRF24L01P_PIPE_P0, rxData, TRANSFER_SIZE);

    LedPackage package = decode_package(rxData);
    return package;
}

void NRF24::encode_package(uint8_t pin, char* data) {
    LedPackage package {
        false,
        pin // Pin number
    };
    
    encode_package(package, data);
}

void NRF24::encode_package(LedPackage led_package, char* data) {
    MBED_ASSERT(sizeof(led_package) <= TRANSFER_SIZE);
    std::memcpy(data, &led_package, sizeof(led_package));
}

LedPackage NRF24::decode_package(char *data){
    LedPackage received_package;

    std::memcpy(&received_package, data, sizeof(LedPackage));

    return received_package;
}
