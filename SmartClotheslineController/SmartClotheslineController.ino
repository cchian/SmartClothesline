//  All image below created by StandAlone Maker more info please visit our page http://www.facebook.com/ArduinoProjectKits)
//  please read below text (กรุณาอ่านเพื่อทำความเข้าใจตัวอย่างโค้ดโปรแกรม)
//  Change Arduino editor font to "Courier New" for specify correct ESP32 image below.
//  ทำการเปลี่ยนฟอนต์ของ Arduino font Editor ให้เป็น "Courier New" การแสดงผลรูปภาพด้านล่างนีจะได้ไม่ผิดเพี้ยน
//  ทำตามขั้นตอนดนล่าง
//  1.Click the link at the line following File > Preferences > More preferences can be edited directly in the file.
//    This will open the .Arduino15 (or something like that) folder.
//  ๑.ไปที่เมนู File > Preferences > More preferences เพื่อเปิดไดเร็คทอรี่ที่เก็บไฟล์ตั้งค่าโปรแกรม

//  2.Exit the Arduino IDE
//  ๒.ปิดโปรแกรม Arduino IDE

//  3.Open the file preferences.txt in a text editor.
//  3.เปิดไฟล์ preference.txt ด้วยโปรแกรม notepad

//  4.Search for the line that starts with "editor.font".
//  ๔.ค้นหาบรรทัดที่มีคำว่า "editor.font".

//  5.Change the first word following the equals sign on that line to the "Courier New".
//    like this "editor.font=Courier New,plain,11"
//  ๕.เปลี่ยนชื่อฟอนต์เป็น  "Courier New" ดังตัวอย่าง เช่น "editor.font=Courier New,plain,11"

//  6.Save the file.
//  ๖.บันทึก

//  7.Start the Arduino IDE. The editor should now be using the font you chose.
//  ๘.เปิด Arduino IDE ขึ้นมาใช้งานได้ตามปรกติ


/*--------------------------setup-----------------------------------*/
//additional library need to download ดาวน์โหลดไลบารีตามลิงค์ด้านล่าง
// -MicroGear for ESP32 (https://github.com/lamloei/ESP32_Microgear)
// -SimpleKalmanFilter  (Available on Library Manager or follow this url:https://github.com/denyssene/SimpleKalmanFilter)
// -MAX44009 Luxmeter   (https://github.com/dantudose/MAX44009)
// -Button              (https://github.com/cchian/Button)
// -Adafruit_GFX        (Available on Library Manager or follow this url:https://github.com/adafruit/Adafruit-GFX-Library)
// -Adafruit_SSD1306    (Available on Library Manager or follow this url:https://github.com/adafruit/Adafruit_SSD1306)

//Smart ClothesLine using Water/Raingdrop and Ambient Sensor.
//ราวตากผ้าอัจริยะใช้เซ็นเซอร์หยดน้ำฝนและเซ็นเซอร์วัดความสว่าง

//----------------------Working Mode---------------------------
#define MODE_MANUAL 0 //พิจารณาหยดน้ำฝนอย่างเดียว
#define MODE_AUTO   1 //พิจารณาค่าความสว่างอย่างเดียว

int working_mode = 0;//เลือกการทำงานจากด้านบน
int cStatus = 2;
int pStatus = -1;

//ค่าแสงที่จะทำการทริกราวตากผ้า สามารถเทียบอ่านค่าจากแอ็พฯ
int LuxLvTrig = 1000;

//----------------------Constanst of Condition-----------------
#define LUXTRIG 1000 //ค่าความสว่างที่จะใช้เป็นเกณฑ์ในการตัดสินใจเปิด/หรือปิดราวตากผ้า
#define WATERTRIG 127 //ความความนำไฟฟ้าจากหยดน้ำฝนที่จะใช้ในการตัดสินใจเปิด/หรือปิดราวตากผ้า
//----------------------Sensor input---------------------------
//ignore below line if use i2c interface to read data from lighting sensor
//#define lighting  3 //(GPIO3) ขาสัญญาณที่จะใช้ในการอ่านค่าจากเซ็นเซอร์วัดความสว่าง

