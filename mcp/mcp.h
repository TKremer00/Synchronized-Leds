#ifndef MCP_CPP
#define MCP_CPP

#include "mbed.h"
#include <cstdint>
#include <functional>

enum Direction {
    FORWARDS,
    BACKWARDS,
};

enum PinNumber {
    // 255    = 0b11111111
    PIN_ONE   = 0b00001010,
    PIN_TWO   = 0b00010100,
    PIN_THREE = 0b00011110,
    PIN_FOUR  = 0b00101000,
    PIN_FIVE  = 0b00110010,
    PIN_SIX   = 0b00111100,
};

class MCP {

public:
    static constexpr uint8_t AMOUNT_OF_LEDS = 5;
    
    MCP();

    bool next_button_pressed();

    bool previous_button_pressed();

    void turn_on_led(PinNumber pin_number);

    PinNumber get_current_lighted_led();

    PinNumber get_next_led(Direction direction);

private:
    void reset_registers();

    void set_register(uint8_t reg_address, int reg_data);

    int get_register(uint8_t reg_address);

    bool check_button_with_debounce(std::function<bool()> is_button_pressed, bool* previous_button_state);

    SPI m_spi;    
    DigitalOut m_nCS;
    
    bool m_previous_button_state = false;
    bool m_next_button_state = false;

    PinNumber m_current_lighted_led = PinNumber::PIN_ONE;

    static constexpr uint8_t DEBOUNCE_TIME_MS = 50;
};

#endif