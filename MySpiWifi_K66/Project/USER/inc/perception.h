#ifndef _perception_h
#define _perception_h

#include "headfile.h"

void Perception(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info);

uint8_t Otsu1D(uint8_t img_2d[ROW][COL]);
void LocalThresh(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 win_size, uint8 is_binary, CarInfo *car_info);

void PreprocessImage(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info);
void ExtractRawFeature(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH]);
void ProcessSpecialElement(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH]);
void UpdateCarInfo(CarInfo *car_info);


void Image_Compress(uint8 src[IMAGE_HEIGHT][IMAGE_WIDTH],uint8 *dest);
void CalcValidLine(uint8_t img60_80[IMAGE_HEIGHT][IMAGE_WIDTH]);
void CalcMiddleLineCurvity(void);//计算中线曲率

void SearchBorderLineAndMidLine(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH]);
void SearchInflexion(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH]);



int getLastLine(void);
void setLastLine(int line);

int getLongestCol(void);
void setLongestCol(int col);

int getAvailableLine(void);
void setAvailableLine(int line);

float getCurvity(void);
void setCurvity(float curv);


#endif