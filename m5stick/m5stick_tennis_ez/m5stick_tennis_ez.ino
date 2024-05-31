

#include <Arduino.h>
#include <M5StickCPlus.h>

#include <HTTPClient.h>

//Wifi系
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUDP.h>
//同じ階層に　WifiSecret.h　ファイルを作る
#include "WifiSecret.h"

WiFiMulti wifiMulti;
HTTPClient httpClient;

const char *ssid = ssid_taku; //WiFIのSSIDを入力
const char *pass = pass_taku; // WiFiのパスワードを入力

WiFiUDP wifiUdp; 
const char *pc_addr = pc_addr_secret;  
const int pc_port = 8888; //送信先のポート
const int my_port = 50008;  //M5自身のポート



float acc[3];        // 加速度測定値格納用（X、Y、Z）
float accOffset[3];  // 加速度オフセット格納用（X、Y、Z）
float gyro[3];       // 角速度測定値格納用（X、Y、Z）
float gyroOffset[3]; // 角速度オフセット格納用（X、Y、Z）
float roll = 0.0F;   // ロール格納用
float pitch = 0.0F;  // ピッチ格納用
float yaw = 0.0F;
const float pi = 3.14;
char buf[60];
const String dev_name = "M5StickCPlus_IMU_Data";
// 加速度、角速度　測定値取得用　関数
void readGyro() {
    M5.update();
    M5.IMU.getAccelData(&acc[0], &acc[1], &acc[2]);   // 加速度の取得
    M5.IMU.getGyroData(&gyro[0], &gyro[1], &gyro[2]); // 角速度の取得
    roll =
        atan(acc[0] / sqrt((acc[1] * acc[1]) + (acc[2] * acc[2]))) * 180 / pi;
    pitch =
        atan(acc[1] / sqrt((acc[0] * acc[0]) + (acc[2] * acc[2]))) * 180 / pi;
    yaw = atan(sqrt((acc[0] * acc[0]) + (acc[1] * acc[1])) / acc[2]) * 180 / pi;
}


/**
 * @brief 一度サーバに送信してから次に送信するまでの期間を `term` と呼ぶ
 */
int term_count = 0;

/**
 * @brief term 内で何回データを取るか
 */
const int frequency_within_term = 20;       

/**
 *  term 内でデータを取る間隔 (ms)。 frequency_within_term * delay_within_term = delay ms秒数になる
 */
const int delay_within_term = 10;

/**
 * @brief acc の値を収納(x,y,zがあるため2次元配列)
 */
float accs_1term[frequency_within_term][3];

/**
 * @brief gyro の値を収納(x,y,zがあるため2次元配列)
 */
float gyros_1term[frequency_within_term][3];

/**
 * @brief term ごとの起動からの経過時間 (ms)
 */
int timers_1term[frequency_within_term];

/**
 * @brief term の起動からの回数
 */
int counts_1term[frequency_within_term];


void get_1term_data(int frequency_within_term, int delay_within_term,
                    float accs_1term[][3], float gyros_1term[][3],
                    int *timers_1term, int *counts_1term) {
  for (int i = 0; i < frequency_within_term; i++) {
    readGyro();  // 加速度および角速度データを取得
    for (int xyz = 0; xyz < 3; xyz++) {
      accs_1term[i][xyz] = acc[xyz];
      gyros_1term[i][xyz] = gyro[xyz];
    }
    timers_1term[i] = millis();
    counts_1term[i] = i;
    delay(delay_within_term);
  }
}


//加速度・格速度データをjson形式に変換
void create_1term_json(char *json, const int frequency_within_term,
                       const int delay_within_term, const float accs_1term[][3],
                       const float gyros_1term[][3], const int *timers_1term,
                       const int *counts_1term) {

  sprintf(json, "{\"term_cnt\":%d,\"freq\":%d,\"delay\":%d,\"moments\":[", term_count++,
          frequency_within_term, delay_within_term);
  for (int i = 0; i < frequency_within_term; i++) {
    char acc[128] = {0};
    char gyro[128] = {0};
    array2json_arr(acc, accs_1term[i]);
    array2json_arr(gyro, gyros_1term[i]);
    if (i != frequency_within_term - 1) {
      sprintf(json, "%s{\"cnt\":%d,\"ms\":%d,\"acc\":%s,\"gyro\":%s},", json,
              counts_1term[i], timers_1term[i], acc, gyro);
    }else{
      sprintf(json, "%s{\"cnt\":%d,\"ms\":%d,\"acc\":%s,\"gyro\":%s}]}", json,
                  counts_1term[i], timers_1term[i], acc, gyro);
    }
  }
}


void array2json_arr(char *str, const float *array) {
    sprintf(str, "[%.5f, %.5f, %.5f]", array[0], array[1], array[2]);
}



void setup() {
  Serial.begin(115200); 
  M5.begin(); // 開始
  M5.Axp.ScreenBreath(34);
  M5.Lcd.setTextSize(2);
  M5.IMU.Init();

  // while (!M5.IMU.Init()) {
  //   delay(100);
  // }

  //wifiに接続
  wifiMulti.addAP(ssid, pass);
  M5.Lcd.print("Waiting for WiFi to");
  M5.Lcd.print(ssid); 
  while(wifiMulti.run() != WL_CONNECTED) {
    M5.Lcd.print("."); 
  }
  // Wi-Fi接続結果をシリアルモニタへ出力 
  Serial.println(""); 
  Serial.println("WiFi connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 

  //ディスプレイに表示
  M5.Lcd.println("WiFi connected");
  M5.Lcd.print("IP address = ");
  M5.Lcd.println(WiFi.localIP());
} 


void loop() { 
  M5.Lcd.setCursor(0, 0);
  /**
  * @brief 本番用 POST 消すな!
  * 
  */
  if(wifiMulti.run() == WL_CONNECTED){
    char json[2048] = {0};

    get_1term_data(frequency_within_term, delay_within_term, accs_1term,
                    gyros_1term, timers_1term,
                    counts_1term); // contains delay()

    Serial.print("Acc: [");
    for (int i = 0; i < 3; i++) {
      Serial.print(acc[i], 5);
      if (i < 2) Serial.print(", ");
    }
    Serial.println("]");

    Serial.print("Gyro: [");
    for (int i = 0; i < 3; i++) {
      Serial.print(gyro[i], 5);
      if (i < 2) Serial.print(", ");
    }
    Serial.println("]");

    create_1term_json(json, frequency_within_term, delay_within_term,
                      accs_1term, gyros_1term, timers_1term, counts_1term);

    Serial.printf("[JSON]: %s\n", json);

    //同一wifi内のIPアドレスと待ち受けるポート
    char url[1024] = {0};
        // = "http://192.168.10.9:8888/m5stick_tennis/post";
    sprintf(url, "http://%s:%d/m5stick_tennis/post",pc_addr_secret,pc_port);
    httpClient.begin(url);

    httpClient.addHeader("Content-Type", "application/json; charset=ascii");

    int httpCode = httpClient.POST((uint8_t *)json, strlen(json));

    if (httpCode == 200) {
        String response = httpClient.getString();
        Serial.printf("[HTTP RESPONSE]: %s", response);
    } else {
        Serial.printf("[HTTP ERR CODE]: %d", httpCode);
        String response = httpClient.getString();
        Serial.printf("[HTTP RESPONSE]: %s\n", response);
    }
    httpClient.end();
  }
  M5.Lcd.fillRect(0, 0, 160, 80, BLACK);
}


