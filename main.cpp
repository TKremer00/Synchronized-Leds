#include "mbed.h"
#include "nrf24/nrf24.h"
#include "mcp/mcp.h"
#include <cstdio>

// void read_serial(Serial& pc, NRF24& nrf, char txData[], int& txDataCnt){
//     if (pc.readable()) {
//         // ...add it to the transmit buffer
//         txData[txDataCnt++] = pc.getc();

//         // If the transmit buffer is full
//         if ( txDataCnt >= sizeof(txData)) {

//             // Send the transmit buffer via the nRF24L01+
//             nrf.send_data(NRF24L01P_PIPE_P0, txData, txDataCnt);
//             printf("Send %s \r\n", txData);

//             txDataCnt = 0;
//         }

//         // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
//     }
// }
#define MAX_ACKNOWLEDGMENT_TIMEOUT_S    10
#define SEND_DELAY_S    1000

bool send_pin_update(NRF24& nrf, uint8_t new_pin_number) {

    nrf.send_led_update(new_pin_number);

    ThisThread::sleep_for(SEND_DELAY_S);

    Timer timer;
    timer.start();

    while(timer.read() < MAX_ACKNOWLEDGMENT_TIMEOUT_S){
        uint8_t received_pin_number = nrf.read_acknowledgement();

        if(received_pin_number != std::numeric_limits<uint8_t>::max()) {
            return received_pin_number == new_pin_number;
        }        
    }

    printf("No acknowledgement receive within %ds \r\n", MAX_ACKNOWLEDGMENT_TIMEOUT_S);

    return false;
}

int main() {
    Serial pc(USBTX, USBRX); // tx, rx
    NRF24 nrf;
    MCP mcp;

    char txData[NRF24::TRANSFER_SIZE];
    char rxData[NRF24::TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;
    
    // TODO: this does hardfault some time, need to look into it.

    while (1) {
        if(mcp.next_button_pressed()) {
            printf("NEXT Pressed \r\n");
            
            auto new_pin_number = mcp.get_next_led(Direction::FORWARDS); 
            if(send_pin_update(nrf, new_pin_number)) {
                mcp.turn_on_led(new_pin_number);
            }
        }

        if(mcp.previous_button_pressed()) {
            printf("PREVIOUS Pressed \r\n");

            auto new_pin_number = mcp.get_next_led(Direction::BACKWARDS);
            if(send_pin_update(nrf, new_pin_number)){
                mcp.turn_on_led(new_pin_number);
            }
        }

        uint8_t received_bytes = nrf.receive_led_update();
        if(received_bytes != std::numeric_limits<uint8_t>::max()) {
            ThisThread::sleep_for(SEND_DELAY_S * 2);
            // we got a pin update, now we need to send it back
            auto bytes_written = nrf.send_led_update(received_bytes);
            if(bytes_written > 0) {
                // we did send the led update back as acknowledgment, and we did send more than 0 bytes.
                mcp.turn_on_led(static_cast<PinNumber>(received_bytes));
            }
        }
    }
}