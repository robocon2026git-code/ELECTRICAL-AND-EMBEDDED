#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"

#include "esp_hidh.h"
#include "esp_hid_common.h"

static const char *TAG = "PS5_HID";

static esp_bd_addr_t ps5_addr;
static bool ps5_found = false;

/* ---------------- HID CALLBACK ---------------- */

static void hidh_event_callback(void *handler_args,
                                esp_event_base_t base,
                                int32_t id,
                                void *event_data)
{
    switch (id) {

    case ESP_HIDH_OPEN_EVENT:
        ESP_LOGI(TAG, "HID Connected!");
        break;

    case ESP_HIDH_INPUT_EVENT:
        ESP_LOGI(TAG, "HID Data Received");
        break;

    case ESP_HIDH_CLOSE_EVENT:
        ESP_LOGI(TAG, "HID Disconnected");
        break;

    default:
        ESP_LOGI(TAG, "HID Event: %ld", id);
        break;
    }
}

/* ---------------- GAP CALLBACK ---------------- */

static void gap_callback(esp_bt_gap_cb_event_t event,
                         esp_bt_gap_cb_param_t *param)
{
    switch (event) {

    /* ---------- Discovery ---------- */

    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:

        if (param->disc_st_chg.state ==
            ESP_BT_GAP_DISCOVERY_STARTED) {

            ESP_LOGI(TAG, "Discovery Started");

        } else if (param->disc_st_chg.state ==
                   ESP_BT_GAP_DISCOVERY_STOPPED) {

            ESP_LOGI(TAG, "Discovery Stopped");

            if (ps5_found) {
                ESP_LOGI(TAG, "Connecting to PS5...");
                esp_hidh_dev_open(ps5_addr,
                                  ESP_HID_TRANSPORT_BT,
                                  0);
            }
        }
        break;

    case ESP_BT_GAP_DISC_RES_EVT: {

        for (int i = 0; i < param->disc_res.num_prop; i++) {

            if (param->disc_res.prop[i].type ==
                ESP_BT_GAP_DEV_PROP_EIR) {

                uint8_t len;
                uint8_t *name =
                    esp_bt_gap_resolve_eir_data(
                        (uint8_t *)param->disc_res.prop[i].val,
                        ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME,
                        &len);

                if (name) {
                    char dev_name[50];
                    memcpy(dev_name, name, len);
                    dev_name[len] = '\0';

                    if (strcmp(dev_name,
                               "DualSense Wireless Controller") == 0) {

                        ESP_LOGI(TAG, "PS5 Controller Found!");

                        memcpy(ps5_addr,
                               param->disc_res.bda,
                               ESP_BD_ADDR_LEN);

                        ps5_found = true;

                        esp_bt_gap_cancel_discovery();
                    }
                }
            }
        }
        break;
    }

    /* ---------- Secure Simple Pairing ---------- */

    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(TAG, "SSP Confirm Request");
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;

    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
            ESP_LOGI(TAG, "Authentication Success");
        else
            ESP_LOGE(TAG, "Authentication Failed");
        break;

    default:
        break;
    }
}

/* ---------------- BLUETOOTH INIT ---------------- */

static void bluetooth_init(void)
{
    esp_err_t ret;

    esp_bt_controller_config_t bt_cfg =
        BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BTDM));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_LOGI(TAG, "Bluetooth Initialized");

    /* Enable Secure Simple Pairing */
    esp_bt_sp_param_t param_type =
        ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap =
        ESP_BT_IO_CAP_IO;

    esp_bt_gap_set_security_param(param_type,
                                  &iocap,
                                  sizeof(uint8_t));

    ESP_ERROR_CHECK(esp_bt_gap_register_callback(gap_callback));

    esp_hidh_config_t config = {
        .callback = hidh_event_callback,
        .event_stack_size = 4096,
        .callback_arg = NULL,
    };

    ESP_ERROR_CHECK(esp_hidh_init(&config));

    ESP_LOGI(TAG, "HID Host Started");

    esp_bt_gap_set_device_name("ESP32_HID");

    esp_bt_gap_set_scan_mode(
        ESP_BT_CONNECTABLE,
        ESP_BT_GENERAL_DISCOVERABLE);
}

/* ---------------- APP MAIN ---------------- */

void app_main(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "NVS Initialized");

    bluetooth_init();

    vTaskDelay(pdMS_TO_TICKS(2000));

    ret = esp_bt_gap_start_discovery(
              ESP_BT_INQ_MODE_GENERAL_INQUIRY,
              30,
              0);

    if (ret == ESP_OK)
        ESP_LOGI(TAG, "Discovery Command Sent");
    else
        ESP_LOGE(TAG, "Discovery Failed: %s",
                 esp_err_to_name(ret));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}