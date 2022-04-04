#include <Servo.h>
#include <SimpleDHT.h>
#include <SoftwareSerial.h> 
#define A_1A 4 // A_1A控制A馬達的前進（HIGH）、後退（LOW）
#define A_1B 5 // A_1B控制A馬達的速度 0~255 ，LOW表示停止
#define BlueToothRXPort 0    // 藍芽TX接RX
#define BlueToothTXPort 1    // 藍芽RX接TX
#define DHTPort 2            // 溫溼度感測器接 port 2
#define MG995Port 3          // MG995伺服馬達接 port 3
Servo MG995;
SimpleDHT11 DHT;
SoftwareSerial BlueToothHC05(BlueToothRXPort, BlueToothTXPort); // RX | TX
int pos; // 轉的角度
int command = -1;
int Fanstatus = 0; // 開或是關
int turnStatus = 1; // 風扇是否旋轉
int turnPos = 0;
byte temperature = 0;
byte humidity = 0;

// 初始化
void setup() {
  Serial.begin(9600);
  Serial.println("Start!");
  pinMode(A_1A,OUTPUT);
  pinMode(A_1B,OUTPUT);
  digitalWrite(A_1A,HIGH);
  digitalWrite(A_1B,LOW);
  MG995.attach(MG995Port);
  BlueToothHC05.begin(9600);
  analogWrite(A_1B, 255);
  pos = 0;
}

void loop() {
  // 讀取藍芽指令
  if (BlueToothHC05.available()) {
    command = BlueToothHC05.read();
  }
  // 因為是ascii的編碼，所以就用0, 1代表開跟關。
  // 在ascii中0是48，1是49
  if (command == 48) { // 如果收到關(0)的指令
    Fanstatus = 0;
  } else if (command == 49) { // 如果收到開(1)的指令
    Fanstatus = 1;
  }
    
  // 顯示接收到的指令
  Serial.print("Command: ");
  Serial.println(command);
  Serial.print("FanStatus: ");
  Serial.println(Fanstatus);
    
  // 根據Fanstatus決定開或是關
  if (Fanstatus) {
    // 偵測濕度與溫度
    DHT.read(DHTPort, &temperature, &humidity, NULL);
      
    // 決定風扇角度
    if (turnStatus) {
      switch (turnPos) {
        case 0:
          pos = 0;
          turnPos = 1;
          break;
        case 1:
          pos = 90;
          turnPos = 2;
          break;
        case 2:
          pos = 180;
          turnPos = 3;
          break;
        case 3:
          pos = 90;
          turnPos = 0;
          break;
      }
    }
    MG995.write(pos);
    int fanSpeed = 200 - (int) temperature * 10;
    if (fanSpeed <= 0) {
      analogWrite(A_1B, 0); 
    } else {
      analogWrite(A_1B, fanSpeed);
    }
    Serial.print("tep: ");Serial.println((int) temperature);
    Serial.print("pos: ");Serial.println(pos);
    delay(2000);
  } else {
    analogWrite(A_1B, 255); 
    delay(2000);
  }
}
