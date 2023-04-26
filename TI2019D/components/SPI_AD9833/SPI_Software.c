// #include <stdio.h>
// #include <string.h>
// #include <esp_log.h>
// #include <esp_err.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include <driver/spi_master.h>
// #include <driver/spi_common.h>
// #include <driver/gpio.h>
// // #include "SPI_AD9833.h"

// #define CS          GPIO_NUM_15
// #define FSYNC       GPIO_NUM_16
// #define SCLK        GPIO_NUM_14
// #define MY_SPI_HOST SPI3_HOST

// static spi_device_handle_t spi_dev;

// static inline void __SPI_Special_Handle(uint16_t* ctt)
// {
//     uint16_t temp = 0x0000;
//     temp = ((*ctt) & (0x00ff));
//     temp <<= 8;
//     temp |= (((*ctt) & (0xff00)) >> 8);
//     *ctt = temp;
// }

// static void SPI_Pre_Trans(spi_transaction_t* t)
// {
//     // gpio_set_level(SCLK, 1);
//     // ESP_LOGI("Pre", "TIME");
//     gpio_set_level(FSYNC, 0);
// }

// static void SPI_Post_Trans(spi_transaction_t* t)
// {
//     gpio_set_level(FSYNC, 1);
//     gpio_set_level(SCLK, 1);
// }

// void SPI_Init(void)
// {
//     static const char* TAG = "SPI init";

//     gpio_config_t gpio_cfg = {
//         .pin_bit_mask = ((1UL << FSYNC) | (1UL << SCLK)),
//         .mode = GPIO_MODE_OUTPUT,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .intr_type = GPIO_INTR_DISABLE
//     };
//     gpio_config(&gpio_cfg);
//     gpio_set_level(SCLK, 1);

//     spi_bus_config_t spi_bus_cfg;
//     memset(&spi_bus_cfg, 0, sizeof(spi_bus_cfg));
//     spi_bus_cfg.miso_io_num = -1;
//     spi_bus_cfg.mosi_io_num = 13;
//     spi_bus_cfg.sclk_io_num = SCLK;
//     spi_bus_cfg.max_transfer_sz = 64;//最大传输数据大小，单位字节，默认为4094
//     ESP_ERROR_CHECK(spi_bus_initialize(MY_SPI_HOST, &spi_bus_cfg, SPI_DMA_CH_AUTO));
//     ESP_LOGI(TAG, "spi bus init done");

//     spi_device_interface_config_t dev_cfg;
//     memset(&dev_cfg, 0, sizeof(dev_cfg));
//     dev_cfg.command_bits = 0;
//     dev_cfg.address_bits = 0;
//     dev_cfg.clock_speed_hz = 1 * 1000;// * 1000
//     dev_cfg.spics_io_num = CS;
//     dev_cfg.mode = 1;
//     dev_cfg.queue_size = 4;
//     dev_cfg.pre_cb = SPI_Pre_Trans;
//     dev_cfg.post_cb = SPI_Post_Trans;
//     ESP_ERROR_CHECK(spi_bus_add_device(MY_SPI_HOST, &dev_cfg, &spi_dev));
//     ESP_LOGI(TAG, "spi bus add device done");
// }

// void SPI_Transmit(uint16_t* content)
// {
//     spi_transaction_t spi_trans;
//     memset(&spi_trans, 0, sizeof(spi_trans));
//     __SPI_Special_Handle(content);
//     spi_trans.length = 16;
//     spi_trans.tx_buffer = content;
//     ESP_LOGI("SPI Trans", "trans:%x", *content);
//     ESP_ERROR_CHECK(spi_device_acquire_bus(spi_dev, portMAX_DELAY));
//     ESP_ERROR_CHECK(spi_device_polling_transmit(spi_dev, &spi_trans));
//     spi_device_release_bus(spi_dev);
// }

// // void SPI_Deinit(void)
// // {
    
// // }
