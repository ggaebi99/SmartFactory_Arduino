


#include <SoftwareSerial.h>

/**
   스마트 팩토리 컨베이어벨트 최종 예제

   DC모터 : 레일을 움직임
   서보모터 : 제품을 색상에 따라 분류
   적외선 IR센서 : 물품이 적재됨을 감지.
   컬러센서 : 색상을 아두이노에 전달. 해당 값에 따라 서보모터의 각도가 결정
   RGB : 측정된 제품의 색상을 표시

   동작 순서
   1. 컨베이어 벨트 레일 작동
   2. 적외선 센서로부터 레일에 제품이 적재됨을 감지하면 일시 정지
   3. 레일이 컬러 센서까지 움직이기 시작
   4. 컬러센서에서 정지 후 해당 제품의 색상을 분석
   5. 색상 분석에 따른 결론에 따라 R, G, B가 결정되고, 해당되는 곳으로 서보모터가 동작
   6. 레일이 다시 움직이며 서보모터가 가리키는 방향으로 제품 분류
*/

/* 라이브러리 불러오기 */
#include <Adafruit_NeoPixel.h>  // LED 모듈 라이브러리
#include <Adafruit_TCS34725.h>  // 컬러 센서 라이브러리
#include <Servo.h>              // 서보모터 라이브러리 불러오기
#include <SoftwareSerial.h>     // 블루투스에 이용되는 라이브러리
#include <Wire.h>               // I2C 통신에 사용되는 라이브러리

/* 상수 선언 : 핀 번호, 속도제어, 서보모터의 각도*/
#define PIN_DC_DIRECTION 13  // DC모터(레일) 방향을 정하는 핀(현재 B모터 사용)
#define PIN_DC_SPEED 11      // DC모터(레일) 속도를 정하는 핀(현재 B모터 사용)
#define PIN_SERVO 9          // 서보모터 연결 핀
#define PIN_LED 5            // LED 연결 핀
#define PIN_IR A0            // 적외선 IR센서 연결 핀

#define POS_RED 55   // 빨간 색 제품을 분류할 서보모터의 각도
#define POS_GREEN 85  // 초록 색 제품을 분류할 서보모터의 각도
#define POS_BLUE 2    // 파란 색 제품을 분류할 서보모터의 각도
#define NUM_PIXELS 3  // 네오픽셀의 픽셀 수: 3

/* 변수 선언 : HW객체, 측정값, 기타 변수, ...*/
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Servo servo;
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
SoftwareSerial mySerial(2,3);

uint16_t clear, red, green, blue;  // 색상 값을 저장할 unsigned short int형 변수 선언
int r, g, b, sum;                  // 색상 값을 사용하기 위한 변환값
int railSpeed = 80;               // 레일 기본 속도, 초기값은 160
bool isRailMoving = true;          // 레일 가동/정지 변수 유지 -> 블루투스로 레일 상태 조정
String ssid = "hackcenter Lab";
String PASSWORD = "@20162016@";
String host = "192.168.100.240";

void connectWifi(){
  String join ="AT+CWJAP=\""+ssid+"\",\""+PASSWORD+"\"";
  Serial.println("Connect Wifi...");
  Serial.println(join);
  mySerial.println(join);
  delay(5000);
  if(mySerial.find("OK"))
  {
    Serial.print("WIFI connect\n");
  }else
  {
   Serial.println("connect timeout\n");
  }
  delay(1000);
}
void httpclient(String char_input){
  digitalWrite(PIN_DC_DIRECTION, LOW);
  digitalWrite(PIN_DC_SPEED, LOW);
  delay(300);
  Serial.println("connect TCP...");
  mySerial.println("AT+CIPSTART=\"TCP\",\""+host+"\",80");
  for(int i=0;mySerial.available();i++){
    Serial.write(mySerial.read());
  }
  if(Serial.find("ERROR")) {    // dc모터의 방향을 제어하는 핀을 output으로 설정
  digitalWrite(PIN_DC_DIRECTION, LOW);    // dc모터의 방향을 제어하는 핀을 output으로 설정
  digitalWrite(PIN_DC_SPEED, HIGH);
    delay(50);
    return;
    }
  Serial.println("Send data...");
  String url=char_input;
  String cmd;
  cmd="GET /arduino.php?color="+url+" HTTP/1.0\r\n\r\n";
  mySerial.print("AT+CIPSEND=");
  mySerial.println(cmd.length());
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  Serial.println(cmd);
  if(mySerial.find(">"))
  {
    Serial.print(">");
  }else
  {
    Serial.println("3");
    mySerial.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    Serial.println("4");
    digitalWrite(PIN_DC_DIRECTION, LOW);    // dc모터의 방향을 제어하는 핀을 output으로 설정
  digitalWrite(PIN_DC_SPEED, HIGH);
    delay(50);
    return;
  }
  delay(100);
       
  mySerial.println(cmd);
  for(int i=0;mySerial.available();i++){
    Serial.write(mySerial.read());
  }
  delay(100);
  if(Serial.find("ERROR")) {
    digitalWrite(PIN_DC_DIRECTION, LOW);
  digitalWrite(PIN_DC_SPEED, HIGH);
    delay(50);
    return;
    }
  mySerial.println("AT+CIPCLOSE");
  delay(100);                              // 레일을 컬러센서까지 움직이기 시작
    digitalWrite(PIN_DC_DIRECTION, HIGH);
  digitalWrite(PIN_DC_SPEED, LOW);
 }  


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600); 
  mySerial.setTimeout(10000);
  /* 모터 설정 */
  connectWifi();
  pinMode(PIN_DC_SPEED, OUTPUT);
  pinMode(PIN_DC_DIRECTION, OUTPUT);     // dc모터의 방향을 제어하는 핀을 output으로 설정
  servo.attach(PIN_SERVO);               // 서보모터를 아두이노와 연결
  servo.write(2);                        // 초기 서보모터가 가리키는 각도는 0도
  delay(500);                            // 서보모터가 완전히 동작을 끝낸 후 detach를 위해 delay를 부여
  digitalWrite(PIN_DC_DIRECTION, HIGH);  // 방향은 전진. 의도한 방향과 반대일 경우 HIGH -> LOW로 변경
  digitalWrite(PIN_DC_SPEED, LOW);  // 레일 작동 시작
  servo.detach();                        // 서보모터와 아두이노 분리

  /* 적외선 센서 설정 */
  pinMode(PIN_IR, INPUT);  // 적외선 센서 핀을 INPUT으로 설정

  /* 컬러 센서 설정 */
  TCS.begin();  // 컬러 센서 작동 시작

  /* LED 모듈 설정 */
  pixels.begin();             // LED 모듈 작동 시작
  pixels.setBrightness(255);  // 밝기(0~255) 설정. 최대 밝기로 출력
}