#define RAIN_DROP ADC1_CHANNEL_3 //(GPIO39) ขาสัญญาณที่จะใช้อ่านค่าจากเซ็นเซอรหยดน้ำฝน

//-----------------------Driver Type----------------------------
#define DRIVER_TYPE_RELAY 0 //รีเลย์บอร์ด 4 ch
#define DRIVER_TYPE_HB 1 //โมดุลขับมอเตอร์แบบ H-Bridge 2 ชุด
#define DRIVER_TYPE_STEPPER 2 //โมดุลขับมอเตอร์ A4988
#define MOTOR_DRIVER_TYPE DRIVER_TYPE_RELAY

#if MOTOR_DRIVER_TYPE == DRIVER_TYPE_STEPPER
#include <Stepper.h>
#endif
//---------------------Limit Switch-----------------------------
//ลิมิตสวิชใช้เป็น A3144 เซ็นเซอร์สนามแม่เหล็กเนื่องจากไม่มีหน้าสัมผัส ความเสียหายที่เกิดจากสนิมเมื่อโดนน้ำฝนจะมีน้อยกว่า
//เนื่องจากเป็นการใช้สนามแม่เหล็กมาทำเป็นสวิตช ดังนั้นจึงต้องใช้งานร่วมกับแม่เหล็กถาวร
#ifndef STEPPER
#define LIMIT_SWITCH_1   5 // GPIO5  สวิชป้องกันการเลื่อนเกินรางเลื่อน
#define LIMIT_SWITCH_2  18 // GPIO18 สวิชป้องกันการเลื่อนเกินรางเลื่อน
#define LIMIT_SWITCH_3  19 // GPIO19 สวิชป้องกันการเลื่อนเกินรางเลื่อน (ประตู)
#define LIMIT_SWITCH_4  23 // GPIO23 สวชป้องกันการเลื่อนเกินรางเลื่อน (ประตู)



//---------------------Motor controls --------------------------
#define MOTOR_1_A  25 //(GPIO25) ขาสัญญาณที่จะใช้ในการควบคุมมอเตอร์ 1A น้ำเงิน
#define MOTOR_1_B  26 //(GPIO26) ขาสัญญาณที่จะใช้ในการควบคุมมอเตอร์ 1B เขียว

#define MOTOR_2_A  33 //(GPO33) ขาสัญญาณที่จะใช้ในการควบคุมมอเตอร์ 2A เหลือง
#define MOTOR_2_B  32 //(GPO32) ขาสัญญาณที่จะใช้ในการควบคุมมอเตอร์ 2B ส้ม

#define MOTOR_1 0 //มอเตอร์ตัวที่ 1
#define MOTOR_2 1 //มอเตอร์ตัวที่ 2

#define MOTOR_STOP 0 //หยุดหมุน
#define MOTOR_DIR_CW 1 //หมุนตามเข็ม
#define MOTOR_DIR_CCW 2//หมุนทวนเข็ม

#define MOTOR_PWM 100 //12KHz ขนาด 8บิต สามารถกำหนดค่าได้ 0-255


#else
//--------------PRE DEFINE FOR STEEPER MOTOR-------------------
//-------------------------------------------------------------
//--------------------------END--------------------------------
#endif

//-----------------------incicator------------------------------
#define INDICATOR_LUMINANCE_SENSOR 17 //(GPIO17)  หลอดไฟแสดงการทำงานของเซ็นเซอร์แสง
#define INDICATOR_RAINDROP_SENSOR 16  //(GPIO16)  หลอดไฟแสดงการทำงานของเซ็นเซอร์หยดน้ำฝน
#define INDICATOR_WIPER 4             //(GPIO4) หลอดไฟแสดงการทำงานของที่ปัดน้ำฝนบนตัวเซ็นเซอร์หยดน้ำฝน



