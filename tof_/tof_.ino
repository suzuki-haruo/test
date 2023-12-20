#include <M5Core2.h>
#include <Wire.h>
#include "VL53L0X.h"
#define RX2 13
#define TX2 14

#define TINY_GSM_MODEM_SIM7080
#define TINY_GSM_RX_BUFFER 650
#define TINY_GSM_DEBUG SerialMon
#define MODULE_BAUD 115200

#define SerialMon Serial
#define SerialAT  Serial2

#//include <M5Core2.h>
// #include "M5GFX.h"

#include <TinyGsmClient.h>
#include <HTTPClient.h>
#include <ArduinoHttpClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

int statusCode = 0;
char payload[256];
char payloadAmbient[256];
char payloadIfttt[256];

// M5GFX display;
// M5Canvas canvas(&display);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SIMカードのAPN設定
const char apn[]           = "soracom.io";
const char gprsUser[]      = "sora";
const char gprsPass[]      = "sora";

// 接続先設定（Soracom Harvest）
// const char server[]        = "uni.soracom.io";
// const char path[]          = "/";
// const int  port            = 80;

// 接続先設定（Ambient）
// const char serverAmbient[] = "ambidata.io";
// const int  portAmbient     = 80;
// const char channelId[]     = "";                        // Ambientのチャンネル名
// const char writeKey[]      = "";                        // チャンネルのライトキー

// 接続先設定（IFTTT）
const char serverIfttt[]   = "maker.ifttt.com";
const char eventIfttt[]    = "IoT_received";        // IFTTTのイベント名
const char keyIfttt[]      = "dSRH0WG9S88PVbz4VDFryZ";                        // IFTTTのキー
const int  portIfttt       = 80;

const int sleepTime        = 30;                        // ループの間隔（60以下にしない）
const bool powerSave       = false;                     // 液晶をスリープするか

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// // 環境センサ用
// SHT3X sht30;
// QMP6988 qmp6988;

// float tmp      = 0.0;
// float hum      = 0.0;
// float pressure = 0.0;


// // 環境センサからデータを取得
// void getEnvData() {
//   pressure = qmp6988.calcPressure();
//   if (sht30.get() == 0) {
//     tmp = sht30.cTemp;
//     hum = sht30.humidity;
//   } else {
//     tmp = 0, hum = 0;
//   }
//   log("Temp: " + String(tmp) + " Humi: " + String(hum) + " Pressure: " + String(pressure));
// }


// void setup() {
//   M5.begin();
//   display.begin();
//   canvas.setColorDepth(1);
//   canvas.setFont(&fonts::efontCN_14);
//   canvas.createSprite(display.width(), display.height());
//   canvas.setTextSize(1);
//   canvas.setPaletteColor(1, GREEN);
//   canvas.setTextScroll(true);

//   // モデム起動
//   modemBoot();

//   // // 環境センサ初期化
//   // Wire.begin();
//   // qmp6988.init();
//   // log("ENVIII Unit(SHT30 and QMP6988) Initializing\n");
//   // log("---------------------------------------------");

//   // ネットワーク接続
//   connectNetwork();
// }


//
// void loop() {

//   M5.update();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // 環境センサからデータを取得
  // getEnvData();

  // 送信するデータをJSON形式で格納
  // sprintf(payload, "{\"Temp\":%2.1f,\"Hum\":%2.0f,\"Pres\":%2.0f}", distance);
  // sprintf(payloadAmbient, "{\"writeKey\":\"%s\",\"d1\":\"%2.1f\",\"d2\":\"%2.0f\",\"d3\":\"%2.0f\"}", writeKey, tmp, hum, pressure);
  // sprintf(payloadIfttt, "{ "this" : [ { "is": { "some": [ "test", "data" ] } } ] }{ "this" : [ { "is": { "some": [ "test", "data" ] } } ] }{ "this" : [ { "is": { "some": [ "test", "data" ] } } ] }", distance);
  

  // 指定の接続先にデータをPOSTする（Soracom Harvest）
  // sendData("Soracom Harvest", server, path, port, payload);

  // 指定の接続先にデータをPOSTする（Ambient）
  // char pathAmbient[256];
  // sprintf(pathAmbient, "/api/v2/channels/%s/data", channelId);
  // sendData("Ambient", serverAmbient, pathAmbient, portAmbient, payloadAmbient);




  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // 指定秒間止める
