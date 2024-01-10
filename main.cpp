#include "mbed.h"
#include "nrf24/nrf24.h"
#include <cstdio>

void read_serial(Serial& pc, NRF24& nrf, char txData[], int& txDataCnt){
    if (pc.readable()) {
        // ...add it to the transmit buffer
        txData[txDataCnt++] = pc.getc();

        // If the transmit buffer is full
        if ( txDataCnt >= sizeof(txData)) {

            // Send the transmit buffer via the nRF24L01+
            nrf.send_data(NRF24L01P_PIPE_P0, txData, txDataCnt);
            printf("Send %s \r\n", txData);

            txDataCnt = 0;
        }

        // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
    }
}

int main() {
    Serial pc(USBTX, USBRX); // tx, rx
    NRF24 nrf;

    char txData[NRF24::TRANSFER_SIZE];
    char rxData[NRF24::TRANSFER_SIZE];
    int txDataCnt = 0;
    int rxDataCnt = 0;

    pc.printf( "Type keys to test transfers:\r\n  (transfers are grouped into %d characters)\r\n", NRF24::TRANSFER_SIZE );

    while (1) {

        // If we've received anything over the host serial link...
        read_serial(pc, nrf, txData, txDataCnt);

        // If we've received anything in the nRF24L01+...
        if(nrf.read_incoming_data(rxData, sizeof(rxData))) {
            // TODO: we read some bytes, what now....
            printf("%s \r\n", rxData);
        }
    }
}