// I2C pin Available at GPIO22,GPIO21 (SCL,ADA) please see below image ESP32 pinout
//
//        _________                      __    _________ .__          __  .__                    .__  .__
//       /   _____/ _____ _____ ________/  |_  \_   ___ \|  |   _____/  |_|  |__   ____   ______ |  | |__| ____   ____
//       \_____  \ /     \\__  \\_  __ \   __\ /    \  \/|  |  /  _ \   __\  |  \_/ __ \ /  ___/ |  | |  |/    \_/ __ \     
//       /        \  Y Y  \/ __ \|  | \/|  |   \     \___|  |_(  <_> )  | |   Y  \  ___/ \___ \  |  |_|  |   |  \  ___/
//      /_______  /__|_|  (____  /__|   |__|    \______  /____/\____/|__| |___|  /\___  >____  > |____/__|___|  /\___  > /\ 
//              \/      \/     \/                      \/                      \/     \/     \/               \/     \/  \/
//
//                                                 ┌─────────────────────────────┐
//                                                 │  ┌───┐   ┌───┐   ┌───┬───┐  │
//                                                 │  │   │   │   │   │   │   │  │
//                                                 │  │   └───┘   └───┘   │   │  │
//                                                 │                      │   │  │
//                                                 │┌───────────────────────────┐│
//                                         GND  1-▓┤│ º                         │├▓-38 GND
//                                         VCC  2-▓┤│                           ░├▓-37 GPIO23
//                                    !     EN  3-▓┤│                           ░├▓-36 GPIO22            SCL
//                  ADC_H ADC1_0 SensVP GPI36   4-▓┤│                           ░├▓-35 GPIO1  TX0        CLK03
//                  ADC_H ADC1_3 SensVN GPI39   5-▓┤│                           ░├▓-34 GPIO3  RX0        CLK02
//                        ADC1_6        GPI34   6-▓┤│                           ░├▓-33 GPIO21            SDA
//                        ADC1_7        GPI35   7-▓┤│           ESP32           ░├▓-32 GPIO20
//                 XTAL32 ADC1_4     T9 GPO32   8-▓┤░                           ░├▓-31 GPIO19
//                 XTAL32 ADC1_5     T8 GPO33   9-▓┤░                           ░├▓-30 GPIO18
//                        ADC2_8  DAC_1 GPIO25 10-▓┤░                           ░├▓-29 GPIO5
//                        ADC2_9  DAC_2 GPIO26 11-▓┤░                           ░├▓-28 GPIO17 TX1
//                        ADC2_7     T7 GPIO27 12-▓┤░                           ░├▓-27 GPIO16 RX1
//                        ADC2_6     T6 GPIO14 13-▓┤░                           ░├▓-26 GPIO4  T0  ADC2_0
//                        ADC2_5     T5 GPIO12 14-▓┤░──────░░░░░░░░░░░░░░░░░────░├▓-25 GPIO0  T1  ADC2_1 CLK01
//                                                 └─────▓─▓─▓─▓─▓─▓─▓─▓─▓─▓─────┘
//                                         GND 15────────┘ │ │ │ │ │ │ │ │ └────────24 GPIO2  T2  ADC2_2
//                        ADC2_4     T4 GPIO13 16──────────┘ │ │ │ │ │ │ └──────────23 GPIO15 T3  ADC2_3
//                        SD_D2          GPIO9 17────────────┘ │ │ │ │ └────────────22 GPIO8      SD_D1
//                        SD_D3         GPIO10 18──────────────┘ │ │ └──────────────21 GPIO7      SD_D0
//                        SD_CMD        GPIO11 19────────────────┘ └────────────────20 GPIO6      SD_CLK
//
//
//
//                                                 ░    PWM Pin
//                                                 1-38 Physical pin on package
//
//                                                 Absolute MAX per pin 12mA, recommended 6mA
//                                                 VCC = 3.3V
//
//
//
//                                                                Vcc
//                                                                 ┬
//                                                                 │
//                                                        ┌─────┐  │
//                                                        │     ├──┘
//                                                        │GY-49├──► SCL/GPIO23
//                                                        │ MAX ├──► SDA/GPIO24
//                                                        │44009├──┐
//                                                        └─────┘  │
//                                                                 │
//                                                                ─┴─
//                                                                GND
//
//
//                                                 AMBIENT LIGHT SENSOR MODULE
//                                                     0.045 - 188000 Lux measurement




