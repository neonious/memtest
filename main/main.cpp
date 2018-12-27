#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <vector>

using namespace std;

#define WITH_REALLOCS 1

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern "C" void app_main(void)
{
    // Keeping this stuff from esp-idf-template, just so some things are actually
    // done before doing the allocation test
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    vector<pair<void *, int> > blocks;
    int used = 0, actions = 0, waitTicks = 10000;
    int startTicks = xTaskGetTickCount();

    printf("Outputting stats every 10 seconds...\n");
    while(true)
    {
        int ticks = xTaskGetTickCount();
        int tim = ticks - startTicks;
        if(tim >= waitTicks)
        {
            waitTicks += 10000;
            printf("Block count: %d   Bytes allocated: %d   At tick: %d  Actions per ms: %.2lf\n", blocks.size(), used, tim, actions / (double)tim);
        }

        int size = rand() % 10000;
        void *data = malloc(size);
        if(!data)
        {
            if(size)
                printf("Memory allocation of %d bytes failed\n", size);
            continue;
        }

        used += size;
        blocks.push_back(pair<void *, int>(data, size));
        actions++;

        while(used >= 3500000)
        {
            int block = rand() % blocks.size();

#if WITH_REALLOCS
            if(rand() % 2)
            {
                size = rand() % 10000;
                data = realloc(blocks[block].first, size);
                if(data)
                {
                    used += size - blocks[block].second;
                    blocks[block].first = data;
                    blocks[block].second = size;
                    actions++;
                    continue;
                }
                else
                {
                    if(size)
                        printf("Memory allocation of %d bytes failed\n", size);
                    else
                        blocks[block].first = NULL;
                }
            }
#endif /* WITH_REALLOCS */

            free(blocks[block].first);
            used -= blocks[block].second;
            blocks.erase(blocks.begin() + block);
            actions++;
        }
    }
}