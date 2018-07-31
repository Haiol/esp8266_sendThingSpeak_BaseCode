#include <SoftwareSerial.h>
#include <stdlib.h>
#define DEBUG true
 
// 업로드 알림 LED 설정 (아두이노 우노 Board LED)


// 자신의 thingspeak 채널의 Write API key 입력
String apiKey = "E81VK0YPQNKTHHYR";
 
SoftwareSerial esp8266(3,2); // TX/RX 설정, esp8266 객체생성
 
void setup() {
  //시리얼통신속도 9600보드레이트 설정    
  Serial.begin(9600); 
  //소프트웨어 시리얼 시작
  esp8266.begin(9600);
 
  /*AT Command 이용*/
  sendData("AT+RST\r\n", 2000, DEBUG); //reset module
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //dual mode로 설정
  sendData("AT+CWJAP=\"Test1363\",\"12345678\"\r\n", 5000, DEBUG); //사용할 공유기 설정
}
 
void loop() {
  /*// blink LED on board
 
  // LM35 값 읽음
  int val = 0;
  for(int i = 0; i < 10; i++) {
      val += analogRead(lm35Pin);   
      delay(500);
  }
 
  // LM35 값을 섭씨온도로 변환
  float temp = val*50.0f/1023.0f;
 
  // String 변환
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  
  Serial.println(strTemp);
  */
  // TCP 연결
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "api.thingspeak.com"; // api.thingspeak.com 접속 IP
  cmd += "\",80";           // api.thingspeak.com 접속 포트, 80
  esp8266.println(cmd);
   
  if(esp8266.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // GET 방식으로 보내기 위한 String, Data 설정
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=1";
  //getStr += String("1");
  getStr += "\r\n\r\n";
 
  // Send Data
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  esp8266.println(cmd);
 
  if(esp8266.find(">")){
    esp8266.print(getStr);
  }
  else{
    esp8266.println("AT+CIPCLOSE");
    // alert uesp8266
    Serial.println("AT+CIPCLOSE");
  }
    
  // Thingspeak 최소 업로드 간격 15초를 맞추기 위한 delay
  delay(16000);  
}
 
/*ESP8266의 정보를 알아내고 설정하기 위한 함수 선언*/
String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp8266.print(command); //command를 ESP8266에 보냄
  long int time=millis();
  
  while((time+timeout)>millis()){
    while(esp8266.available()){
      /*esp가 가진 데이터를 시리얼 모니터에 출력하기 위함*/
      char c=esp8266.read(); //다음 문자를 읽어옴
      response+=c;
    }
  }
  if(debug){
    Serial.print(response);
  }
  return response;
}