//
//                                                         //Raindrop Sensor
//                                                        ┌───┬───┬───┬───┬────with input pulldown
//                                                        │ │ │ │ │ │ │ │ │ │
//                                                        │ │ │ │ │ │ │ │ │ │
//                                              RAIN_DROP─┘ │ │ │ │ │ │ │ │ │
//                                                  VCC───┐ │ │ │ │ │ │ │ │ │
//                                                        │ │ │ │ │ │ │ │ │ │
//                                                        └─┴───┴───┴───┴───┘
//                                          ใช้หลักการนำไฟฟ้าของน้ำ ซึ่งจริงๆแล้วน้ำบริสุทธิ์ไม่สามารถนำไฟฟ้าได้หรือได้แต่น้อยมากๆ
//                                          หากแต่การนำไฟฟ้าที่เกิดขึ้นนั้นเกิดจากสิ่งเจือปนที่อยู่ในน้ำต่างหาก และอีกหลังการหนึ่งก็คือการทำตัวเป็นอิเล็คโทรไลต์ของน้ำ
//




//                                                     +┌────┐-              +┌────┐-        VCC
//                                                   ┌──┤ M1 ├──┐          ┌──┤ M2 ├──┐       ┬
//                                                   │  └────┘  │          │  └────┘  │       │
//                                       ┌───────────│──────────│──────────│──────────│───────┴┐
//                                       │           │          │          │          │        │
//                                       │ V+Motor   │ V+Motor  │  V+Motor │ V+Motor  │      │├─── IN1/MOTOR_1_A
//                                       │     ┬     │C   ┬     │C   ┬     │C   ┬     │C     │ │
//                                       │     │ NC┌─┴─┐  │ NC┌─┴─┐  │ NC┌─┴─┐  │ NC┌─┴─┐    │├─── IN2/MOTOR_1_B
//                                       │     └───┤RL1│  └───┤RL2│  └───┤RL3│  └───┤RL4│    │ │
//                                       │         └─┬─┘      └─┬─┘      └─┬─┘      └─┬─┘    │├─── IN3/MOTOR_2_A
//                                       │           │NO        │NO        │NO        │NO    │ │
//                                       │           └──────────┴──────────┴──────────┴─┐    │├─── IN4/MOTOR_2_B
//                                       │                                              │      │
//                                       └──────────────────────────────────────────────│──────┘
//                                                                                      │
//                                                                                     ─┴─
//                                                                                     GND
//
//                                                 การเชื่อมต่อบอร์ดรีเลย์เข้ากับมอเตอร์และ ESP32
//                           แนะนำเปลี่ยนเป็น บอร์ด H-Brigde Motor driver Module (https://www.arduitronics.com/product/983/motor-drive-module-bts7960-43a-with-h-bridge)
//                           การใช้บอร์ดดังกล่าวเราสามารถควบคุมความเร็วของมอเตอร์ได้
//                           หรืออาจใช้มอเตอร์เป็นสเตปเปอร์มอเตอร์ได้ ซึ่งมีข้อดีคือเราสามารถกำหนดตำแหน่งในการเลื่อนราวตากผ้าได้ ซึ่งเป็นหลักการเดียวกันที่ใช้กับเครื่อง cnc หรือ เครื่องปรินเตอร์ 3มิติ

