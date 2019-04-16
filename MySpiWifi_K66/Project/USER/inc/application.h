#ifndef _application_h
#define _application_h

#include "headfile.h"

void onCreate(void);
void onStart(void);
void onStop(void);
void onDestroy(void);

void onBluetoothReceiveData(UartEvent event);
void onSpiWifiReceiveData(WifiEvent event);

void onAccelerationAndGyroscopeSensorUpdated(SensorEvent event);
void onSpeedSensorUpdated(SensorEvent event);
void onDistanceSensorUpdated(SensorEvent event);
void onCameraUpdated(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE]);
void onFPSUpdated(void);
void onKeyUp(KeyEvent event);
void onKeyDown(KeyEvent event);

void onTimer1Updated(void);
void onTimer2Updated(void);
void onTimer3Updated(void);

//void sendImageToBluetooth(void *imgaddr, uint32_t imgsize);

//自定义函数

float getMiddleLine(void);
float getLeftSpeed(void);
float getRightSpeed(void);
float getAngle(void);
float getGradient(void);

int16 getRefSpeed(void);
void setRefSpeed(int16 speed);

void initCarInfo(CarInfo *car_info);

void turnImgFlag(void);
void turnInfoFlag(void);
void ProcBTMsg(uint8 cmd, uint8 data[]);
void ProcKUMsg(uint8 type, uint8 keycode);
void ProcKDMsg(uint8 type, uint8 keycode);


uint8 IsShutDown(void);
void ShutDown(void);


#endif
