#ifndef MCP_CPP
#define MCP_CPP

#include "mbed.h"
#include <cstdint>
#include <functional>

enum Direction {
    FORWARDS,
    BACKWARDS,
};

class MCP {

public:
    MCP();

    bool next_button_pressed();

    bool previous_button_pressed();

    void turn_on_led(uint8_t pin_number);

    uint8_t get_current_lighted_led();

    uint8_t get_next_led(Direction direction);

private:
    void reset_registers();

    void set_register(uint8_t reg_address, int reg_data);

    int get_register(uint8_t reg_address);

    bool check_button_with_debounce(std::function<bool()> is_button_pressed, bool* previous_button_state);

    SPI m_spi;    
    DigitalOut m_nCS;
    
    bool m_previous_button_state = false;
    bool m_next_button_state = false;

    uint8_t m_current_lighted_led = 0;

    static constexpr uint8_t AMOUNT_OF_LEDS = 5;
    static constexpr uint8_t DEBOUNCE_TIME_MS = 50;
};

#endif