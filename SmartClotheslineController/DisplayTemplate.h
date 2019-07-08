#define DISPLAY_STARTUP 0
#define DISPLAY_MAINPAGE 1
#define DISPLAY_CONNECTION 2
#define DISPLAY_RAIN 3
#define DISPLAY_LIGHT 4
#define DISPLAY_INFO 5
#define DISPLAY_DEV 6

// สำหรับหรับ Display Mode ด้านล่าง ก่อนการเซ็ต displayPointer ให้เป็น DISPLAY_ANIMATION  ต้องกำหนด data ให้กับ bitmap_buffer_array ก่อน เพื่อกำหนดรูปภาพที่จะทำการแสดงแบบเคลื่อนไหว
// โดยภาพเคลื่อนไหวความเร็วการแสดงผลจะใช้ propperty ด้านล่างในการกำหนด โดยมีหน่วยเป็นเฟรมต่อวินาที
#define DISPLAY_ANIMATION 7

//ความเร็วภาพเคลื่อนไหว 2 เฟรมต่อวินาที
//ซึ่งจำนวนภาพที่จะแสดงแบบเคลื่อนไหวจะอ้างอิงจาก bitmap_buffer_array เป็นหลัก
#define ANIMATION_RATE 2

//ค่าคงที่แสดงความผิดพลาด
#define ERROR_MOTOR_CRASH 0


int displayPointer = DISPLAY_MAINPAGE;

void mainPage(String wifi, String ip, String mg, int lx, int rain) {
  //  // text display tests
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("WF:");
  for (int i = 0; i < (17-ip.length()); i++) {
    display.print(wifi[i]);
  }
  display.print("~");
  int em = 21 - 9;
  em -= ip.length();
  for (int i = 0; i < em; i++) {
    display.print(" ");
  }
  display.println(ip);


  display.println("MG:" + mg);
  display.println("Lumn");
  display.println("Watr");
  display.drawRect(28, 18, 60, 5, WHITE);
  display.drawRect(28, 26, 60, 5, WHITE);

  display.setCursor(95, 17);
  String blx = ((lx < 10) ? "  " : (lx < 100) ? " " : "") + String(lx);
  String brain = ((rain < 10) ? "  " : (rain < 100) ? " " : "") + String(rain);
  display.print(blx + " %");
  display.setCursor(95, 25);
  //display.print(brain + " %");

  //lx
  //display.fillRect(28, 18, 55, 5, WHITE);
  display.fillRect(28, 18, map(lx, 0, 100, 0, 60), 5, WHITE);


  //rain
  //display.fillRect(28, 26, 30, 5, WHITE);
  display.fillRect(28, 26, map(rain, 0, 1, 0, 60), 5, WHITE);
  display.display();
}
