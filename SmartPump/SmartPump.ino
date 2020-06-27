#include "sp_motor.h"
#include "sp_humi_sensor.h"

SPMotor motor(D1);
SPHumiSensor sensor(A0);

//////////////////// CONFIG DATA ////////////////////
uint16_t duration_minute = 1;
uint16_t humi_th         = 50; 
uint16_t humi            = 0;

//////////////////// SERVER ////////////////////
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

IPAddress apip_gate(172,19,19,19);
IPAddress apmask   (255,255,255,0);

#define APPSK  "thereisnospoon"
#define APSSID "SmartPump"

ESP8266WebServer server(80);

String html_head = "<html><head>";

String html_meta =  "<meta charset='UTF-8'>\
                    <meta name='viewport' content='width=device-width,height=device-height,initial-scale=1.0,maximum-scale=1.0,user-scalable=no'>\
                    <meta name='apple-mobile-web-app-capable' content='yes'>\
                    <style type='text/css'>body {text-align: center;}</style></head><body><h1>蹦蹦1.0</h1><h2>当前设置</h2>";
String html_tail = "<form action='/' method='get'><p>设置侦测间隔分钟（0 - 60）<input type='text' name='duration_minute'/></p>\
                    <p>设置土壤湿度阈值（20 - 100）<input type='text' name='humi_th' /></p>\
                    <input type='submit' value='提交' /></form></body></html>";

void setup_server() {
    String apssid = String(APSSID) + "-" + get_machine_id();
    String appsk  = APPSK;

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(apip_gate,apip_gate,apmask);
    WiFi.softAP(apssid,appsk);

    server.on("/", handle_root);
    server.begin();  
}

void handle_root() {
    uint16_t temp_duration_minute = duration_minute;
    uint16_t prev_duration_minute = duration_minute;
    if (server.hasArg("duration_minute")) {
        temp_duration_minute = server.arg("duration_minute").toInt();
        if (temp_duration_minute >= 0 && temp_duration_minute <=60) {
            duration_minute = temp_duration_minute;
        }
    }
    uint16_t temp_humi_th = humi_th;
    uint16_t prev_humi_th = humi_th;
    if (server.hasArg("humi_th")) {
        temp_humi_th = server.arg("humi_th").toInt();
        if (temp_humi_th >= 20 && temp_humi_th <=100) {
            humi_th = temp_humi_th;
        }
    }    
    if (humi_th != prev_humi_th || duration_minute != prev_duration_minute) {
      write_config();
    }
    String html_my_meta;
    if (server.args() > 0) {
      html_my_meta = "<meta http-equiv='refresh' content='0;url=/'>"; 
    }
    String html = html_head + html_my_meta + html_meta;
    humi = sensor.get_data(true);
    html += "<p>侦测间隔 " + String(duration_minute) + " 分钟</p>";
    html += "<p>土壤湿度阈值 " + String(humi_th) + " % </p>";
    html += "<p>当前土壤湿度 " + String(humi) + " % </p>";
    html += html_tail;
    server.send(200, "text/html",html);
}

String get_machine_id () {
    String macAddress = WiFi.macAddress();
    macAddress.replace(":","");
    return macAddress;
}

//////////////////// SAVE ////////////////////

#define E_DM_ADDR    0
#define E_TH_ADDR    4
#include <EEPROM.h>

void init_config () {
   EEPROM.begin(16);
}
void read_config() {
    unsigned int temp_duration_minute = EEPROMReadInt(E_DM_ADDR);
    if (temp_duration_minute >= 1 && temp_duration_minute <=60) {
        duration_minute = temp_duration_minute;
    }   
    unsigned int temp_humi_th = EEPROMReadInt(E_TH_ADDR);
    if (temp_humi_th >= 20 && temp_humi_th <=100) {
        humi_th = temp_humi_th;
    }    
    Serial.print("Load ");
    Serial.println(duration_minute);
}
void write_config() {
    Serial.print("Save ");
    Serial.println(duration_minute);
    EEPROMWriteInt(E_DM_ADDR,duration_minute);
    EEPROMWriteInt(E_TH_ADDR,humi_th);
    EEPROM.commit();
}

void EEPROMWriteInt(int p_address, unsigned int p_value) {
    byte lowByte = ((p_value >> 0) & 0xFF);
    byte highByte = ((p_value >> 8) & 0xFF);

    EEPROM.write(p_address, lowByte);
    EEPROM.write(p_address + 1, highByte);
}

unsigned int EEPROMReadInt(int p_address) {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

//////////////////// MAIN ////////////////////

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  init_config();
  read_config();
  motor.begin(15);
  setup_server();
}

unsigned long mTimer = 0;
unsigned long lasttime = 0;

void loop() {
  server.handleClient();
  if (lasttime == 0) {
     lasttime = millis();
  } else {
     mTimer += (millis() - lasttime);
     lasttime = millis();
     unsigned long duration = duration_minute == 0 ? 10000 : ((unsigned long)duration_minute) * 1000 * 60;
     if (mTimer > duration) {
        mTimer = 0;  
        humi = sensor.get_data(false);
        if (humi < humi_th) {
          motor.run();
        }
     }
  }
}
