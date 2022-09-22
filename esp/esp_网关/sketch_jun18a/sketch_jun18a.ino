#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//状态define
#define STATE_INIT  0
#define STATE_WIFI_CONNECTING 1
#define STATE_MQTT_CONNECTING 2
#define STATE_WORKING 3
#define STATE_MQTT_ERROR  4
#define STATE_WIFI_ERROR  5
//参数定义
//TODO:read from serial config
const char* ssid = "dra"; // wifi名称
const char* password = "1234567890"; // wifi密码
const char* mqttServer = "xxxxx.xxxxx"; //mqtt服务服务器 地址
const int mqttPort = 1883;
const char* mqttUser = "xxx"; //mqtt服务器 用户名
const char* mqttPassword = "xxx"; // mqtt 服务器 密码
String msg = "";         // 缓存字符串 
//设置JSON编码
StaticJsonDocument<200> jsonBuffer;//声明一个JsonDocument对象，长度200
/**
 * private Long id;
    private Double temperature;
    private Double humidity;
 */
//wifi client
WiFiClient espClient;
//mqtt client
PubSubClient client(espClient);
//状态参数
int workstate = STATE_INIT;
//声明函数
void serial_data_publish();
//接受完成标志
bool stringComplete = false;
void setup()
{ 
  Serial.begin(115200);
}

/*
主要根据状态进行各种操作：
INIT状态：检查参数配置，如果配置有错误，卡死，等待配置
wifi连接状态：connectwifi，如果连接失败，到wifi error状态（进行wifi断开或其他清理工作），
再回到init状态进行参数load和检查
mqtt连接状态：connectmqtt，如果连接失败，到mqtt error状态（进行mqtt断开或其他清理工作），
5次以后wifi重连，wifi失败后重新init
working状态：mqtt连接成功，errtimes清零，loop查看是否有mqtt下发消息，检测mqtt状态
wifi状态进行状态跳转。
*/
void loop() {
  
  int ret = 0;
  int mqtt_err_times = 0;
  
//  serial_data_handle();
  String output;
  
  switch(workstate){
    case STATE_INIT:
      if((ssid== "")||(password=="")){
          //error wifi args
          break;
      }
      if((mqttServer =="")||(mqttPort == 0)||(mqttUser=="")||(mqttPassword == "")){
        //error mqtt args  
        break;
      }
      workstate = STATE_WIFI_CONNECTING;
      break;
   case STATE_WIFI_CONNECTING:
      ret = wifi_connect();
      if(ret == 0){
        //connect success
        workstate = STATE_MQTT_CONNECTING;
      }else{
        //connect error
        workstate = STATE_WIFI_ERROR;
      }
      break;
   case STATE_MQTT_CONNECTING:
      ret = mqtt_connect();
      if(ret == 0){        
        // control temp
        client.subscribe("closeValue");
        mqtt_err_times = 0;
        workstate = STATE_WORKING;
      }else{
        //connect error
        workstate = STATE_MQTT_ERROR;
      }
      break;
   case STATE_WORKING:
      client.loop();
      if (!client.connected()) {
        if(WiFi.status() != WL_CONNECTED){
          workstate = STATE_WIFI_ERROR;  
        }else{
          workstate = STATE_MQTT_ERROR;
        }
        
      }
      // 如果缓存string接收完成:
      if (stringComplete) {
        Serial.println(msg);
        serial_data_publish();
        // 清空String:
        msg = "";
        stringComplete = false;
      }
           
      //Serial.println("state:%d",client.state());
      break;
   case STATE_MQTT_ERROR:
      mqtt_err_times++;
      if(mqtt_err_times>=5){
        //reconnect wifi
        mqtt_err_times = 0;
        workstate = STATE_WIFI_ERROR;
      }else{
        workstate = STATE_MQTT_CONNECTING;
      }
      break;
   case STATE_WIFI_ERROR:
      //wifi disconnect 
      workstate = STATE_INIT;
      break;
   default:
      workstate = STATE_INIT;
      break;
  }   
}
/**
 * 连接wifi
 */
int wifi_connect(void)
{
  int retry_times = 0;
  
  WiFi.begin(ssid,password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    retry_times++;
    if(retry_times>=10){
        Serial.print("Wifi Connect timeout");
        Serial.println();
        return -1;
    }
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  return 0;
}
/**
 * 连接mqtt服务器
 */
int mqtt_connect()
{
  int retry_times = 0;
  
  client.setServer(mqttServer,mqttPort);
  client.setCallback(callback);
//  client.connect("ESP8266Client",mqttUser,mqttPassword);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client1",mqttUser,mqttPassword)) {
      /**
       * const char* mqttUser = "dra";
const char* mqttPassword = "123";

       */
      Serial.println("connected");
      
      break;
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      retry_times++;
      if(retry_times>=10){
        return -1;  
      }
      delay(500);
    }
  }
  client.subscribe("ESP32");
  return 0;
}
/**
 * 处理mqtt数据
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
  Serial.print((char)payload[i]);
  }
  Serial.println("-----------------------");
}
/**
 * 本地串口数据处理
// */
void serialEvent() {
  while (Serial.available()) {
    // 获取新的字符:
    char inChar = (char)Serial.read();
    // 将它加到msg中:
    msg += inChar;
    // 如果收到了换行符，就将一个“旗标”变量设置为true，这样loop函数就知道inputString已经缓存完成了:
    if (inChar == ']') {
      stringComplete = true;
    }
  }
}
/**
 * 数据处理并推送
 */
void serial_data_publish(){
    Serial.println("msg="+msg);
    //数据解析
    msg.replace("[","");
    msg.replace("]","");
    int i = 3;
    String output;
    Serial.println("output="+msg);
    int start = msg.length();
    while(1){
      int index = msg.lastIndexOf(",");
      if(index == -1){
        jsonBuffer["id"]=msg;
        Serial.println("sub="+msg);
        break;
      }
        
      String sub = msg.substring(index+1,start);
      start = index;
      msg = msg.substring(0,index);
      Serial.println("sub="+sub);
      //JSON序列化
      switch(i){
        case 2: jsonBuffer["temperature"]=sub;break;
        case 3: jsonBuffer["humidity"]=sub;break;
      }
      i--;
    }
    Serial.println(msg);
    Serial.print("Publish message: This ESP to IOT-JSM-ACCEPT");
    
    serializeJson(jsonBuffer, output);
    Serial.print(output);  
    client.publish("IOT-JSM-ACCEPT", 
    output.c_str()/* String to char* */
    );

    
}
