#ifndef DISPLAY_HXX_
#define DISPLAY_HXX_

//#define WITH_DMA 1

#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/time.h>
#include "font_6x13.hxx"

class Display {
public:

    /**
     * Initializes the display
     **/
    static void initialize();

    /**
     * Sets text to be displayed
     * @param txt String to be displayed
     * @param line Line to write text to (default 0, first line)
     **/
    static void setText(const char* txt, uint line = 0);

    /**
     * Sets text to be displayed
     * @param line1 String to be displayed on line 1
     * @param line2 String to be displayed on line 2
     **/
    static void setText(const char* line1, const char* line2);

    /**
     * Displays the LISA logo
     **/
    static void showLogo();

    /**
     * Animates the display
     * Must be called repeatly
     * @param line1Scrolled Set to true if scrolling of first line will restart
     * @param line2Scrolled Set to true if scrolling of second line will restart
     **/
    static void animateDisplay(bool& line1Scrolled, bool& line2Scrolled);

    static void busScan();
#ifdef WITH_DMA
    static void dmaDone();
#endif

private:
    Display() = delete;
    virtual ~Display() = delete;
    Display& operator=(const Display& ) = delete;

    static constexpr uint SDA_PIN = 10;                 //!< GPIO pin for SDA
    static constexpr uint SCL_PIN = 11;                 //!< GPIO pin for SCL
    static constexpr uint BAUD_RATE = 400*1000;         //!< Default baud rate    
    static constexpr uint8_t OLED_ADDR = 0x3C;          //!< OLED I2C default address

    static constexpr uint8_t OLED_HEIGHT = 32;          //!< OLED screen height in pixels
    static constexpr uint8_t OLED_WIDTH = 128;          //!< OLED screen width in pixels
    static constexpr uint8_t OLED_PAGE_HEIGHT = 8;      //!< OLED page height in pixel
    static constexpr uint8_t OLED_NUM_PAGES = OLED_HEIGHT / OLED_PAGE_HEIGHT;   //!< OLED number of memory pages
    static constexpr uint16_t OLED_BUFFER_SIZE = (OLED_NUM_PAGES * OLED_WIDTH); //!< OLED buffer size

    static constexpr uint8_t OLED_WRITE_MODE = 0xFE;    //!< OLED write mode
    static constexpr uint8_t OLED_READ_MODE = 0xFF;     //!< OLED write mode

    //OLED commands
    static constexpr uint8_t OLED_SET_CONTRAST = 0x81;
    static constexpr uint8_t OLED_SET_ENTIRE_ON = 0xA4;
    static constexpr uint8_t OLED_SET_NORM_INV = 0xA6;
    static constexpr uint8_t OLED_SET_DISP = 0xAE;
    static constexpr uint8_t OLED_SET_MEM_ADDR = 0x20;
    static constexpr uint8_t OLED_SET_COL_ADDR = 0x21;
    static constexpr uint8_t OLED_SET_PAGE_ADDR = 0x22;
    static constexpr uint8_t OLED_SET_DISP_START_LINE = 0x40;
    static constexpr uint8_t OLED_SET_SEG_REMAP = 0xA0;
    static constexpr uint8_t OLED_SET_MUX_RATIO = 0xA8;
    static constexpr uint8_t OLED_SET_COM_OUT_DIR = 0xC0;
    static constexpr uint8_t OLED_SET_DISP_OFFSET = 0xD3;
    static constexpr uint8_t OLED_SET_COM_PIN_CFG = 0xDA;
    static constexpr uint8_t OLED_SET_DISP_CLK_DIV = 0xD5;
    static constexpr uint8_t OLED_SET_PRECHARGE = 0xD9;
    static constexpr uint8_t OLED_SET_VCOM_DESEL = 0xDB;
    static constexpr uint8_t OLED_SET_CHARGE_PUMP = 0x8D;
    static constexpr uint8_t OLED_SET_HORIZ_SCROLL = 0x26;
    static constexpr uint8_t OLED_SET_SCROLL = 0x2E;
    static constexpr uint8_t OLED_SET_ENTIRE_DISP_ON = 0xA5;
    static constexpr uint8_t OLED_SET_ENTIRE_DISP_OFF = 0xA4;

    static constexpr int CMD_BUFFER_LEN = 12;
    static constexpr int FRAME_BUFFER_LEN = OLED_BUFFER_SIZE + 1;
    static constexpr size_t MAX_LINE_CHARS = OLED_WIDTH/Font16x13::FONT_WIDTH;

    static constexpr int32_t SCROLL_SPEED = 100;
    static constexpr int SCROLL_WAIT = 5;
#ifdef WITH_DMA
    static bool displayAddressed_;                     //!< Flag to know if display is addressed
    static uint16_t frameBuffer_[FRAME_BUFFER_LEN];    //!< Buffer for DMA (use 16 bits for the stop)
    static uint16_t cmdBuffer_[CMD_BUFFER_LEN];        //!< Command buffer
    static int dmaChan_;                               //!< DMA channel for transfer
#endif

    typedef struct LineData {
        char *text;         //!< Pointer to text
        size_t len;         //!< Text lenght (to avoid calling strlen)
        int scroll;         //!< Scroll amount (0 -> len)
        int scrollWait;     //!< Scroll wait state
        bool dir;           //!< Scroll direction 
        inline LineData() : text(nullptr), len(0), scroll(SCROLL_WAIT), dir(true), scrollWait(0){}
    }LineData;

    static LineData lines_[2];                         //!< Text lines
    static bool textMode_;                             //!< Text mode (to prevent scrolling)
    static absolute_time_t nextScroll_;                //!< Next scrolling time

    /**
     * Configures the I2C bus
     **/
    static void configureI2C();

    /**
     * Sends a command to the OLED display
     * @param cmd Command to be send
     **/
    static void sendCmd(uint8_t cmd);

#ifdef WITH_DMA
    /**
     * Send a complete buffer to OLED
     * Should use the DMA
     **/
    static void sendBuffer();
#endif

    /**
     * Update a portion of the display with a render area
     **/
    static void render(const uint8_t *buf);

    /**
     * Renders text lines to buffer
     **/
    static void renderText();

    /**
     * Timer callback
     **/
    static bool scrollTimer(repeating_timer_t *rt);
};


#endif