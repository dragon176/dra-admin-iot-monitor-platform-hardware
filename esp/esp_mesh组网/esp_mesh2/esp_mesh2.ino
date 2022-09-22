#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

/**
 * 汇聚节点
 */
painlessMesh  mesh;

Scheduler userScheduler;

/** 
 * 数据接收
 */
void receivedCallback( uint32_t from, String &msg ) {
    //通过串口发送
    Serial.printf(msg.c_str());
}
/**
 * 新连接提示
 */
void newConnectionCallback(uint32_t nodeId) {
    //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
/**
 * 连接改变提示
 */
void changedConnectionCallback() {
    //Serial.printf("Changed connections\n");
}
/**
 * 时间调整
 */
void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
void setup() {
  Serial.begin(115200);
  delay(3000);
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );
  Serial.printf("初始化...");
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  Serial.printf("设置接受回调函数...");
  mesh.onReceive(&receivedCallback);
  
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  Serial.printf("设置完成...");
}
void loop() {

  //数据监听
  mesh.update();

}
