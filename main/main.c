#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define WIFI_SSID "STI Student"  // Byt ut med namnet på nätverket du är på
#define WIFI_PASS "STI1924stu"  // Byt ut med ditt wifi lösenord
#define LED_GPIO_PIN 2  // Byt detta till vilken pin lampan e kopplad till

static const char *TAG = "LED_Control";

static void wifi_init(void) { //initierar wifi connection till ESP32:an
    esp_netif_init();
    esp_event_loop_create_default(); 
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
}

static esp_err_t led_on_handler(httpd_req_t *req) { //hanterar HTTP requests för att tända lampan
    gpio_set_level(LED_GPIO_PIN, 1);
    httpd_resp_send(req, "LED is ON", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t led_off_handler(httpd_req_t *req) { //hanterar HTTP requests för att släcka lampan
    gpio_set_level(LED_GPIO_PIN, 0);
    httpd_resp_send(req, "LED is OFF", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static httpd_uri_t led_on_uri = { 
    .uri       = "/led/on",
    .method    = HTTP_GET,
    .handler   = led_on_handler,
    .user_ctx  = NULL
};

static httpd_uri_t led_off_uri = {
    .uri       = "/led/off",
    .method    = HTTP_GET,
    .handler   = led_off_handler,
    .user_ctx  = NULL
};

void app_main(void) {
    // Initiera NVS minnet för att lagra WIFI-information, raderar minnet om det blir fullt eller den hittar en ny version
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initiera WiFi
    wifi_init();

    // Konfigurera LED GPIO
    gpio_reset_pin(LED_GPIO_PIN);  // Reset the pin to a known state
    gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

    // Starta HTTP servern
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &led_on_uri);
        httpd_register_uri_handler(server, &led_off_uri);
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}