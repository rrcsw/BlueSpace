#ifndef __IMG_PROC_H__
#define __IMG_PROC_H__

#include "common.h"
#include "SEEKFREE_FUN.h"    //需要调用其中的限幅保护
#include "SEEKFREE_MT9V032.h"


#define S_SIZE 600
/*--建立摄像头采集数据 结构体--*/
struct Camera_Info
{
  u8 image01[ROW * COL];    //二值化图像
  u8 smallimg[ S_SIZE ];   //压缩后的二值化图像
  
  uchar Far_Point[COL];     //每列白像素到达的的最远点（由近及远第一次有黑点的行数）
  uchar Fariest_Point;      //这帧图像白像素到达的最远点（由近及远）
  
  uchar Left_Line[ROW];     // 原始左"外"边界数据(从左到右第一次白的位置)
  uchar Right_Line[ROW];    // 原始右"外"边界数据（从右到左第一次白的位置）
  uchar Guess_Mid_Line[ROW];//根据左右边界数据预估的中线位置
  uchar Out_Width[ROW];     //“外”赛道宽度 上面两个参数之差
 
  uchar True_Jump_Num[ROW]; //原始二值化图像每行跳变次数
  uchar Jump_Location[ROW * 10];//“腐蚀滤波”后每行各个跳变点位置 一般一幅图像跳变不超过10次
  
//  uchar Jump_B_W[ROW][10];  //“腐蚀滤波”后每行“黑跳白”跳变点位置 一般一幅图像跳变不超过10次
//  uchar Jump_W_B[ROW][10];  //“腐蚀滤波”后每行“白跳黑”跳变点位置 一般一幅图像跳变不超过10次
};



//********************************函数声明*************************************//

//------二值化图像生成、优化相关------
uint8 OtsuThreshold( uint8 *image, uint16 col, uint16 row);                     //大津法阈值
void Create_Image01(uint8 *image, uint16 col, uint16 row);      //生成二值化图像 （使用大津法）
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);                        //二值化图像解压
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen);                     //二值化图像压缩
void Image_Compression(uint8 src[ROW*COL],uint8 *dest);
//-------------------------------------------------------------------


//------------边界基础处理------------
//左先左后搜索边界，记录“外”赛道宽度    成功回 1， 失败回 0
uchar Traversal_Left_Right(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);  
//先右后左搜索边界，记录“外”赛道宽度    成功回 1， 失败回 0
uchar Traversal_Right_Left(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);
//以上一次中点为基准，向两边寻找变为黑的位置，做左右赛道边界
uchar Traversal_Mid_Side(uchar i, uchar mid, uchar *data);
//以上一次左右边界为基准，向左右分别找边界 ，第一次以外边为基准
uchar Traversal_Line_Line(uchar i,  uchar *data);
//寻找 第Point列 最远点行号
uchar Limit_Scan(uchar j, uchar *data);
//腐蚀滤波，消除干扰跳变点，并返回第i行跳变次数
uchar Corrode_Filter(uchar i, uchar *data, uchar Left_Min, uchar Right_Max); 
//获取每行跳变次数及位置
uchar Get_Jump(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);          
//-------------------------------------------------------------------

char Error_Transform(uchar Data, uchar Set);


#endif