//
//
//                                                Vcc                               Vcc
//                                                 ┬                                 ┬
//                                                 ├──────┐                          ├──────┐
//                                                 │     ┌┴┐RH1                      │     ┌┴┐RH1
//                                                 │IC1  │ │10k                      │IC2  │ │10k
//                                              ┌──┴──┐  └┬┘                      ┌──┴──┐  └┬┘
//                                              │A3144├───┴───►  LIMIT_SWITCH_1   │A3144├───┴───►  LIMIT_SWITCH_2
//                                              └──┬──┘                           └──┬──┘
//                                                 │                                 │
//                                                ─┴─                               ─┴─
//                                                GND                               GND
//
//                                                       A3144 Hall Effect Sensor as Limit Switch

//-------------------WiFi params--------------------------------
const char* ssid     = "Wii3";
const char* password = "";

//-------------Microgear params (https://netpie.io)-------------
#define APPID   "proj4u"
#define KEY     "h1sAAeIkJN06CbM"
#define SECRET  "CtEPQT5IamWJizjSkSoQEH5fZ"
#define ALIAS   "CtrC"

String sender = "";

#include <Preferences.h>
#include <math.h>
#include <WiFi.h>
#include <MicroGear.h>

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

Preferences preferences;

//--------------------Display-----------------------------------
#include<Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
Adafruit_SSD1306 display(-1);

#include "Bitmap.h"
#include "DisplayTemplate.h"
#include "Enviroment.h"


EnviromentSensor env(RAIN_DROP);
TaskHandle_t eventListener;
TaskHandle_t motorWorker;

boolean stringComplete  =     false;    // whether the string is complete on Serial
String inputString      =     "";       // a String to hold incoming data from Serial

String splitString(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/* If a new message arrives, do this */

//message template
// $msgCtrlC#CtrlV#1,1,rrrr,lllll
// $notifyCtrlC#CtrlV#rainy          //cloudy
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  String sMsg = "";
  for (int i = 0; i < msglen; i++)
    sMsg += (char)msg[i];
  //process.......................
  sender = splitString(sMsg, '#', 0);
  String recv = splitString(sMsg, '#', 1);
  String body = splitString(sMsg, '#', 2);

  if (recv.equals(String(ALIAS))) {
    int command = body.toInt();
    switch (command) {
      case MANUAL:
        Serial.println("Message is Manual");
        previous_cmd = command;
        working_mode = command;
        preferences.begin("smc", false);
        working_mode = preferences.getInt("mode", MANUAL);
        preferences.end();
        microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
        break;
      case AUTO:
        Serial.println("Message is Auto");
        previous_cmd = command;
        working_mode = command;
        preferences.begin("smc", false);
        working_mode = preferences.getInt("mode", AUTO);
        preferences.end();
        microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
        break;
      case ON:
        Serial.println("Message is ON");
        previous_cmd = command;
        cStatus = 1;
        microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
        break;
      case OFF:
        Serial.println("Message is OFF");
        previous_cmd = command;
        cStatus = 0;
        microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
        break;
      case RAW:
        Serial.println("Message is RAW");
        previous_cmd = command;
        microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
        break;
      default:
        Serial.println("message isn't command!");
        break;
    }
  }
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.setAlias(ALIAS);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    }
    inputString += inChar;
  }
}

void driveMotor(int motorIndex, int direction, int speed = 0) {

#ifndef STEPPER
  switch (motorIndex) {
    case 0:
      switch (direction) {
        case 0:
          ledcWrite(1, 0);
          ledcWrite(2, 0);
          //Serial.println("Motor 1 Stop");
          break;
        case 1:
          ledcWrite(1, speed);
          ledcWrite(2, 0);
          //Serial.println("Motor 1 CW");
          break;
        case 2:
          ledcWrite(1, 0);
          ledcWrite(2, speed);
          //Serial.println("Motor 1 CCW");
          break;
      }
      break;
    case 1:
      switch (direction) {
        case 0:
          ledcWrite(3, 0);
          ledcWrite(4, 0);
          //Serial.println("Motor 2 Stop");
          break;
        case 1:
          ledcWrite(3, speed);
          ledcWrite(4, 0);
          //Serial.println("Motor 2 CW");
          break;
        case 2:
          ledcWrite(3, 0);
          ledcWrite(4, speed);
          //Serial.println("Motor 2 CCW");
          break;
      }
      break;
  }
#else
  //-----------------------
#endif
}