//   if (powerSave) {
//     delay(1000 * 3); // 3秒表示
//     M5.Axp.LightSleep(SLEEP_SEC(sleepTime-3)); // 指定秒液晶をスリープ
//   } else {
//     delay(1000 * sleepTime);    
//   }
// }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










// モデムの起動
void modemBoot() {
  log("---------------------------------------------");
  log("Initializing modem...\n");
  log("---------------------------------------------");

  // UARTを設定
  if (TinyGsmAutoBaud(SerialAT, MODULE_BAUD, MODULE_BAUD) == 0) {
    log("UART connect error");
  }
  log("");

  // モデムを初期化
  modem.init();
  String modemName = modem.getModemName();
  log("Modem Name: " + modemName);
  String modemInfo = modem.getModemInfo();
  log("Modem Info: " + modemInfo);
  while (!modem.getSimStatus()) {
    log("\nNot sim card");
  }
  log("");
  log("---------------------------------------------");
}


// ネットワーク接続
void connectNetwork() {
  log("GPRS connect...");
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    log("Failed");
    delay(1000 * 10);
    return;
  }
  if (modem.isGprsConnected()) {
      log("GPRS connected");
  }
  log("");

  log("Waiting for network....");
  if (!modem.waitForNetwork()) {
    log("Failed");
    delay(1000 * 10);
    return;
  }
  if (modem.isNetworkConnected()) {
    log("Network connected");
  }
  log("");

  log("---------------------------------------------");
  String ccid = modem.getSimCCID();
  log("CCID: " + ccid);

  String imei = modem.getIMEI();
  log("IMEI: " + imei);

  String imsi = modem.getIMSI();
  log("IMSI: " + imsi);

  String cop = modem.getOperator();
  log("Operator: " + cop);

  IPAddress local = modem.localIP();
  log("REMOTE IP: " + local.toString());

  int csq = modem.getSignalQuality();
  log("RSSI: " + String(csq));
  log("IP: " + local.toString());
  log("");
  log("---------------------------------------------");
  
}


// 指定の接続先にデータをPOSTする
void sendData(String cname, const char* cserver, const char* cpath, int cport, const char* cpayload) {

  HttpClient http(client, cserver, cport);

  // 接続先が有効か確認
  logline(cname + ": ");
  if (!client.connect(cserver, cport)) {
    log("Failed to Connect");
    delay(1000 * 5);  
    return;
  }
  log("");
  
  // 接続先にJSONデータをPOSTする
  int err = http.post(cpath, "application/json", cpayload);
  if (err != 0) {
    log("Failed to Connect");
  }

  // ステータスコードを取得
  statusCode = http.responseStatusCode();
  if (statusCode >= 200 && statusCode < 300) {
    log("Success! Status: " + String(statusCode));
  } else {
    log("Failed! Status: " + String(statusCode));
  }

  http.stop();
  client.stop();
  log("---------------------------------------------");

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通

// ログを出力
void log(String info) {
  SerialMon.println(info);
  // canvas.println(info);
  // canvas.pushSprite(0, 0);
}
void logline(String info) {
  SerialMon.print(info);
  // canvas.print(info);
  // canvas.pushSprite(0, 0);
}
//library   = https://github.com/pololu/vl53l0x-arduino

VL53L0X sensor;
void setup() {
  /*
  connectNetwork();
  */
  M5.begin();

  //Power chipがgpio21, gpio22, I2Cにつながれたデバイスに接続される。
  //バッテリー動作の場合はこの関数を読んでください（バッテリーの電圧を調べるらしい）
  // M5.Power.begin();
  Wire.begin();// I2C通信を開始する
  
  sensor.init();
  sensor.setTimeout(500);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(4);
}

// int time = 0;
// if(time = ){

// }


int count = 0;
void loop() {
  int distance = sensor.readRangeSingleMillimeters();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print(distance);
  M5.Lcd.print("[mm]");
    
  if (sensor.timeoutOccurred()) {
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.println("TIMEOUT");
  } else {
    
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.println(count);  

    if(distance >= 100){
      M5.Lcd.print("success!");
      // 指定の接続先にデータをPOSTする（IFTTT）
      //char pathIfttt[256];
      //sprintf(pathIfttt, "/trigger/%s/with/key/%s", eventIfttt, keyIfttt);
      //sendData("Ifttt", serverIfttt, pathIfttt, portIfttt, payloadIfttt);
    } else if(distance < 100){
    
      count = count + 1;
      if(distance)
      // break;
      // M5.Lcd.print("OMG!");
      delay(2000);
    }
  }

  delay(10);
  
}