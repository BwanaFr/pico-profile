#include "display.hxx"
#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

#include "lisa_logo.h"

#define OLED_I2C i2c1

bool Display::displayAddressed_ = false;             //!< Flag to know if display is addressed
uint16_t Display::frameBuffer_[FRAME_BUFFER_LEN];     //!< Buffer for DMA (use 16 bits for the stop)
uint16_t Display::cmdBuffer_[CMD_BUFFER_LEN] = {0x80, OLED_SET_COL_ADDR,
                                    0x80, 0,
                                    0x80, OLED_WIDTH - 1,
                                    0x80, OLED_SET_PAGE_ADDR,
                                    0x80, 0,
                                    0x80, OLED_NUM_PAGES | (bool_to_bit(true)<<I2C_IC_DATA_CMD_STOP_LSB)
                                    };
int Display::dmaChan_ = -1;
Display::LineData Display::lines_[2];
repeating_timer_t Display::scrollTimer_;
bool Display::textMode_ = false;
bool Display::rendering_ = false;
critical_section_t Display::critSection_;

void Display::configureI2C()
{    
    gpio_set_pulls(SCL_PIN, true, false);
    gpio_set_slew_rate(SCL_PIN, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(SCL_PIN, true);
    gpio_set_function(SCL_PIN, gpio_function::GPIO_FUNC_I2C);
    
    gpio_set_pulls(SDA_PIN, true, false);
    gpio_set_slew_rate(SDA_PIN, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(SDA_PIN, true);
    gpio_set_function(SDA_PIN, gpio_function::GPIO_FUNC_I2C);
    i2c_init(OLED_I2C, BAUD_RATE);
    printf("I2C configured\n");
}

void Display::sendCmd(uint8_t cmd)
{
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(OLED_I2C, (OLED_ADDR & OLED_WRITE_MODE), buf, 2, false);
}

void Display::dmaDone()
{
    //DMA transfer done
    if(dma_channel_get_irq1_status(dmaChan_)){
        dma_channel_acknowledge_irq1(dmaChan_);
        while(!(i2c_get_hw(OLED_I2C)->raw_intr_stat & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS)){
            tight_loop_contents();
        }        
        if(!displayAddressed_){
            displayAddressed_ = true;
            sendBuffer();
        }else{
            displayAddressed_ = false;
        }
    }
}

void Display::sendBuffer()
{
    //Waits for previous DMA transfer to be completed
    while((dmaChan_ != -1) && dma_channel_is_busy(dmaChan_))
        tight_loop_contents();
    //Prepare the I2C/DMA for addressing   
    i2c_get_hw(OLED_I2C)->enable = 0;
    i2c_get_hw(OLED_I2C)->tar = (OLED_ADDR & OLED_WRITE_MODE);
    i2c_get_hw(OLED_I2C)->enable = 1;
    if(dmaChan_ == -1) {
        dmaChan_ = dma_claim_unused_channel(true);
        irq_add_shared_handler(DMA_IRQ_1, Display::dmaDone, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY );       //Set IRQ handler for DMA transfer
        irq_set_enabled(DMA_IRQ_1, true);                          //Enable IRQ
        dma_channel_set_irq1_enabled(dmaChan_, true);
    }
    //Prepare the DMA transfer
    dma_channel_config c = dma_channel_get_default_config(dmaChan_);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(OLED_I2C, true));
    if(!displayAddressed_){
        //Configure channel to transmit adressing commands
        dma_channel_configure(
            dmaChan_,
            &c,
            &i2c_get_hw(OLED_I2C)->data_cmd,    // Destination pointer
            cmdBuffer_,                         // Source pointer
            12,                                 // Number of transfers
            true                                // Start
        );
    }else{
        //Configure channel to data adressing commands
        dma_channel_configure(
            dmaChan_,
            &c,
            &i2c_get_hw(OLED_I2C)->data_cmd,    // Destination pointer
            frameBuffer_,                       // Source pointer
            FRAME_BUFFER_LEN,                   // Number of transfers
            true                                // Start
        );
    }    
}

void Display::initDisplay()
{
    critical_section_init(&critSection_);
    configureI2C();

    sendCmd(OLED_SET_DISP | 0x00); // set display off

    /* memory mapping */
    sendCmd(OLED_SET_MEM_ADDR); // set memory address mode
    sendCmd(0x00); // horizontal addressing mode

    /* resolution and layout */
    sendCmd(OLED_SET_DISP_START_LINE); // set display start line to 0

    sendCmd(OLED_SET_SEG_REMAP | 0x01); // set segment re-map
    // column address 127 is mapped to SEG0

    sendCmd(OLED_SET_MUX_RATIO); // set multiplex ratio
    sendCmd(OLED_HEIGHT - 1); // our display is only 32 pixels high

    sendCmd(OLED_SET_COM_OUT_DIR | 0x08); // set COM (common) output scan direction
    // scan from bottom up, COM[N-1] to COM0

    sendCmd(OLED_SET_DISP_OFFSET); // set display offset
    sendCmd(0x00); // no offset

    sendCmd(OLED_SET_COM_PIN_CFG); // set COM (common) pins hardware configuration
    sendCmd(0x02); // manufacturer magic number

    /* timing and driving scheme */
    sendCmd(OLED_SET_DISP_CLK_DIV); // set display clock divide ratio
    sendCmd(0x80); // div ratio of 1, standard freq

    sendCmd(OLED_SET_PRECHARGE); // set pre-charge period
    sendCmd(0xF1); // Vcc internally generated on our board

    sendCmd(OLED_SET_VCOM_DESEL); // set VCOMH deselect level
    sendCmd(0x30); // 0.83xVcc

    /* display */
    sendCmd(OLED_SET_CONTRAST); // set contrast control
    sendCmd(0xFF);

    sendCmd(OLED_SET_ENTIRE_ON); // set entire display on to follow RAM content

    sendCmd(OLED_SET_NORM_INV); // set normal (not inverted) display

    sendCmd(OLED_SET_CHARGE_PUMP); // set charge pump
    sendCmd(0x14); // Vcc internally generated on our board

    sendCmd(OLED_SET_SCROLL | 0x00); // deactivate horizontal scrolling if set
    // this is necessary as memory writes will corrupt if scrolling was enabled

    sendCmd(OLED_SET_DISP | 0x01); // turn display on

    //Starts the timer used for scrolling text
    add_repeating_timer_ms(SCROLL_SPEED, Display::scrollTimer, nullptr, &scrollTimer_);
}

void Display::render(const uint8_t *buf)
{
    critical_section_enter_blocking(&critSection_);
    frameBuffer_[0] = 0x40;
    for(int i=1;i<FRAME_BUFFER_LEN;++i){
        frameBuffer_[i] = buf[i-1];
    }
    frameBuffer_[FRAME_BUFFER_LEN-1] |= (bool_to_bit(true)<<I2C_IC_DATA_CMD_STOP_LSB);
    critical_section_exit(&critSection_);
    sendBuffer();
}

bool Display::scrollTimer(repeating_timer_t *rt)
{
    if(textMode_){        
        for(int i=0;i<2;++i){
            if(lines_[i].len > MAX_LINE_CHARS){
                //Need to scroll
                if(lines_[i].dir){
                    lines_[i].scroll -= 1;
                    if(lines_[i].scroll < -SCROLL_WAIT){
                        lines_[i].dir = false;
                    }
                }else{
                    lines_[i].scroll += 1;
                    if((lines_[i].len - lines_[i].scroll) < MAX_LINE_CHARS){
                        lines_[i].dir = true;
                    }
                }
            }else{
                lines_[i].scroll = 0;
            }
        }
        renderText();
    }
    return true;
}

void Display::showLogo()
{
    textMode_ = false;
    render(lisa_logo);
}

void Display::demo()
{
    static int count = 0;
    printf("Display demo!\n");

    showLogo();
    sleep_ms(1000);
    char text[32];
    setText("Hello, I'm Pico-profile. The low-cost Apple Profile emulator for Lisa (and maybe Apple III)", 1);
    for(int i=0;i<100;++i){
        sprintf(text, "Line 1 : %5d", ++count);
        setText(text);
    }
}

void Display::setText(const char* txt, uint line)
{
    if(line > 1)
        return;
    textMode_ = true;
    critical_section_enter_blocking(&critSection_);
    size_t newLen = strlen(txt);
    if(newLen > lines_[line].len){
        delete[] lines_[line].text;
        lines_[line].text = new char[newLen];
    }
    memcpy(lines_[line].text, txt, newLen);
    lines_[line].len = newLen;
    lines_[line].scroll = 0;
    lines_[line].dir = true;
    critical_section_exit(&critSection_);
    renderText();
}

void Display::setText(const char* line1, const char* line2)
{
    textMode_ = true;
    critical_section_enter_blocking(&critSection_);
    for(int line=0;line<2;++line){
        const char * txt = (line == 0 ? line1 : line2);
        size_t newLen = strlen(txt);
        if(newLen > lines_[line].len){
            delete[] lines_[line].text;
            lines_[line].text = new char[newLen];
        }
        memcpy(lines_[line].text, txt, newLen);
        lines_[line].len = newLen;
        lines_[line].scroll = 0;
        lines_[line].dir = true;
    }
    critical_section_exit(&critSection_);
    //renderText();
}

void Display::renderText()
{
    uint8_t buf[OLED_BUFFER_SIZE];
    for(int i=0;i<OLED_BUFFER_SIZE;++i){
        buf[i] = 0x0;
    }
    
    uint8_t *q = buf;    
    
    for(unsigned int l=0;l<2;++l){
        q = &buf[l*256];
        int offset = lines_[l].scroll > 0 ? lines_[l].scroll : 0;
        size_t len = lines_[l].len - offset;
        if(len > MAX_LINE_CHARS){
            len = MAX_LINE_CHARS;
        }
        const char* txt = lines_[l].text + offset;
        for(unsigned int i=0;i<len;++i){
            char c = txt[i] & 0x7F;
            if(c > 0x7e){
                c = '.';
            }
            c -= 0x20;  //Font starts with space
            const uint8_t *p = &Font16x13::data[c * Font16x13::FONT_WIDTH * 2];
            if(txt[i] & 0x80){
                //Reverse
                for(int i=0;i<Font16x13::FONT_WIDTH;++i){
                    q[i] = ~p[i];
                    q[i+128] = ~p[i+Font16x13::FONT_WIDTH];
                }            
            }else{
                memcpy(q, p, Font16x13::FONT_WIDTH);
                memcpy(q+128, p+Font16x13::FONT_WIDTH, Font16x13::FONT_WIDTH);
            }
            q += Font16x13::FONT_WIDTH;
        }
    }
    render(buf);
}