void saveProp(const char* key, String val) {
  //  preferences.begin("smc", false);
  //  String str = String(key);
  //  String var = String(val);
  //  var.trim();
  //
  //  if (str.equals("") && val.equals("")) {
  //    preferences.clear();
  //    preferences.end();
  //    Serial.println("All configuration has been cleared!");
  //    return;
  //  }
  //
  //  str.toUpperCase();
  //  Serial.print("\n" + str);
  //
  //  if (var.equals("") || var.equals("pass")) {
  //    Serial.print(":Read<<" );
  //    Serial.println(preferences.getString(key, "NULL") );
  //  } else {
  //    Serial.print(">>OK" );
  //    preferences.putString(key, val);
  //  }
  //  preferences.end();
}

void processString(String cmd) {
  if (cmd.startsWith("ssid")) {
    cmd.replace("ssid", ""); cmd.replace(":", "");
    saveProp("ssid", cmd);
    return;
  } else  if (cmd.startsWith("pass")) {
    cmd.replace("pass:", "");
    saveProp("pass", cmd);
    return;
  } else  if (cmd.startsWith("restart")) {
    Serial.println("\nRestart>OK");
    ESP.restart();
  } else  if (cmd.startsWith("id")) {
    cmd.replace("id", ""); cmd.replace(":", "");
    saveProp("id", cmd);
    return;
  } else  if (cmd.startsWith("key")) {
    cmd.replace("key", "");
    cmd.replace(":", "");
    saveProp("key", cmd);
    return;
  } else  if (cmd.startsWith("secret")) {
    cmd.replace("secret", ""); cmd.replace(":", "");
    saveProp("secret", cmd);
    return;
  } else  if (cmd.startsWith("alias")) {
    cmd.replace("alias", ""); cmd.replace(":", "");
    saveProp("alias", cmd);
    return;
  } else if (cmd.startsWith("clear")) {
    saveProp("", "");
  }

  int command = cmd.toInt();
  switch (command) {
    case AUTO:
      Serial.println("\nAutomatic");
      previous_cmd = command;
      break;
    case MANUAL:
      Serial.println("\nManual Control");
      previous_cmd = command;
      break;
    case ON:
      Serial.println("\nON");
      previous_cmd = command;
      break;
    case OFF:
      Serial.println("\nOFF");
      previous_cmd = command;
      break;


    case RAW:
      Serial.println("\nRAW");
      previous_cmd = command;
      break;
  }
}

void rainny(int message) {
  Serial.println("Got Rain " + String(message));
  //ถ้าเซ็นเซอร์ตรวจจับฝนภูกทริก ให้เป็น 0 คือมีฝนตก
  if (working_mode == AUTO) {
    //100% ของการประมวลผลนี้ต้องทำการปิดการทำงานของราวตากผ้าทันที
    if (message == 0) {
      cStatus = 0;
    } else {
      cStatus = 1;
      //และเมื่อไม่มีฝนตก ให้พิจารณาสภาพแสงก่อนว่า มืดหรือสว่าง ตรวจเช็กการครื้มฟ้าครี้มฝน
      // if (env.getLightingValue() < LuxLvTrig) {
      //ฟ้ายังครื้มอยู่ แต่ฝนไม่ตก
      //    cStatus = 0;
      //  } else {
      //ฟ้าโปร่ง แต่ฝนไม่ตก
      //    cStatus = 1;
      //  }
    }
  }
}

void cloudy(int message) {
  Serial.println("Light:" + String(message));
  //และเมื่อไม่มีฝนตก ให้พิจารณาสภาพแสงก่อนว่า มืดหรือสว่าง ตรวจเช็กการครื้มฟ้าครี้มฝน
  if (message < LuxLvTrig) {
    //ฟ้ายังครื้ม
    //cycleOff();
  } else {
    //ฟ้าโปร่ง
    //cycleOn();
  }
}

