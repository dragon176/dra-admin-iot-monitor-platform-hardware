#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

String ID = "176101";  //设备id
/**
 * 终端节点
 */
painlessMesh  mesh;

Scheduler userScheduler;

void sendMessage();
String output;//发送数据缓冲区
String input;//获取数据缓冲区
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

/**
 * 数据广播
 */
void sendMessage() {
  String msg = "["+ID+","+output+"]";
//  msg += mesh.getNodeId();
  Serial.println("msg="+msg);
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

/**
 * 新连接提示
 */
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
/**
 * 连接改变提示
 */
void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
}
/**
 * 时间调整
 */
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}
/**
 * 串口信息处理
 */
void serialEvent() {
  while (Serial.available()) {
    // 获取新的字符:
    char inChar = (char)Serial.read();
    //判断是否未结尾数据
    if (inChar == ':') {
      output = input;
      input = "";
      break;
    }
    // 将它加到msg中:
    input += inChar;
    
  }
}
void loop() {
  mesh.update();
}