void loop() {

    String a = "abc";
  /* 제품 적재여부 

    /* 제품 적재여부 확인 */
    if (digitalRead(PIN_IR) == HIGH) {  // 적외선 센서는 물건 감지 시 LOW값을 전달. HIGH라는 것은 감지되지 않았음
        return;                         // loop에 대한 return문장은 그 즉시 loop문을 종료하고, 처음부터 loop을 시작하게 함
    }
//  if (digitalRead(PIN_IR) == HIGH) {                          // 적외선 센서는 물건 감지 시 LOW값을 전달. HIGH라는 것은 감지되지 않았음
//    Serial.println(digitalRead(PIN_IR));
//    return;                                                 // loop에 대한 return문장은 그 즉시 loop문을 종료하고, 처음부터 loop을 시작하게 함
//  }
  digitalWrite(PIN_DC_DIRECTION, LOW);
  digitalWrite(PIN_DC_SPEED, LOW);
  servo.attach(PIN_SERVO);
  servo.write(185);                                                 // 감지되었을 때 나오는 소리를 부저에 출력
  delay(200);
  digitalWrite(PIN_DC_DIRECTION, HIGH);
  digitalWrite(PIN_DC_SPEED, LOW); 

  do {                                                        // do-while 반복문을 사용하면 최초 1회는 반드시 실행됨
    TCS.getRawData(&red, &green, &blue, &clear);            // 색상 감지 센서에서 측정 값 받아오기
    r = map(red, 0, 21504, 0, 1000);                        // 빨간색 데이터 값
    g = map(green, 0, 21504, 0, 1000);                      // 초록색 데이터 값
    b = map(blue, 0, 21504, 0, 1000);                       // 파란색 데이터 값
    sum = r + g + b;                                        // 제품의 색상이 온전하게 감지되었는지 확인하기
  } while (sum < 15);
  digitalWrite(PIN_DC_DIRECTION, LOW);
  digitalWrite(PIN_DC_SPEED, LOW);                              // 감지되었을 때 나오는 소리를 부저에 출력
  delay(500);                                                // 1.5초 간 레일 대기

  if (r > g && r > b) {                                                                 // 빨간 색 제품
    servo.attach(PIN_SERVO);                                                          // 서보모터를 아두이노와 연결
    servo.write(POS_RED);                                                             // 서보모터를 빨간 색 제품 분류방향으로 회전

    /* LED모듈에 있는 3개의 LED모듈에 모두 빨간색에 해당하는 값(#ff0000) 입력*/
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    
    a = "red";
  }

  else if (g > r && g > b) {    // 초록 색 제품
    servo.attach(PIN_SERVO);  // 서보모터를 아두이노와 연결
    servo.write(POS_GREEN);   // 서보모터를 빨간 색 제품 분류방향으로 회전

    /* LED모듈에 있는 3개의 LED모듈에 모두 초록색에 해당하는 값(#00ff00) 입력*/
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(2, pixels.Color(0, 255, 0));

    a = "green";
  }

  else {                        // 파란 색 제품
    servo.attach(PIN_SERVO);  // 서보모터를 아두이노와 연결
    servo.write(POS_BLUE);    // 서보모터를 빨간 색 제품 분류방향으로 회전

    /* LED모듈에 있는 3개의 LED모듈에 모두 초록색에 해당하는 값(#0000ff) 입력*/
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.setPixelColor(1, pixels.Color(0, 0, 255));
    pixels.setPixelColor(2, pixels.Color(0, 0, 255));

    a = "blue";
  }
  pixels.show();  // 입력한 색상 값을 LED 모듈에 출력
  
  delay(500);                                                                      // 측정 결과 표기 후 1.5초 간 레일 대기
  servo.detach();     
  String str_output = String(a);                                                                    // 서보모터와 아두이노 연결 해제
  httpclient(str_output);  // 블루투스를 통해 스마트폰에 GREEN 상품 분류된 것을 전달                                                                       // 다음 제품은 1초 뒤부터 인식 가능
  delay(250);                                              // 레일 작동 시작
}