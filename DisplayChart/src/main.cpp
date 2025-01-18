#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ui.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>



#define LVGL_TICK_PERIOD 20


/**************************DTH************************/
#define DHTPIN 26 //Gpio pin for dht 
#define DHTTYPE    DHT22   
DHT dht(DHTPIN, DHTTYPE);

/**************************DTH END************************/

// Set the screen resolution
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10]; 

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

//configure the resolution
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void DHTtimer(lv_timer_t * timer) { //Create timer for sensor

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    //   /*Temperture*/
    // Convert float to string
    String tempString = String(temperature, 1)+ "\u00B0C";  //Symbol degree
    const char* tempValue = tempString.c_str();   // Convert string to const char*
    lv_label_set_text(ui_Label3, tempValue);

    /*Humidity*/
    String humidString = String(humidity, 1)+ " %";
    const char* humiValue = humidString.c_str();
    lv_label_set_text(ui_Label4, humiValue);

    /*Chart display*/
    lv_chart_set_next_value(ui_Chart1, ui_Chart1_series_1, temperature);//Tên biểu đồ/giá trị cột trái/gía trị cột phải/Giá trị cảm biến
    lv_chart_set_next_value(ui_Chart1, ui_Chart1_series_2, humidity);
    lv_chart_refresh(ui_Chart1); /*Làm mới biểu đồ*/

}

void setup() {
    Serial.begin(115200);

    dht.begin();
    lv_init();
    
    tft.begin();
    tft.setRotation(1);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    //Initialize the display.
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    ui_init();

    lv_timer_t* timer = lv_timer_create(DHTtimer,2000,NULL); //Update sensor value after 2 second

    Serial.println("Setup done");
}

void loop() {
    lv_timer_handler();
    delay(5);
    
}


