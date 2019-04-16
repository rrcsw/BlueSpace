#include "img_proc.h"

struct Camera_Info camera_info;  //全局变量结构体，用于extern语法调用

extern uint8 image[ROW][COL];      //总钻风内部使用灰度图像位置

//camera.c自身调用的全局变量
uint8 colour[2] = {255, 0}; //二值化后的颜色
uint8 black = 1,white = 0;  //0 和 1 分别对应的颜色


void struct_clear(unsigned char *pta, int size )
{
    while(size>0)
    {
        *pta++ = 0;
        size --;
    }
}


int32 range_protect(int32 duty, int32 min, int32 max)//限幅保护
{
    if (duty >= max)
    {
        return max;
    }
    if (duty <= min)
    {
        return min;
    }
    else
    {
        return duty;
    }
}


uint8 Get_Camera_Info (uint8 *image )
{ 
  u8 i=0, j=0;
  u8 temp_far=79;  //用于寻找、记录整图最远点

  //结构体内部元素初始化清零
  struct_clear(( unsigned char *)&camera_info,sizeof(camera_info));
  
  //用大津法生成二值化图像 存入 camera_info.image01[ROW][COL]                        经历两次遍历
  Create_Image01(image, COL, ROW);
  //对二值化图像进行压缩 存入   camera_info.smallimg                                 经历一次遍历
//  img_recontract(camera_info.smallimg, camera_info.image01,COL * ROW /8);
  Image_Compression(camera_info.image01, camera_info.smallimg);
  
  camera_info. Fariest_Point = ROW - 1;  //先将最远点初始化为最近
  
  for(j = 0; j < COL ;j++)                                                      //经历一次遍历
  {
    temp_far = Limit_Scan(j, camera_info.image01);   //找每列从近往远第一次变黑的点的位置
    if(temp_far < camera_info. Fariest_Point)
    {
      camera_info. Fariest_Point = temp_far;         //更新全图最远黑点位置
    }
  }
  
  //给temp_far 的新任务  记录本图像扫描边界的最远行位置
  if(camera_info. Fariest_Point <= ROW/4)   //如果整图赛道视野开阔，取近40行 扫描左右侧边界
    temp_far = ROW/4;
  else 
    temp_far = camera_info. Fariest_Point; //如果整图赛道集中在靠下方，取全图最远点行数 扫描左右侧边界
  
  
  for(i = ROW - 1; i >= temp_far ;i--)                                          //最多 (2/3)*3 次遍历
  {
//    Traversal_Left_Right(i, camera_info.image01,1, COL - 1);    //最近行到最远行记录左右“外”边界 并 记录“赛道宽度”
    Traversal_Line_Line(i, camera_info.image01);    //以上一次左右边界为基准，向左右分别找边界 ，第一次以外边为基准
    //最近行 取中值为mid
//    if(i == ROW - 1)
//      Traversal_Mid_Side(i, COL/2, camera_info.image01);
//    else
//      Traversal_Mid_Side(i, camera_info.Out_Width[i+1], camera_info.image01);
    
//    if(camera_info.Out_Width[i] < COL)  
    if(camera_info.Left_Line[i]<camera_info.Right_Line[i])      //上一行成功执行
    {
      //根据得到的左右“外”边界进行腐蚀滤波，记录该行真实跳变次数  True_Jump_Num[i]
      camera_info.True_Jump_Num[i] = Corrode_Filter(i, camera_info.image01, camera_info.Left_Line[i], camera_info.Right_Line[i]);
      //记录第i行腐蚀滤波后，发生跳变的对应列数
      Get_Jump(i, camera_info.image01, camera_info.Left_Line[i], camera_info.Right_Line[i]);
    }
  } 
}



