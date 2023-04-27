#include <esp_log.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <esp_rmaker_factory.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_devices.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_scenes.h>
#include <esp_rmaker_console.h>

#include <esp_rmaker_common_events.h>

#include <app_wifi.h>
#include <app_insights.h>
#include "Rainmaker.h"

static const char *TAG = "rainmaker";
// static const char* freq_param_name = "Freq";
#define freq_fine_param_name "Freq"
#define freq_coarse_param_name "Unit"
const char * freq_units[] = {"Hz", "kHz", "MHz"};
esp_rmaker_device_t *rainmaker_device;

QueueHandle_t xQueue;

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    if (strcmp(esp_rmaker_param_get_name(param), ESP_RMAKER_DEF_POWER_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %s for %s - %s",
                val.val.b? "true" : "false", esp_rmaker_device_get_name(device),
                esp_rmaker_param_get_name(param));
        if (uxQueueSpacesAvailable(xQueue) == 0)
        {
            ESP_LOGW(TAG, "Queue full, give up change!");
            return ESP_OK;
        }
        driver_info.power_state = true;
        xTaskCreate(app_driver_SPI, "SPI", 1024 * 8, NULL, 2, NULL);
        xQueueSend(xQueue, &(driver_info.power_state), portMAX_DELAY);
    }
    else if (strcmp(esp_rmaker_param_get_name(param), freq_fine_param_name) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, esp_rmaker_device_get_name(device),
                esp_rmaker_param_get_name(param));
         driver_info.freq_fine = val.val.i;
    }
    else if (strcmp(esp_rmaker_param_get_name(param), freq_coarse_param_name) == 0) {
        ESP_LOGI(TAG, "Received value = %s for %s - %s",
                val.val.s, esp_rmaker_device_get_name(device),
                esp_rmaker_param_get_name(param));
        for (int i = 0 ; i < 3 ; ++i)
        {
            if (strcmp(freq_units[i], val.val.s) == 0)
            {
                driver_info.freq_coarse = freq_units[i];
                break;
            }
        }
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}
/* Event handler for catching RainMaker events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == RMAKER_EVENT) {
        switch (event_id) {
            case RMAKER_EVENT_INIT_DONE:
                ESP_LOGI(TAG, "RainMaker Initialised.");
                break;
            case RMAKER_EVENT_CLAIM_STARTED:
                ESP_LOGI(TAG, "RainMaker Claim Started.");
                break;
            case RMAKER_EVENT_CLAIM_SUCCESSFUL:
                ESP_LOGI(TAG, "RainMaker Claim Successful.");
                break;
            case RMAKER_EVENT_CLAIM_FAILED:
                ESP_LOGI(TAG, "RainMaker Claim Failed.");
                break;
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Event: %"PRIi32, event_id);
        }
    } else if (event_base == RMAKER_COMMON_EVENT) {
        switch (event_id) {
            case RMAKER_EVENT_REBOOT:
                ESP_LOGI(TAG, "Rebooting in %d seconds.", *((uint8_t *)event_data));
                break;
            case RMAKER_EVENT_WIFI_RESET:
                ESP_LOGI(TAG, "Wi-Fi credentials reset.");
                break;
            case RMAKER_EVENT_FACTORY_RESET:
                ESP_LOGI(TAG, "Node reset to factory defaults.");
                break;
            case RMAKER_MQTT_EVENT_CONNECTED:
                ESP_LOGI(TAG, "MQTT Connected.");
                break;
            case RMAKER_MQTT_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "MQTT Disconnected.");
                break;
            case RMAKER_MQTT_EVENT_PUBLISHED:
                ESP_LOGI(TAG, "MQTT Published. Msg id: %d.", *((int *)event_data));
                break;
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Common Event: %"PRIi32, event_id);
        }
    } else if (event_base == APP_WIFI_EVENT) {
        switch (event_id) {
            case APP_WIFI_EVENT_QR_DISPLAY:
                ESP_LOGI(TAG, "Provisioning QR : %s", (char *)event_data);
                break;
            case APP_WIFI_EVENT_PROV_TIMEOUT:
                ESP_LOGI(TAG, "Provisioning Timed Out. Please reboot.");
                break;
            case APP_WIFI_EVENT_PROV_RESTART:
                ESP_LOGI(TAG, "Provisioning has restarted due to failures.");
                break;
            default:
                ESP_LOGW(TAG, "Unhandled App Wi-Fi Event: %"PRIi32, event_id);
                break;
        }
    }
    else {
        ESP_LOGW(TAG, "Invalid event received!");
    }
}

void Rainmaker_Init(void)
{
    /* Initialize Application specific hardware drivers and
     * set initial state.
     */
    esp_rmaker_console_init();
    app_driver_init();
    xQueue = xQueueCreate(1, sizeof(bool));
    // app_driver_set_state(DEFAULT_POWER);

    /* Initialize NVS. */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    /* Initialize Wi-Fi. Note that, this should be called before esp_rmaker_node_init()
     */
    app_wifi_init();

    /* Register an event handler to catch RainMaker events */
    // ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    // ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    // ESP_ERROR_CHECK(esp_event_handler_register(APP_WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    // ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_OTA_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "Switch");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }

    /* Create a Switch device.
     * You can optionally use the helper API esp_rmaker_switch_device_create() to
     * avoid writing code for adding the name and power parameters.
     */
    rainmaker_device = esp_rmaker_device_create(DEVICE_NAME, DEVICE_TYPE, NULL);

    /* Add the write callback for the device. We aren't registering any read callback yet as
     * it is for future use.
     */
    esp_rmaker_device_add_cb(rainmaker_device, write_cb, NULL);

    /* Add the standard name parameter (type: esp.param.name), which allows setting a persistent,
     * user friendly custom name from the phone apps. All devices are recommended to have this
     * parameter.
     */
    esp_rmaker_device_add_param(rainmaker_device, esp_rmaker_name_param_create(ESP_RMAKER_DEF_NAME_PARAM, "Switch"));

    /* Add the standard power parameter (type: esp.param.power), which adds a boolean param
     * with a toggle switch ui-type.
     */
    esp_rmaker_param_t *power_param = esp_rmaker_power_param_create(ESP_RMAKER_DEF_POWER_NAME, DEFAULT_POWER);
    esp_rmaker_device_add_param(rainmaker_device, power_param);

    esp_rmaker_param_t *fine_part_param = esp_rmaker_param_create(freq_fine_param_name, NULL,
            esp_rmaker_int(400), PROP_FLAG_READ | PROP_FLAG_WRITE);
    if (fine_part_param) {
        esp_rmaker_param_add_ui_type(fine_part_param, ESP_RMAKER_UI_SLIDER);
        esp_rmaker_param_add_bounds(fine_part_param, esp_rmaker_int(0), esp_rmaker_int(1000), esp_rmaker_int(1));
        esp_rmaker_device_add_param(rainmaker_device, fine_part_param);
    }
    else
    {
        abort();
    }

    esp_rmaker_param_t *coarse_part_param = esp_rmaker_param_create(freq_coarse_param_name, NULL,
            esp_rmaker_str("Hz"), PROP_FLAG_READ | PROP_FLAG_WRITE);
    if (coarse_part_param) {
        esp_rmaker_param_add_ui_type(coarse_part_param, ESP_RMAKER_UI_DROPDOWN);
        esp_rmaker_param_add_valid_str_list(coarse_part_param, freq_units, 3);
        esp_rmaker_device_add_param(rainmaker_device, coarse_part_param);
    }
    else
    {
        abort();
    }
    
    /* Assign the power parameter as the primary, so that it can be controlled from the
     * home screen of the phone apps.
     */
    esp_rmaker_device_assign_primary_param(rainmaker_device, power_param);
    esp_rmaker_device_assign_primary_param(rainmaker_device, fine_part_param);
    esp_rmaker_device_assign_primary_param(rainmaker_device, coarse_part_param);

    /* Add this switch device to the node */
    esp_rmaker_node_add_device(node, rainmaker_device);

    /* Enable OTA */
    // esp_rmaker_ota_enable_default();

    /* Enable timezone service which will be require for setting appropriate timezone
     * from the phone apps for scheduling to work correctly.
     * For more information on the various ways of setting timezone, please check
     * https://rainmaker.espressif.com/docs/time-service.html.
     */
    // esp_rmaker_timezone_service_enable();

    /* Enable scheduling. */
    // esp_rmaker_schedule_enable();

    /* Enable Scenes */
    // esp_rmaker_scenes_enable();

    /* Enable Insights. Requires CONFIG_ESP_INSIGHTS_ENABLED=y */
    // app_insights_enable();

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    /* Start the Wi-Fi.
     * If the node is provisioned, it will start connection attempts,
     * else, it will start Wi-Fi provisioning. The function will return
     * after a connection has been successfully established
     */
    err = app_wifi_start(POP_TYPE_RANDOM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start Wifi. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }
}