char* unconstchar(const char* s) {
  int i;
  char* res;
  for (i = 0; s[i] != '\0'; i++) {
    res[i] = s[i];
  }
  res[i] = '\0';
  return res;
}


//for initialize motor----------
void openDoor() {
  Serial.println("Wait for LIMIT_SWITCH_4 TRIG");
  do {
    driveMotor(MOTOR_2, MOTOR_DIR_CW, MOTOR_PWM);
    //ถ้าสวิตช์ 4 ไม่ถูกกด จะติดสถานะ on คือเป็นลอจิก 1 มอเตอร์ก็จะภูกสั่งให้หมุนไปเรื่อยๆ หากสวิตช์เสียโปรแกรมก็จะไม่มีทางไปขั้นตอนต่อไปได้
  } while (digitalRead(LIMIT_SWITCH_4));
  //สังหยุดมอเตอร์เมื่อประตูเปิดออกแล้ว
  driveMotor(MOTOR_2, MOTOR_STOP);
}
void closeDoor() {
  Serial.println("Wait for LIMIT_SWITCH_3 TRIG");
  do {
    driveMotor(MOTOR_2, MOTOR_DIR_CCW, MOTOR_PWM);
    //ถ้าสวิตช์ 3 ไม่ถูกกด จะติดสถานะ on คือเป็นลอจิก 1 มอเตอร์ก็จะภูกสั่งให้หมุนไปเรื่อยๆ หากสวิตช์เสียโปรแกรมก็จะไม่มีทางไปขั้นตอนต่อไปได้
  } while (digitalRead(LIMIT_SWITCH_3));
  //สังหยุดมอเตอร์เมื่อประตูปิดแล้ว
  driveMotor(MOTOR_2, MOTOR_STOP);
}
void openClotheslineRail() {
  Serial.println("Wait for LIMIT_SWITCH_2 TRIG");
  do {
    driveMotor(MOTOR_1, MOTOR_DIR_CCW, MOTOR_PWM);
  } while (digitalRead(LIMIT_SWITCH_2));
  driveMotor(MOTOR_1, MOTOR_STOP);
}
void closeClotheslineRail() {
  Serial.println("Wait for LIMIT_SWITCH_1 TRIG");
  do {
    driveMotor(MOTOR_1, MOTOR_DIR_CW, MOTOR_PWM);
  } while (digitalRead(LIMIT_SWITCH_1));
  driveMotor(MOTOR_1, MOTOR_STOP);
}


void cycleOn() {
  microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
  openDoor();
  delay(500);
  openClotheslineRail();
  delay(500);
}
void cycleOff() {
  microgear.publish("/chat", "$msg" + String(ALIAS) + "#" + sender + "#" + String(working_mode) + "," + String(cStatus) + "," + String(env.getRawRain()) + "," + env.getLightingValue());
  closeClotheslineRail();
  delay(500);
  closeDoor();
  delay(500);
}

