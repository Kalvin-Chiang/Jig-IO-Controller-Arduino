/*
 * Arduino Control Board
 * Ver:1.0
 * a.安裝 DHT Sensor Library
 *   1.草稿碼->匯入程式庫->管理程式庫,搜尋"dht",然後點選安裝即可
 */

#include <avr/wdt.h>
#include "DHT.h"

/*------------------------ DHT Sendor ------------------------*/
#define DHTPIN    A0
#define DHTTYPE   DHT22
/*--------------------------- GPIO ---------------------------*/
#define PinNumber 13
/*------------------------ Constrant -------------------------*/
#define NEWLINE   "\r\n"
#define VER       "version:1.0"
#define PROMPT    "shell>"
#define OK        "rc = 0"
#define NG        "rc = -1"
/*------------------------ Variable --------------------------*/
String szInBuf = "";            /* 存放 Serial 收到字元的緩衝區 */
bool bCommandComplete = false;  /* 判斷接收命令是否已完成的旗標 */
byte OutPinArray[] = {2,3,4,5,6,7,8,9,10,11,12,13/*,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53*/};
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  /* 初始化 Serial */
  Serial.begin(115200);
  /* 初始化 DHT Sensor */
  dht.begin();
  /* string 物件預先保留 200 bytes 的記憶體空間 */
  szInBuf.reserve(200);
  for (byte x = 0; x<sizeof(OutPinArray); x++){
    pinMode (OutPinArray[x], OUTPUT);
  }
  Serial.write(PROMPT);
  /* 啟動二秒看門狗 */
  wdt_enable(WDTO_2S);
}

void loop() {
  if (bCommandComplete) {
    /* 處理命令 */
    CommandProcess();
    /* 清除接收緩衝區 */
    szInBuf = "";
    /* 重設旗標 */
    bCommandComplete = false;
    Serial.write(NEWLINE);
    Serial.write(PROMPT);
  }
  /* 餵狗 */
  wdt_reset();
}

/* Serial 事件處理 */
void serialEvent() {
  while (Serial.available()) {
    /* 從 Serial 讀取一個字元 */
    char ch = (char)Serial.read();
    Serial.write(ch);
    /* 加入緩衝區 */
    if (ch != '\r' && ch != '\n') {
      szInBuf += ch;
    }
    
    if (ch == '\r') {
      bCommandComplete = true;
    }
  }
}

/* 命令處理 */
void CommandProcess(void)
{
  szInBuf.toUpperCase();
  if (szInBuf.equals("VER"))
  {
    Serial.write(NEWLINE);
    Serial.write(VER);
  } else if (szInBuf.equals("ALL_OFF")) {
    for (byte x = 0; x < sizeof(OutPinArray); x++) {
     digitalWrite(OutPinArray[x], LOW);
    }
    Serial.write(NEWLINE);
    Serial.write(OK);
  } else if (szInBuf.equals("TEMP")) {
    float t = dht.readTemperature();
    Serial.write(NEWLINE);
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C");
    Serial.write(NEWLINE);
    Serial.write(OK);
  } else if (szInBuf.equals("HUM")) {
    float h = dht.readHumidity();
    Serial.write(NEWLINE);
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %");
    Serial.write(NEWLINE);
    Serial.write(OK);
  } else if (szInBuf.startsWith("D") && szInBuf.indexOf("_") > 0) {
    String szPin = szInBuf.substring(1,szInBuf.indexOf("_"));
    String szSwitch = szInBuf.substring(szInBuf.indexOf("_")+1, szInBuf.length());
    int nPin = szPin.toInt();
    /* 檢查 pin 的合法性 */
    if (!OutputPinValidate(nPin)) {
      Serial.write(NEWLINE);
      Serial.write("pin ");
      Serial.print(nPin);
      Serial.write(" not defined!");
      Serial.write(NEWLINE);
      Serial.print(NG);
      return;
    }

    /* 檢查開關狀態的合法性 */
    if (!szSwitch.equals("ON") && !szSwitch.equals("OFF")) {
      Serial.write(NEWLINE);
      Serial.write("Invalid Toggle ");
      Serial.print(szSwitch);
      Serial.write(NEWLINE);
      Serial.print(NG);
      return;
    }

    if (szSwitch.equals("ON")) {
      digitalWrite(nPin, HIGH);
    } else {
      digitalWrite(nPin, LOW);
    }
    
    Serial.write(NEWLINE);
    Serial.print("Pin : ");
    Serial.print(nPin, DEC);
    Serial.print(" Status : ");
    Serial.print(szSwitch);
    Serial.write(NEWLINE);
    Serial.write(OK);
  }
}

bool OutputPinValidate(int n)
{
  for(int i = 0; i < sizeof(OutPinArray); i++) {
    if (n == OutPinArray[i]) {
      return true;
    }
  }
  return false;
}
