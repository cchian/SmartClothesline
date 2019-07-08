#define RAINING_DRY 0
#define RAINING_POUR 1
#define RAINING_MODERATE 2
#define RAINING_HEAVY 3
#define RAINING_FLOOD 4

//----Control Command--------
#define MANUAL 0
#define AUTO 1
#define ON 2
#define OFF 3

#define STA_ON 1
#define STA_OFF 0

#define RAW 4
int previous_cmd = 0;


#include <SimpleKalmanFilter.h>
#include <MAX44009.h>
#include <driver/adc.h> //https://esp-idf.readthedocs.io/en/v2.0/api/peripherals/adc.html



SimpleKalmanFilter rainFilter(2, 2, 0.01);
SimpleKalmanFilter luxFilter(2, 2, 0.01);

MAX44009 light;

int RainState;
int rawRain;
float Lux;
int prain = 0;
int pLux;

int dLux;
int raindrop_sensor_pin;



class EnviromentSensor {
  private:
    void (*funcRainny)(int message);
    void (*funcCloudy)(int message);
  public:
    void onRainny(void (*rainnyFunc));
    void onCloudy(void (*cloudyFunc));

    int getRainStatus() {
      return RainState;
    }
    int getRawRain() {
      return rawRain;
    }
    float getLightingValue() {
      return Lux;
    }

    void setCloudyLevel(int lvl) {
      dLux = lvl;
    }
    EnviromentSensor(int raindrop_pin) {
      raindrop_sensor_pin = raindrop_pin;

      pinMode(39, INPUT);
      //     adc1_config_width(ADC_WIDTH_12Bit);
      //     adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_0db);

      Wire.begin();
      if (light.begin()) {
        Serial.println("Could not find a valid MAX44009 sensor, check wiring!");
        //while (1);
      }
    }
    void measure();
};

void EnviromentSensor::onRainny(void (*rainnyFunc)) {
  this->funcRainny = (void (*)(int))rainnyFunc;
}
void EnviromentSensor::onCloudy(void (*cloudyFunc)) {
  this->funcCloudy = (void (*)(int))cloudyFunc;
}

//การอ่านค่าจากเซ็นเซอร์ ในโค้ดด้านล่างนี้จะอ่านข้อมูลแบบ analog ทั้งหมด
//เพื่อที่จะได้ข้อมูลดิบไปใช้ในการตัดสินใจที่ถูกต้องและสามารถปรับแก้ไขกระบวนการประมวลด้วยซอฟท์แวร์ได้
//เบิ้งต้นการเซ็นเซอร์น้ำฝนให้มี 5 ระดับได้แก่
//แห้ง,ตกริน,ปานกลาง,หนัก,หนักมาก(ท่วม)
//ทากส์ที่ 2
void EnviromentSensor::measure() {
  //--------water/rain measuring--------------
  // float rainState = rainFilter.updateEstimate();  // ADC1_CHANNEL_3 คือขา VN หรือ GPI39 สังเกตดีๆ ขานี้คือ GPI ไม่มี O ซึ่งใช้งานเป็นอินพุตได้เท่านั้น ดูเพิ่มเติมได้ในแท็บ SmartClothesline
  //rawRain = adc1_get_voltage(ADC1_CHANNEL_3);
  rawRain = digitalRead(39);
  RainState = rawRain;
  //-------------lx measuring-----------------
  Lux = light.get_lux();

  if (displayPointer == DISPLAY_MAINPAGE) {
    int r = map(rawRain, 0, 1, 1, 0);
    int l = map(Lux, 0, 2937.6, 0, 100);
    //Serial.println(String(r) + "=" + String(rawRain) + "," + String(l) + "=" + String(Lux));
    mainPage(ssid, String(WiFi.localIP().toString().c_str()), (microgear.connected()) ? "Online" : "Offline", l, r);
  }

  if (Lux < dLux) {
    if (pLux != 0) {
      pLux = 0;
      funcCloudy(Lux);
    }
  } else {
    if (pLux != 1) {
      pLux = 1;
      funcCloudy(Lux);
    }
  }
  if (prain != rawRain) {
    prain = rawRain;
    funcRainny(rawRain);
  }
  //Serial.println("mesuring..."+String(rawRain));
  //grant other task to spend machine cycle.
  //ให้โอกาส Task อื่นสาร์ถใช้ทรัพยากรณ์ CPU ได้ ยิ่งหน่วงเวลามาก Task ที่มีความสำคัญต่ำกว่าก็จะยิ่งมีโอกาสหยิบใช้ cycle machine ได้มาก แต่ก็จะมีผลเสียโดยรวมกับระบบมาก
  //พึงระลึกไว้เสมอว่า การใช้คำสั่งหน่วงเวลา หากจัดการไม่ดี จะเป็นปัญหาใหญ่ต่อระบบ เพราะมันคือการทิ้ง cycle machine ไปโดยไม่ได้นำมาใช้ประโยชน์เท่าที่ควรจะเป็น
}