uint8 OtsuThreshold( uint8 *image, uint16 col, uint16 row)
{
    uint16 OtsuThresholdOut;
    uint16 GrayList[256]= {0};  //灰度值个数统计数组
    uint16 i, pixelSum = col * row;   //图像大小
    uint16 Camera_Graylevel=0;
    uint16 Graylevel=0;         //灰度值
    uint16 cnts=0,cnt=0;
    float avfor=0,avback=0;
    float total=0,total_low=0,total_high=0,Variance=0,VarianceMax=0;
    //清空直方图
    for(Graylevel=0; Graylevel<256; Graylevel++)
    {
        GrayList[Graylevel]=0;
    }
    //计算直方图

    for(i=0; i<pixelSum; i++)
    {
        Camera_Graylevel=*(image+i);//获取灰度值
        GrayList[Camera_Graylevel]+=1;//直方图统计图像所有像素点
        total+=Camera_Graylevel;//total计算图像的灰度值总和
    }

    for(Graylevel=0; Graylevel<256; Graylevel++)
    {
        cnt=GrayList[Graylevel];//当前灰度值的计数数量
        if(cnt==0)   continue;
        total_low+=cnt*Graylevel;//计算灰度值从0到当前值的像素点灰度值的总和
        cnts+=cnt;//计算灰度值从0到当前值的像素点数目的总和

        if(cnts==pixelSum)  break;//统计完所有像素点，之后的灰度值已经没有任何像素点符合，计算完毕，退出

        total_high=total-total_low;//背景灰度=灰度总和-前景灰度(还没有归一化)
        avfor=(int)(total_low/cnts);//前景归一化

        avback=(int)(total_high/(pixelSum-cnts));//背景归一化
        Variance=(avback-avfor)*(avback-avfor)*(cnts*pixelSum-cnts*cnts)/(pixelSum*pixelSum);//计算方差

        if(VarianceMax<Variance)
        {
            VarianceMax=Variance;
            OtsuThresholdOut=Graylevel;//方差最大的灰度值即为大津阈值
        }
    }
    return OtsuThresholdOut;
}


/*!      生成二值化图像 到image01[ROW][COL]里去
*  @brief      生成二值化图像
*  @param      image           原图像地址
*  @param      image01         二值化图像地址
*  @param      col             图像col
*  @param      row             图像row
*  @since      v5.0            image_threshold = OtsuThreshold(image[0],COL,ROW);
*  Sample usage:
*/
void Create_Image01(uint8 *image,  uint16 col, uint16 row)
{
  uint8 i,j;                      //计数器
  uint8 *pic;                   //遍历图像点
  uint8 image_threshold;        //二值化阈值
  
  //大津法计算二值化阈值
  image_threshold = OtsuThreshold( image, col, row ); 
  //指针指向原图像首地址
  pic = &image[0];
  //生成二值化图像
  for(i = 0; i < row; i++)
  {
    for(j = 0; j<col; j++)
    {
      if( pic[i * col + j] > image_threshold )    
        camera_info.image01[i*COL+j] = colour[white];
//        image[i*COL+j] = 255;
      else                      
        camera_info.image01[i*COL+j] = colour[black];
//        image[i*COL+j] = 0;
    }
  }
}


/*!      二值化图像解压
*  @brief      二值化图像解压（空间 换 时间 解压）
*  @param      dst             图像解压目的地址
*  @param      src             图像解压源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  while(srclen --)
  {
    tmpsrc = *src++;
    *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
    *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
  }
}

void Image_Compression(uint8 src[ROW * COL],uint8 *dest)
{
    int count = 0;
    uint8 temp = 0;
    for(int i = 0; i < ROW; i++)
    {
        for(int j = 0; j < COL; j++)
        {
            if(src[i*COL+j] != colour[black])
            {
                temp |=  1 << (7 - count);
            }
            count++;
            if(count >= 8)
            {
                count = 0;
                *dest++ = temp;
                temp = 0;
            }
        }
    }
}


/*!      二值化图像压缩
*  @brief      二值化图像压缩（空间 换 时间 压缩）
*  @param      dst             图像压缩目的地址
*  @param      src             图像压缩源地址
*  @param      srclen          二值化图像的占用空间大小
*  @since      v5.0            img_extract(img, imgbuff,CAMERA_SIZE);
*  Sample usage:
*/
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen)
{
  uint8 tmpsrc;
  uint8 i, j, k;
  uint32 Count = 0;
  
  for (i = 0; i < ROW/2; i++)
  {
    for (j = 0; j < COL/8; j++)
    {
      Count = (i*2+1)*COL+j*8;
      for (k = 0; k < 8; k++)
      {
        tmpsrc <<= 1;
        if (!dst[Count++])
        {
          tmpsrc |= 1;
        }
      }
      *src++ = tmpsrc;
    }
  }
}


