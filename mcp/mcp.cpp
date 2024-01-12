#include "mcp.h"
#include <cstdint>
#include <cstdio>
#include <chrono>

#define MOSI_PIN    D11
#define MISO_PIN    D12
#define SCK_PIN     D13
#define CSN_PIN     D10

#define MCP_SPI_MAX_DATA_RATE     10000000

#define MCP23S08_ADDR        0x20
#define MCP23S08_IODIR       0x00
#define MCP23S08_GPIO        0x09
#define MCP23S08_GPIO_STATE  0x06

MCP::MCP() 
    : m_spi(MOSI_PIN, MISO_PIN, SCK_PIN),
      m_nCS(CSN_PIN)
{

    m_spi.frequency(MCP_SPI_MAX_DATA_RATE);
    m_spi.format(8, 0);

    reset_registers();
    turn_on_led(PinNumber::PIN_ONE);
}

bool MCP::next_button_pressed() {
    const uint8_t BUTTON = 1 << 7;    

    auto is_button_pressed = check_button_with_debounce([this]() {
        return (get_register(MCP23S08_GPIO) & BUTTON) == 0;
    }, &m_next_button_state);

    return is_button_pressed;
}

bool MCP::previous_button_pressed() {
    const uint8_t BUTTON = 1 << 6;

    auto is_button_pressed = check_button_with_debounce([this]() {
        return (get_register(MCP23S08_GPIO) & BUTTON) == 0;
    }, &m_previous_button_state);

    return is_button_pressed;
}

bool MCP::check_button_with_debounce(std::function<bool()> is_button_pressed, bool* previous_button_state) {
    auto current_state = is_button_pressed();

    if(current_state != *previous_button_state) {
        *previous_button_state = current_state;
        ThisThread::sleep_for(DEBOUNCE_TIME_MS);

        if(current_state == is_button_pressed()) {
            return current_state;
        }
    }

    return false;
}

void MCP::turn_on_led(PinNumber pin) {


    uint8_t pin_number = 255;

    switch(pin) {
        case PIN_ONE: {
            pin_number = 0;
            break;
        }
        case PIN_TWO: {
            pin_number = 1;
            break;
        }
        case PIN_THREE: {
            pin_number = 2;
            break;
        }
        case PIN_FOUR: {
            pin_number = 3;
            break;
        }
        case PIN_FIVE: {
            pin_number = 4;
            break;
        }
        case PIN_SIX: {
            pin_number = 5;
            break;
        }
        default: 
            error("No known pin number in turn_on_led");
            break;
    }

    m_current_lighted_led = pin;
    printf("Writing pin number %d \r\n", pin_number);

    set_register(MCP23S08_GPIO, ~(1 << pin_number));
}

PinNumber MCP::get_current_lighted_led() {
    return m_current_lighted_led;
}

PinNumber MCP::get_next_led(Direction direction) {
    switch (direction) {
        case FORWARDS: {
            switch(m_current_lighted_led) {
                case PIN_ONE: return PIN_TWO;
                case PIN_TWO: return PIN_THREE;
                case PIN_THREE: return PIN_FOUR;
                case PIN_FOUR: return PIN_FIVE;
                case PIN_FIVE: return PIN_SIX;
                case PIN_SIX: return PIN_ONE;
                default: 
                    error("No known pin number in turn_on_led");
                    break;
            }
        }
        case BACKWARDS: {
            switch(m_current_lighted_led) {
                case PIN_ONE: return PIN_SIX;
                case PIN_TWO: return PIN_ONE;
                case PIN_THREE: return PIN_TWO;
                case PIN_FOUR: return PIN_THREE;
                case PIN_FIVE: return PIN_FOUR;
                case PIN_SIX: return PIN_FIVE;
                default: 
                    error("No known pin number in turn_on_led");
                    break;
            }
        }
        default: error("Case for direction not handled in get_next_led");
    }
}

void MCP::reset_registers() {
    // set button to pull up
    set_register(MCP23S08_GPIO_STATE, 0b11000000);
    // set gpio pin states
    set_register(MCP23S08_IODIR, 0b11000000);
}

void MCP::set_register(uint8_t reg_address, int reg_data) {
    m_nCS = 0;
    
    m_spi.write((MCP23S08_ADDR << 1) & 0xFE);
    m_spi.write(reg_address);
    m_spi.write(reg_data);

    m_nCS = 1;
}

int MCP::get_register(uint8_t reg_address) {
    m_nCS = 0;
    
    m_spi.write((MCP23S08_ADDR << 1) | 0x01);
    m_spi.write(reg_address);
    auto reponse = m_spi.write(0x00);
    m_nCS = 1;

    return reponse;
}
