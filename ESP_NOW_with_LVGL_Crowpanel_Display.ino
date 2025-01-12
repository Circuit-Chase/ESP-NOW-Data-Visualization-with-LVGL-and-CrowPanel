#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h> 
#include <SPI.h>
#include <lvgl.h>
#include "gfx_conf.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf1[screenWidth * screenHeight / 10];
static lv_color_t disp_draw_buf2[screenWidth * screenHeight / 10];
static lv_disp_drv_t disp_drv;

// We'll store the values here, indexed by deviceId-1
static int wemosValues[4] = {0, 0, 0, 0};

// We track the last message time in a parallel array
static unsigned long lastMsgTime[4] = {0, 0, 0, 0};

// Data structure we expect to receive
typedef struct {
    uint8_t deviceId;
    int     randomValue;
} DataPackage;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

// Manual screen coordinates for 4 data points
int x_coords[4] = {-120, -120, 120, 120}; 
int y_coords[4] = {-20, 80, -20, 80}; 

/**
 * Display data on the screen 
 */
void display_data(lv_obj_t *parent, int x_coords[4], int y_coords[4]) {
    // Clear the screen
    lv_obj_clean(parent);

    // Create rectangle background for the title
    lv_obj_t *title_bg = lv_obj_create(parent);
    lv_obj_set_size(title_bg, screenWidth, 50);
    lv_obj_set_style_bg_color(title_bg, lv_color_hex(0x4169E1), 0); // Royal Blue
    lv_obj_align(title_bg, LV_ALIGN_TOP_MID, 0, 0);

    // Create a title label
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "ESP-NOW Data");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0); // Bright White
    lv_obj_set_style_text_opa(title, LV_OPA_COVER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Show the 4 data values with labels
    for (int i = 0; i < 4; i++) {
        char label_text[10];
        snprintf(label_text, sizeof(label_text), "ID %d Data", i + 1);

        // Create a small label above the value
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, label_text);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, x_coords[i], y_coords[i] - 40);

        // Create a label for the value (in bigger font)
        char value_text[10];

        // Check how long it's been since the last update
        unsigned long elapsed = millis() - lastMsgTime[i];

        if (elapsed <= 1200) {
            // If device updated in the last 1200 ms, show the value
            snprintf(value_text, sizeof(value_text), "%d", wemosValues[i]);
        } else {
            // Otherwise, show "N/A"
            snprintf(value_text, sizeof(value_text), "N/A");
        }

        lv_obj_t *value_label = lv_label_create(parent);
        lv_label_set_text(value_label, value_text);
        lv_obj_set_style_text_font(value_label, &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_color(value_label, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_text_opa(value_label, LV_OPA_COVER, 0);
        lv_obj_align(value_label, LV_ALIGN_CENTER, x_coords[i], y_coords[i]);
    }
}

/**
 * Callback function that is called when data is received via ESP-NOW.
 */
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    DataPackage dataIn;
    memcpy(&dataIn, incomingData, sizeof(dataIn));

    Serial.print("Received from deviceId: ");
    Serial.print(dataIn.deviceId);
    Serial.print("  randomValue: ");
    Serial.println(dataIn.randomValue);

    // If deviceId is in [1..4], store the values and update lastMsgTime
    if (dataIn.deviceId >= 1 && dataIn.deviceId <= 4) {
        wemosValues[dataIn.deviceId - 1] = dataIn.randomValue;
        lastMsgTime[dataIn.deviceId - 1] = millis();  // <== Track time
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("LVGL ESP-NOW Manual Coordinates Data Display");

    //--- Set up WiFi in STA mode for ESP-NOW ---
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Just to be sure we're not connected to anything

    // Print the MAC address (STA) for reference
    Serial.print("ESP32-S3 STA MAC: ");
    Serial.println(WiFi.macAddress());

    //--- Initialize ESP-NOW ---
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register the callback for incoming data
    esp_now_register_recv_cb(OnDataRecv);

    //--- Display / LVGL initialization (your code) ---
#if defined(CrowPanel_50) || defined(CrowPanel_70)
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW);
    pinMode(17, OUTPUT);
    digitalWrite(17, LOW);
    pinMode(18, OUTPUT);
    digitalWrite(18, LOW);
    pinMode(42, OUTPUT);
    digitalWrite(42, LOW);
#elif defined(CrowPanel_43)
    pinMode(20, OUTPUT);
    digitalWrite(20, LOW);
    pinMode(19, OUTPUT);
    digitalWrite(19, LOW);
    pinMode(35, OUTPUT);
    digitalWrite(35, LOW);
    pinMode(38, OUTPUT);
    digitalWrite(38, LOW);
    pinMode(0, OUTPUT); // TOUCH-CS
#endif

    tft.begin();
    tft.fillScreen(TFT_BLACK);
    delay(200);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, screenWidth * screenHeight / 10);

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.full_refresh = 1;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // Create a base screen
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_scr_load(screen);

    // Initial screen draw
    display_data(screen, x_coords, y_coords);

    Serial.println("Setup done. Waiting for data...");
}

void loop() {
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();

    // Redraw the screen every 100 miliseconds (for example)
    if (now - lastUpdate > 100) {
        display_data(lv_scr_act(), x_coords, y_coords);
        lastUpdate = now;
    }

    lv_timer_handler(); 
    delay(5);
}