/*      腐蚀滤波 
*	腐蚀滤波                                                      
*	说明：将干扰滤除，并统计黑白跳变点数量，用于起跑线检测
*       i                要处理的行数（1 代表 第1行）
*       *data            待处理图像 
*       Left_Min         最左侧列数
*       Right_Max        最右侧列数   
*/
uchar Corrode_Filter(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;
  //  uchar Test_Jump = 0;
  uchar White_Flag = 0; //白点找到标志位
  uchar Jump_Count = 0;	// 跳变点计数
  
  if(Left_Min>=Right_Max)
    return 0;
  
  Right_Max = range_protect(Right_Max, 1, COL - 1);	// 保留右侧部分区域，防止溢出
  
  for (j = Left_Min; j <= Right_Max; j++)	// 从左向右扫描，方向不影响结果
  {
    if (!White_Flag)	// 先查找白点，只滤黑点，不滤白点
    {
      if (data[i * COL + j] == colour[white])	// i行j列 检测到白点
      {
        White_Flag = 1;	// 开始找黑点
      }
    }
    else
    {
      if (data[i * COL + j] == colour[black])	// 检测到黑点
      {
        Jump_Count++;	// 视为一次跳变
        
        //        Test_Jump = Jump_Count;
        
        if (!data[i*COL + j+1] && j+1 <= Right_Max)	// 连续两个黑点
        {
          if (!data[i*COL + j+2] && j+2 <= Right_Max)	// 连续三个黑点
          {
            if (!data[i*COL + j+3] && j+3 <= Right_Max)	// 连续四个黑点
            {
              if (!data[i*COL + j+4] && j+4 <= Right_Max)	// 连续五个黑点
              {
                if (!data[i*COL + j+5] && j+5 <= Right_Max)	// 连续六个黑点
                {
                  White_Flag = 0;	// 认为不是干扰，不做任何处理，下次搜索白点
                  j += 5;
                }
                else if (j+5 <= Right_Max)
                {
                  data[i*COL + j] = colour[white];	// 仅有连续五个黑点，滤除掉
                  data[i*COL+j+1] = colour[white];	// 仅有连续五个黑点，滤除掉
                  data[i*COL+j+2] = colour[white];	// 仅有连续五个黑点，滤除掉
                  data[i*COL+j+3] = colour[white];	// 仅有连续五个黑点，滤除掉
                  data[i*COL+j+4] = colour[white];	// 仅有连续五个黑点，滤除掉
                  
                  j += 5;
                }
                else
                {
                  j += 5;
                }
              }
              else if (j+4 <= Right_Max)
              {
                data[i*COL + j] = colour[white];	// 仅有连续四个黑点，滤除掉
                data[i*COL+j+1] = colour[white];	// 仅有连续四个黑点，滤除掉
                data[i*COL+j+2] = colour[white];	// 仅有连续四个黑点，滤除掉
                data[i*COL+j+3] = colour[white];	// 仅有连续四个黑点，滤除掉
                
                j += 4;
              }
              else
              {
                j += 4;
              }
            }
            else if (j+3 <= Right_Max)
            {
              data[i*COL + j] = colour[white];	// 仅有连续三个黑点，滤除掉
              data[i*COL+j+1] = colour[white];	// 仅有连续三个黑点，滤除掉
              data[i*COL+j+2] = colour[white];	// 仅有连续三个黑点，滤除掉
              
              j += 3;
            }
            else
            {
              j += 3;
            }
          }
          else if (j+2 <= Right_Max)
          {
            data[i*COL + j] = colour[white];	// 仅有连续两个黑点，滤除掉
            data[i*COL+j+1] = colour[white];	// 仅有连续两个黑点，滤除掉
            
            j += 2;
          }
          else
          {
            j += 2;
          }
        }
        else if (j+1 <= Right_Max)
        {
          data[i*COL + j] = colour[white];	// 有一个黑点，滤除掉
          
          j += 1;
        }
        else
        {
          j += 1;
        }
      }
    }
  }
  
  return Jump_Count;	// 返回跳变点计数
}