void homingSeek() {
  //กำหนดให้มอเตอร์ 1 ควบคุมราวตากผ้า
  //กำหนดให้มอเตอร์ 2 ควบคุมประตู ร่วมกับ ลิมิตสวิตช์ตัวที่ 3สำหรับประตูปิด และลิมิตสวิตช์ตัวที่ 4 สำหรับประตูเปิด
  //step.1 เปิดประตูก่อน มอเตอร์หมุนเปิดประตูจนกว่าลิมิตสวิตช์ 4 จะติดสถานะ off โครงสร้างต้องทำให้ประตูกดสวิตช์ 4 เมื่อประตูเปิดออกจนสุด
  Serial.println("Open the door!");
  openDoor();
  delay(1000);
  //step.2 สั่งราวตากผ้ากลับเข้าที่เก็บ โดยอ้างอิงสถานะสวิตช์ 1
  Serial.println("Close ClotheslineRail");
  closeClotheslineRail();
  delay(1000);
  //step.3 ปิดประตู(คือการทำย้อนกลับ step.1 นั่นเองเปลี่ยนสวิตช์จาเดิมเป็นสวิตช์ 4 ก็เป็นสวิตช์ 3 และหมุนมอเตอร์กลับทิศทางกัน
  Serial.println("Close the door!");
  closeDoor();
  delay(1000);
  //กำหนดสถานะการทำงาน
  Serial.println("Homing complete!");
  cStatus = STA_OFF;
  pStatus = cStatus;
  delay(1000);
}
void setup() {

  Serial.begin(115200);
  Serial.println("Starting...");

  microgear.setEEPROMOffset(512);
  uint8_t mac[6] {0xA8, 0xD9, 0xB3, 0x0D, 0xAA, 0xCE};
  WiFi.macAddress(mac);
  /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
  /* You may want to use other method that is more complicated, but provide better user experience */
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());



  pinMode(LIMIT_SWITCH_1, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_2, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_3, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_4, INPUT_PULLUP);

  ledcAttachPin(MOTOR_1_A, 1);
  ledcAttachPin(MOTOR_1_B, 2);
  ledcAttachPin(MOTOR_2_A, 3);
  ledcAttachPin(MOTOR_2_B, 4);

  ledcSetup(1, 12000, 8);
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  ledcSetup(4, 12000, 8);

  homingSeek();
  preferences.begin("smc", false);
  working_mode = preferences.getInt("mode", 0);
  preferences.end();

  Serial.println("Current Mode/STA {" + String((working_mode == AUTO) ? "AUTO" : "MANUAL") + "/" + String((cStatus == ON) ? "ON" : "OFF") + "}");
  //  String str = String(key);

  /* Add Event listeners */

  /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear);

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  Serial.println("Connecting to NETPIE...");
  microgear.connect(APPID);
  microgear.subscribe("/chat");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //ตั้งหน้าการแสดงผลที่หน้าหลัก (oled display)
  displayPointer = DISPLAY_MAINPAGE;



  //Start Multi-Tasking.... bye RTOS
  env.onRainny((void*)rainny);
  env.onCloudy((void*)cloudy);
  env.setCloudyLevel(LuxLvTrig);

  xTaskCreate(&EventListener_Task, "EventListener_Task", 2048, NULL, 3, &eventListener);
  xTaskCreate(&MotorWorker_Task, "MotorWorker_Task", 4096, NULL, 3, &motorWorker);
}

void loop() {
  if (microgear.connected()) {
    //Serial.println("connected");
    /* Call this method regularly otherwise the connection may be lost */
    microgear.loop();
    if (timer >= 1000) {
      //Serial.println("Publish...");
      /* Chat with the microgear named ALIAS which is myself */
      //microgear.chat(ALIAS, "Hello");
      timer = 0;
    }
    else timer += 100;
  } else {
    //Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
    }
    else timer += 100;
  }


//ควบคุมมอเตอร์ เพื่อปิด/เปิดราวและประตู
  if (cStatus != pStatus) {
    if (cStatus == STA_ON) {
      Serial.println("Cycle ON Clothesline Slide Rail");
      cycleOn();
    } else {
      Serial.println("Cycle OFF Clothesline Slide Rail");
      cycleOff();
    }
    Serial.println("Complete");
    pStatus = cStatus;
  }
}


//ทากส์ที่ 2
unsigned long previousMillis2 = 0;
const long interval2 = 1000;
int x = 0;
void EventListener_Task(void *p) {
  while (1) {
    unsigned long currentMillis = millis();
    //Event
    serialEvent();
    if (!inputString.equals("")) {
      processString(inputString);
      inputString = "";
      stringComplete = false;
    }
    if (currentMillis - previousMillis2 >= interval2) {
      previousMillis2 = currentMillis;
      env.measure();
    }
    delay(1);
  }
}

void MotorWorker_Task(void *p) {
  while (1) {

    delay(1);
  }
}
