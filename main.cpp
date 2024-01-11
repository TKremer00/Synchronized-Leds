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
            mcp.turn_on_led(mcp.get_next_led(Direction::FORWARDS));
            wait_ns(500);
        }

        if(mcp.previous_button_pressed()) {
            printf("PREVIOUS Pressed \r\n");
            mcp.turn_on_led(mcp.get_next_led(Direction::BACKWARDS));
            wait_ns(500);
        }

        // If we've received anything in the nRF24L01+...
        if(nrf.read_incoming_data(rxData, sizeof(rxData))) {
            // TODO: we read some bytes, what now....
            printf("%s \r\n", rxData);
        }
    }
}