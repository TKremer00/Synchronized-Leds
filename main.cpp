#include "mbed.h"
#include "nrf24/nrf24.h"
#include "mcp/mcp.h"
#include <cstdio>

#define MAX_ACKNOWLEDGMENT_TIMEOUT_S    0.40
#define SEND_DELAY_MS    150

void send_pin_update(NRF24& nrf, uint8_t new_pin_number) {
    while(true) {
        nrf.send_led_update(new_pin_number);

        ThisThread::sleep_for(SEND_DELAY_MS);

        Timer timer;
        timer.start();

        while(timer.read() < MAX_ACKNOWLEDGMENT_TIMEOUT_S){
            LedPackage received_pin_number = nrf.read_acknowledgement();
            if(received_pin_number.is_acknowledgement) {
                printf("Recieved acknowledgement %d %d \r\n", received_pin_number.pin_number, received_pin_number.is_acknowledgement);
                return;
            }        
        }

        printf("No acknowledgement receive within %fs trying again \r\n", MAX_ACKNOWLEDGMENT_TIMEOUT_S);
    }
}

int main() {
    Serial pc(USBTX, USBRX); // tx, rx
    NRF24 nrf;
    MCP mcp;

    char txData[NRF24::TRANSFER_SIZE];
    char rxData[NRF24::TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;
    
    while (1) {
        if(mcp.next_button_pressed()) {
            printf("NEXT Pressed \r\n");
            
            auto new_pin_number = mcp.get_next_led(Direction::FORWARDS); 
            send_pin_update(nrf, new_pin_number);
            mcp.turn_on_led(new_pin_number);
        }

        if(mcp.previous_button_pressed()) {
            printf("PREVIOUS Pressed \r\n");

            auto new_pin_number = mcp.get_next_led(Direction::BACKWARDS);
            send_pin_update(nrf, new_pin_number);
            mcp.turn_on_led(new_pin_number);
        }

        LedPackage received_package = nrf.receive_led_update(); // 60 - false
        if(received_package.pin_number != std::numeric_limits<uint8_t>::max()) {
            ThisThread::sleep_for(SEND_DELAY_MS);
            // we got a pin update, now we need to send it back
            received_package.is_acknowledgement = true;
            auto bytes_written = nrf.send_acknowledgement(received_package);
            if(bytes_written > 0) {
                // we did send the led update back as acknowledgment, and we did send more than 0 bytes.
                mcp.turn_on_led(static_cast<PinNumber>(received_package.pin_number));
            }
        }
    }
}