/*      获取第i行跳变次数、位置
*	获取第i行跳变次数                                                     
*       i                要处理的行数（1 代表 第1行）
*       *data            待处理图像 
*       Left_Min         最左侧列数
*       Right_Max        最右侧列数   
*/
uchar Get_Jump(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j=0;  //列计数器
  uchar jump_count=0;
  
  for(j = Left_Min; j < Right_Max; j++)
  {
    if(data[i * COL +j] != data[i * COL +(j + 1)])  //发生跳变
    {
      camera_info.Jump_Location[i * 10 + jump_count] = j;  //记录跳变对应左侧列数（若黑.白，记录黑的列数）
      jump_count++;
    }
  }
}


/*      先左后右搜索边界
*	先左后右搜索边界，记录“外”赛道宽度 和猜测中线位置   成功回 1， 失败回 0（本行无白点）
*
*	说明：记录赛道宽度，保存边界数据
*/
uchar Traversal_Left_Right(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;                    //行号
  uchar White_Flag = 0;       //白点标志,代表第一个方向（从左到右）已找到白点
  
  //将左右边界初始化
  camera_info.Left_Line[i]  = COL-1;
  camera_info.Right_Line[i] = 0;
  
  
  for (j = Left_Min; j <= Right_Max; j++)	// 边界坐标 1到79
  {  
    if (data[i*COL + j] == colour[white] )	// 检测到白点
    {
      camera_info.Left_Line[i] = j;	// 记录当前j值为本行左边界     
      
      break;	// 跳出本循环， 左边界已找到，开始找右边界
    }
    
//    if(j == Right_Max)
//      return 0;    //本行已遍历，未找到白点，保留 0 0 为左右边界值，代表没找到
  }
  
  
  for (j = Right_Max; j >= Left_Min; j--)	// 边界坐标 79 到 1
  {
    if ( data[i * COL + j] == colour[white] )	// 检测到白点
    {
      camera_info.Right_Line[i] = j;	// 记录当前j值为本行右“外”边界
      break;    //已找到左右边界，返回
    }
  }
  
  //记录当前行 猜测宽度 作参考
  camera_info.Out_Width[i] = camera_info.Right_Line[i] - camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      先右后左搜索边界
*	先右后左搜索边界，记录“外”赛道宽度    成功回 1， 失败回 0（本行无白点）
*
*	说明：记录赛道宽度，保存边界数据
*/
uchar Traversal_Right_Left(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;                    //行号
//  uchar White_Flag = 0;       //白点标志,代表第一个方向（从左到右）已找到白点
  
  //将左右边界初始化
  camera_info.Left_Line[i]  = COL-1;
  camera_info.Right_Line[i] = 0;
  
  
  for (j = Right_Max; j >= Left_Min; j--)	// 边界坐标 1到79
  {  
    if (data[i*COL + j] == colour[white] )	// 检测到白点
    {
      camera_info.Right_Line[i] = j;	// 记录当前j值为本行右边界     
      
      break;	// 跳出本循环， 右边界已找到，开始找左边界
    }
    
//    if(j == Left_Min)
//      return 0;    //本行已遍历，未找到白点，保留 0 0 为左右边界值，代表没找到
  }
   
  for (j = Left_Min; j <= Right_Max; j++)	// 边界坐标 79 到 1
  {
    if ( data[i * COL + j] == colour[white] )	// 检测到白点
    {
      camera_info.Left_Line[i] = j;	// 记录当前j值为本行左“外”边界     
      break;    //已找到左右边界，返回
    }
  }
  
    //记录当前行 猜测宽度 作参考
  camera_info.Out_Width[i] = camera_info.Right_Line[i] -camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      以上一次中点为基准，向两边寻找变为黑的位置，做左右赛道边界                  写的很不完善
*	记录赛道宽度，本行中点位置              
*
*	说明：记录赛道宽度，保存边界数据
*/
uchar Traversal_Mid_Side(uchar i, uchar mid, uchar *data)
{
  uchar j;                    //行号
  uchar Mid_Black_Flag = 0;       //黑点标志,代表前一行中点在本行为黑
  
    //将左右边界初始化
  camera_info.Left_Line[i]  = 1;
  camera_info.Right_Line[i] = COL - 1;
  
  for(j=mid; j>0; j--)
  {
    if (data[i*COL + j] == colour[black] )
    {
      camera_info.Left_Line[i] = j;  //记录j+1为本行左边界
      break;            //已找到本行右边界
    }
  }
   
  for(j=mid; j< COL - 1; j++)
  {
    if (data[i*COL + j] == colour[black] )
    {
      camera_info.Right_Line[i] = j;  //记录j+1为本行右边界
      break;           //已找到本行左、右边界
    }
  }
  
  
  
   //记录当前行 猜测宽度  大概的中点 作参考
  camera_info.Out_Width[i] = camera_info.Right_Line[i] -camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      以上一次左右边界为基准，向左右分别找边界 ，第一次以外边为基准               
*	记录赛道宽度，本行中点位置 本行边界位置              
*
*	说明：记录赛道宽度，保存边界数据
*/
uchar Traversal_Line_Line(uchar i,  uchar *data)
{
  u8 j=0;
  
  if(i != ROW - 1) //不是最近行
  {  
    //上一行左边界位置本行为白，想左找左边界
    if(data[i*COL + camera_info.Left_Line[i+1]] == colour[white]) 
    {
      if(camera_info.Left_Line[i+1] != 0)
      {
        camera_info.Left_Line[i] = 1;
        for(j=camera_info.Left_Line[i+1];j>0;j--)
        {
          if ( data[i * COL + j] == colour[black] )
          {
            camera_info.Left_Line[i] = j+1;
            break;
          }
        } 
      }
      else
        camera_info.Left_Line[i] = 0;
    }
    //上一行左边界位置本行为黑，向右找左边界
    else 
    {
      camera_info.Left_Line[i] = COL-1;
      for(j=camera_info.Left_Line[i+1];j<COL;j++)
      {
        if ( data[i * COL + j] == colour[white] )
        {
          camera_info.Left_Line[i] = j;
          break;
        }
      }     
    }
    
    //上一行右边界位置本行为白，向右找右边界
    if(data[i*COL + camera_info.Right_Line[i+1]] == colour[white]) 
    {
      if(camera_info.Right_Line[i+1] != COL-1)
      {
        camera_info.Right_Line[i] = COL-1;
        for(j=camera_info.Right_Line[i+1];j<COL;j++)
        {
          if ( data[i * COL + j] == colour[black] )
          {
            camera_info.Right_Line[i] = j-1;
            break;
          }
        }
      }
      else 
        camera_info.Right_Line[i] = COL-1;
    }
    //上一行右边界位置本行为黑，向左找右边界
    else 
    {
      camera_info.Right_Line[i] = 1;
      for(j=camera_info.Right_Line[i+1];j>0;j--)
      {
        if ( data[i * COL + j] == colour[white] )
        {
          camera_info.Right_Line[i] = j;
          break;
        }
      }        
    }
  }
  else  //最近行，做先左后右往中间找
    Traversal_Left_Right(i,data, 1, COL-1);
  
  //记录当前行 猜测宽度 作参考
  if(camera_info.Right_Line[i] > camera_info.Left_Line[i])
  {
    camera_info.Out_Width[i] = camera_info.Right_Line[i] - camera_info.Left_Line[i];
    camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
  }
  else
  {
    camera_info.Out_Width[i] = 0;
    camera_info.Guess_Mid_Line[i] = 0;
  }  
}

/*      尽头搜索
*	尽头搜索，记录本列从下往上第一次遇到黑点的行号，成功回行号 不成功回 COL-1
*
*	说明：从图像最下端开始竖直向上搜索，返回该列最远行坐标
*/
uchar Limit_Scan(uchar j, uchar *data)
{
  u8 i=0;
  
  camera_info.Far_Point[j] = ROW -1;        //初始化最远点为59，即最近一行
  
  for(i = ROW - 1 ;i >= 0 ;i-- )//从最下行往上找
  {
    if(data[i * COL + j] == colour[black])
    {
      camera_info.Far_Point[j] = i;  //记录本列“最远点”（第一次遇到黑点的位置）
      return i;          //结束搜索
    }
    
    if(i == 0)
    {
      return 0;    //已遍历本列，未找到黑点
    }
  }
  
}



char Error_Transform(uchar Data, uchar Set)                                     //（应该在控制层）
{
    char Error;

    Error = Set - Data;
    if (Error < 0)
    {
        Error = -Error;
    }

    return Error;
}
