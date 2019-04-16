#include "perception.h"

void Perception(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info)
{
  PreprocessImage(img_2d, img60_80, img_buff, car_info);//预处理图像，由原始灰度图像生成滤波后的二值化图像
  //GetInductorValue();//获取电感值，并由三次曲线拟合计算电磁中线
  ExtractRawFeature(img60_80);//提取原始特征,包括左右边线，计算出来的中线，左右丢线数量等特征
  ProcessSpecialElement(img60_80);//在得到原始特征的基础上处理特殊元素,包括环岛十字，障碍物，横断，等在内的特殊元素
  //CalculateCurvityAndMiddleLineFitting();//计算赛道曲率并用三次曲线平滑中线
  //AnalyzeRoadType();//判断当前赛道类型，用于对速度的控制，以及pid参数的修正
  UpdateCarInfo(car_info);//更新car_info
}


int   MiddleLine[IMAGE_HEIGHT + 1];
int   RightEdge[IMAGE_HEIGHT + 1];
int   LeftEdge[IMAGE_HEIGHT + 1];
int   Width[IMAGE_HEIGHT + 1] = {
                                  0, 0, 0, 0, 2,       3, 5, 6, 8, 8,     //10
                                  10, 10, 12, 14, 14,  16, 17, 18, 20, 20,//20
                                  21, 22, 24, 24, 26,  27, 28, 30, 30, 32,//30
                                  32, 34, 35, 36, 37,  38, 39, 40, 41, 42,//40
                                  43, 46, 47, 48, 50,  50, 52, 52, 54, 54,//50
                                  56, 56, 58, 58, 60,  60, 62, 62, 63, 64, 64
                                };

int   MidPri            = IMAGE_WIDTH / 2;//搜索起点
int   LastLine          = 0;//前瞻所在行
int   LongestCol        = 0;//前瞻所在列
int   AvailableLines    = 0;//可用行数
int   LeftLose          = 0;//左边丢线数量
int   RightLose         = 0;//右边丢线数量
int   AllLose           = 0;//两边同时丢线 数量
int   LeftLoseStart     = 0;//记录左边丢线的开始行
int   RightLoseStart    = 0;//记录右边边丢线的开始行
int   WhiteStart        = 0;//全白开始点
int   WhiteNum          = 0;//白线数量

float  curvity         = 0;//简单等效曲率



void PreprocessImage(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info)
{
//  LocalThresh(img_2d, img60_80, 23, 1, car_info);//二值化图像
  uint8 dynamic_thresh = Otsu1D(img_2d);
  int m = 0, n = 0;
  for(int i = 0; i < ROW; i += 1)
  {
    n = 0;
    for(int j = 0; j < COL; j += 2)
    {
      img60_80[m][n] = img_2d[i][j];
      n++;
    }
    m++;
  }
  for(int i = 0; i < IMAGE_HEIGHT; i++)
  {
    for(int j = 0; j < IMAGE_WIDTH; j++)
    {
      if(img60_80[i][j] > dynamic_thresh)
      {
        img60_80[i][j] = WHITE;
      }
      else
      {
        img60_80[i][j] = BLACK;
      }
    }
  }
//  ErodeImage(img60_80);//腐蚀滤波
  CalcValidLine(img60_80);//初步计算前瞻
  Image_Compress(img60_80, img_buff);//图像压缩
}

void ExtractRawFeature(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  SearchBorderLineAndMidLine(img60_80);//寻找左右边线，并计算中线，计算左右丢线数量等特征
  SearchInflexion(img60_80);//寻找左右拐点
}

void ProcessSpecialElement(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  //处理特殊元素有优先级，先圆环十字，后障碍物，再断路（配合会车），再横断，。。。。
//  DetectCircleAndCross();
}

void UpdateCarInfo(CarInfo *car_info)
{
  for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
  {
    MiddleLine[i] = MiddleLine[i] > 126 ? 126 : MiddleLine[i];
    MiddleLine[i] = MiddleLine[i] < - 126 ? - 126 : MiddleLine[i];
    car_info->mid_line[i] = (char)MiddleLine[i];
    car_info->left_line[i] = (char)LeftEdge[i];
    car_info->right_line[i] = (char)RightEdge[i];
  }
  car_info->longest_col = getLongestCol();
  car_info->last_line = getLastLine();
}

int getLastLine(void)
{
  return LastLine;
}

void setLastLine(int line)
{
  LastLine = line;
}

int getLongestCol(void)
{
  return LongestCol;
}

void setLongestCol(int col)
{
  LongestCol = col;
}

int getAvailableLine(void)
{
  return AvailableLines;
}

void setAvailableLine(int line)
{
  AvailableLines = line;
}

float getCurvity(void)
{
  return curvity;
}

void setCurvity(float curv)
{
  curvity = curv;
}

void Image_Compress(uint8 src[IMAGE_HEIGHT][IMAGE_WIDTH],uint8 *dest)
{
  int count = 0;
  uint8 temp = 0;
  for(int i = 0; i < IMAGE_HEIGHT; i++)
  {
    for(int j = 0; j < IMAGE_WIDTH; j++)
    {
      if(src[i][j] > 76)
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

void CalcMiddleLineCurvity(void)//计算中线曲率
{
  float temp = 0;
  for(int i = IMAGE_HEIGHT - 1;i > getLastLine() + 1;i--)
  {
    temp += abs(MiddleLine[i - 1] - MiddleLine[i]);
  }
  if(getAvailableLine() > 2)
    setCurvity(temp / getAvailableLine());
  else
    setCurvity(1);
}

void CalcValidLine(uint8_t img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  int min = IMAGE_HEIGHT;
  int argmin = 0;
  for(u16 i = 0; i < IMAGE_WIDTH / 2; i++)
  {
    int j = IMAGE_HEIGHT - 1;
    while(j > 0 && img60_80[j][i] == WHITE)
    {
      j--;
      if(j < 1)
        break;
    }
    if(j <= min)
    {
      min = j;
      argmin = i;
    }
  }
  for(u16 i = IMAGE_WIDTH - 1; i >= IMAGE_WIDTH / 2; i--)
  {
    int j = IMAGE_HEIGHT - 1;
    while(img60_80[j][i] == WHITE)
    {
      j--;
      if(j < 1)
        break;
    }
    if(j <= min)
    {
      min = j;
      argmin = i;
    }
  }
  setLongestCol(argmin);
  setLastLine(min);
  setAvailableLine(IMAGE_HEIGHT - min);
}

uint8_t CalcVerticalHistogram(uint8_t img_2d[IMAGE_HEIGHT][IMAGE_WIDTH])//计算垂直直方图，并寻找前瞻
{
  uint8_t max = 0;
  uint8_t argmax = 200;
  u8 v_h_gram[IMAGE_WIDTH] = { 0 };
  for(u16 i = 0;i < IMAGE_WIDTH;i++)
  {
    v_h_gram[i] = 0;
    for(u16 j = 0;j < IMAGE_HEIGHT;j++)
    {
      if(img_2d[j][i] == WHITE)
      {
        v_h_gram[i]++;
      }
    }
    if(v_h_gram[i] > max)
    {
       max = v_h_gram[i];
       argmax = i;
    }
  }
  return argmax;
}

uint8_t Otsu1D(uint8_t img_2d[ROW][COL])//一维大津法，计算全局阈值
{
    float Histogram[256] = { 0 };        //建立一维灰度直方图,并初始化变量
    uint16_t height = ROW;        //矩阵的行数
    uint16_t width = COL;         //矩阵的列数
    uint32_t N = height * width;              //像素的总数

    for (int i = 0; i < height; i++)
    {
            for (int j = 0; j < width; j++)
            {
                    uint8_t gray_val = img_2d[i][j];         //获取当前灰度值
                    Histogram[gray_val]++;                  //记录（i,j）的数量
            }
    }
    for (int i = 0; i < 256; i++)
    {
        Histogram[i] /= N;     //归一化的每一个二元组的概率分布
    }
    
    float avg_val = 0.0;
    
    for (int i = 1; i < 256; i++)
    {
        avg_val += i * Histogram[i];
    }


    int threshold = 0; //阈值
    float max_variance = 0.0;
    float w = 0.0, u = 0.0;
    
    for(int i = 0;i < 256;i++)
    {
        w += Histogram[i];
        u += i * Histogram[i];
        
        float t = avg_val * w - u;
        float variance = t * t / (w * (1 - w));
        
        if(variance > max_variance)
        {
            max_variance = variance;
            threshold = i;
        }
    }
    return threshold;
}

uint8 imager[ROW][COL] = { 0 };

void LocalThresh(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 win_size, uint8 is_binary, CarInfo *car_info)//均值滤波，计算局部阈值，结合全局阈值对图像进行二值化
{
  uint8 global_thresh = Otsu1D(img_2d);
  car_info->thresh = global_thresh;
  //float K_w = 0.85;
  int K_w = 1;
  int i = 0, j = 0, k = 0, r = 0, n = 0, sum = 0;
  r = (win_size - 1) / 2;
  int Acol[COL] = { 0 };
  for(i = 0; i < ROW; i++)
  {
    if(i == 0)
    {
      for(k = 0;k < COL; k++)
      {
        Acol[k] = img_2d[0][k];
        for(n = 1; n <= r; n++)
          Acol[k] += img_2d[n][k] * 2;
      }
    }
    else
    {
      if(i >= 1 && i <= r)//即i<r+1;
      {
        for(k = 0; k < COL; k++)
        {
          Acol[k] = Acol[k] - img_2d[r + 1 - i][k] + img_2d[i + r][k];
        }
      }
      if(i > r && i < (ROW - r))
      {
        for(k = 0;k < COL; k++)
        {
          Acol[k] = Acol[k] - img_2d[i - r - 1][k] + img_2d[i + r][k];
        }
      }
      if(i >= (ROW - r) && i < ROW)
      {
        for(k = 0; k < COL; k++)
        {
          Acol[k] = Acol[k] - img_2d[i - r - 1][k] + img_2d[2 * ROW - i - r - 1][k];
        }
      }
    }
    sum = Acol[0];
    for(n = 1; n <= r; n++)
      sum += 2 * Acol[n];
    imager[i][0] = sum / (win_size * win_size);
    if(is_binary)
    {
      int local_thresh = K_w * imager[i][j];//(imager[i][j] + global_thresh) / 2;
      if((img_2d[i][j] - local_thresh) <= 0)
        img_2d[i][j] = BLACK;
      else
        img_2d[i][j] = WHITE;
    }
    for(j = 1; j < COL; j++)
    {
      if(j >= 1 && j <= r)
      {
        sum = sum + Acol[j + r] - Acol[r + 1 - j];
        imager[i][j] = sum / (win_size * win_size);
        if(is_binary)
        {
          int local_thresh = K_w * imager[i][j];//(imager[i][j] + global_thresh) / 2;
          if((img_2d[i][j] - local_thresh) <= 0)
            img_2d[i][j] = BLACK;
          else
            img_2d[i][j] = WHITE;
        }
      }
      if(j > r && j < (COL - r))
      {
        sum = sum - Acol[j - r - 1] + Acol[j + r];
        imager[i][j] = sum / (win_size * win_size);
        if(is_binary)
        {
          int local_thresh = K_w * imager[i][j];//(imager[i][j] + global_thresh) / 2;
          if((img_2d[i][j] - local_thresh) <= 0)
            img_2d[i][j] = BLACK;
          else
            img_2d[i][j] = WHITE;
        }
      }
      if(j >= (COL - r) && j < COL)
      {
        sum = sum - Acol[j - r - 1] + Acol[2 * COL - j - r - 1];
        imager[i][j] = sum / (win_size * win_size);
        if(is_binary)
        {
          int local_thresh = K_w * imager[i][j];//(imager[i][j] + global_thresh) / 2;
          if((img_2d[i][j] - local_thresh) <= 0)
            img_2d[i][j] = BLACK;
          else
            img_2d[i][j] = WHITE;
        }
      }
    }
  }
  int p = 0, q = 0;
  for(int i = 0; i < ROW; i += 1)
  {
    q = 0;
    for(int j = 0; j < COL; j += 2)
    {
      img60_80[p][q] = img_2d[i][j];
      q++;
    }
    p++;
  }
}

//设置中线，左线，右线的初始化值
void initVal(void)
{
  LeftLose        = 0;
  RightLose       = 0;
  AllLose         = 0;
  WhiteNum        = 0;
  Width[IMAGE_HEIGHT] = IMAGE_WIDTH;
  for(int i = 0; i < IMAGE_HEIGHT + 1; i++)//赋初值             
  {
    LeftEdge[i]   = 0;
    RightEdge[i]  = IMAGE_WIDTH;
    MiddleLine[i] = IMAGE_WIDTH / 2;
  }
}
void SearchInflexion(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  
}

void SearchBorderLineAndMidLine(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  initVal();//变量清零
  int start_search_line = getLastLine();
  if(start_search_line >= IMAGE_HEIGHT - 20)
  {
    for(int i = IMAGE_HEIGHT - 1; i > start_search_line; i--)
    {
      //int j = getLongestCol();
      int j = 0;
      if(i == IMAGE_HEIGHT - 1)//首行就以图像中心作为扫描起点
      {
        j = MidPri;//40
      }
      else
      {
        j = MiddleLine[i + 1];//否则就以上一行中点的位置作为本行扫描起点
      }
      if(j <= 3)
      {
          j = 3;
      }
      while(j >= 3)
      {
        if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE && img60_80[i][j + 1] == WHITE)//找黑白白跳变
        {
          LeftEdge[i] = j - 1;
          break;
        }
        j--;
      }
      //j = getLongestCol();
      if(i == IMAGE_HEIGHT - 1) //再找右边界
      {
        j = MidPri;//如果首行，从图像中心开始搜寻
      }
      else
      {
        j = MiddleLine[i + 1];//否则从上一行中心位置开始搜寻
      }
      if(j >= IMAGE_WIDTH - 4)//j >= IMAGE_WIDTH - 3有效范围内搜寻右线
      {
        j = IMAGE_WIDTH - 4;
      }
      while(j <= IMAGE_WIDTH - 4)
      {
        if(img60_80[i][j - 1] == WHITE && img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//找白白黑跳变
        {
          RightEdge[i] = j + 1;
          break;
        }
        j++;
      }
      if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)//中线判断，没有丢线
      {
        MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;  
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//丢了左线
      {
        LeftLose++;//记录只有左线丢的数量
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//突变      
        {
          MiddleLine[i] = MiddleLine[i + 1];//用上一行的中点
        }
        else 
        {
          MiddleLine[i] = RightEdge[i] - Width[i] / 2;//正常的话就用半宽补
        }
      }
      else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//丢了右线
      {
        RightLose++;//记录只有右线丢的数量
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//突变
        {
          MiddleLine[i] = MiddleLine[i + 1];//用上一行的中点
        }
        else 
        {
          MiddleLine[i] = LeftEdge[i] + Width[i] / 2;//正常的话就用半宽补
        }
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//两边都丢了的话  
      {
        AllLose++;
        if(i == IMAGE_HEIGHT - 1)//如果是首行就以图像中心作为中点
        {
          MiddleLine[i] = IMAGE_WIDTH / 2;//MidPri;
        }
        else
        {
          MiddleLine[i] = MiddleLine[i + 1];//如果不是首行就用上一行的中线作为本行中点
        }
      }
      if(MiddleLine[IMAGE_HEIGHT - 1] >= IMAGE_WIDTH - 20)
      {
        MidPri = IMAGE_WIDTH - 20;
      }
      else if(MiddleLine[IMAGE_HEIGHT - 1] <= 20)
      {
        MidPri = 20;
      }
      else
      {
        MidPri = MiddleLine[IMAGE_HEIGHT - 1];//记录本帧图像第59行的中线值，作为下一幅图像的59行扫描起始点
      }
    }
  }
  else
  {
    for(int i = IMAGE_HEIGHT - 1; i > IMAGE_HEIGHT - 20; i--)
    {
      //int j = getLongestCol();
      int j = 0;
      if(i == IMAGE_HEIGHT - 1)//首行就以图像中心作为扫描起点
      {
        j = MidPri;//40
      }
      else
      {
        j = MiddleLine[i + 1];//否则就以上一行中点的位置作为本行扫描起点
      }
      if(j <= 3)
      {
          j = 3;
      }
      while(j >= 3)
      {
        if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE && img60_80[i][j + 1] == WHITE)
        {
          LeftEdge[i] = j - 1;
          break;
        }
        j--;
      }
      //j = getLongestCol();
      if(i == IMAGE_HEIGHT - 1) //再找右边界
      {
        j = MidPri;//如果首行，从图像中心开始搜寻
      }
      else
      {
        j = MiddleLine[i + 1];//否则从上一行中心位置开始搜寻
      }
      if(j >= IMAGE_WIDTH - 4)//j >= IMAGE_WIDTH - 3有效范围内搜寻右线
      {
        j = IMAGE_WIDTH - 4;
      }
      while(j <= IMAGE_WIDTH - 4)
      {
        if(img60_80[i][j - 1] == WHITE && img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)
        {
          RightEdge[i] = j + 1;
          break;
        }
        j++;
      }
      if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)//中线判断，没有丢线
      {
        MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;  
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//丢了左线
      {
        LeftLose++;//记录只有左线丢的数量
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//突变      
        {
          MiddleLine[i] = MiddleLine[i + 1];//用上一行的中点
        }
        else 
        {
          MiddleLine[i] = RightEdge[i] - Width[i] / 2;//正常的话就用半宽补
        }
      }
      else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//丢了右线
      {
        RightLose++;//记录只有右线丢的数量
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//突变
        {
          MiddleLine[i] = MiddleLine[i + 1];//用上一行的中点
        }
        else 
        {
          MiddleLine[i] = LeftEdge[i] + Width[i] / 2;//正常的话就用半宽补
        }
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//两边都丢了的话  
      {
        AllLose++;
        if(i == IMAGE_HEIGHT - 1)//如果是首行就以图像中心作为中点
        {
          MiddleLine[i] = IMAGE_WIDTH / 2;//MidPri;
        }
        else
        {
          MiddleLine[i] = MiddleLine[i + 1];//如果不是首行就用上一行的中线作为本行中点
        }
      }
      if(MiddleLine[IMAGE_HEIGHT - 1] >= IMAGE_WIDTH - 20)
      {
        MidPri = IMAGE_WIDTH - 20;
      }
      else if(MiddleLine[IMAGE_HEIGHT - 1] <= 20)
      {
        MidPri = 10;
      }
      else
      {
        MidPri = MiddleLine[IMAGE_HEIGHT - 1];//记录本帧图像第59行的中线值，作为下一幅图像的59行扫描起始点
      }
    }
    for(int i = IMAGE_HEIGHT - 20; i > start_search_line; i--)
    {
      if(LeftEdge[i + 1] != 0 && RightEdge[i + 1] != IMAGE_WIDTH) //上一行两边都找到 启用边沿扫描
      {
        int j = ((LeftEdge[i + 1] + 10) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (LeftEdge[i + 1] + 10);//先找左边界
        int jj = ((LeftEdge[i + 1] - 5) <= 1) ? 1 : (LeftEdge[i + 1] - 5);
        while(j >= jj)
        {
            if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)//找黑白跳变
            {
                 LeftEdge[i] = j;
                 break;
            }
           j--;
        }
        j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//再找右边界
        jj = ((RightEdge[i + 1] + 5) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (RightEdge[i + 1] + 5);    
        while(j <= jj)    
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//找白黑跳变
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
      }
      else if(LeftEdge[i + 1] != 0 && RightEdge[i + 1] == IMAGE_WIDTH)//上一行只找到左边界
      {
        int j  = ((LeftEdge[i + 1] + 10) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (LeftEdge[i + 1] + 10);//左边界用边沿扫描
        int jj = ((LeftEdge[i + 1] - 5) <= 1) ? 1 : (LeftEdge[i + 1] - 5);
        while(j >= jj)
        {
            if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)//找黑白
            {
                 LeftEdge[i] = j;
                 break;
            }
           j--;
        }
        j = MiddleLine[i + 1];//上一行丢了右边界用全行扫描
        if(j >= IMAGE_WIDTH - 3)
        {
             j = IMAGE_WIDTH - 3;
        }
        while(j <= IMAGE_WIDTH - 3)
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
      }
      else if(LeftEdge[i + 1] == 0 && RightEdge[i + 1] != IMAGE_WIDTH)//上一行只找到右边界  
      {
        int j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//边缘追踪找右边界
        int jj = ((RightEdge[i + 1] + 5) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (RightEdge[i + 1] + 5);
        while(j <= jj)
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//找白黑
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
        j = MiddleLine[i + 1];//全行扫描找左边界
        if(j <= 2)
        {
          j = 2;
        }
        while(j >= 2)
        {
          if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)
          {
            LeftEdge[i] = j;
            break;
          }
          j--;
        }
      }
      else if(LeftEdge[i + 1] == 0 && RightEdge[i + 1] == IMAGE_WIDTH)//上一行没找到边界，可能是十字或者环形  
      {
        int j = MiddleLine[i + 1];//全行找左边界
        while(j >= 2)
        {
          if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)
          {
            LeftEdge[i] = j;
            break;
          }
          j--;
        }
        j = MiddleLine[i + 1];//全行找右边界   
        while(j <= IMAGE_WIDTH - 3)
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
      }
      if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//不满足畸变
      {
        MiddleLine[i] = MiddleLine[i + 1];//用上一行
      }
      else
      {
        if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)
        {
          MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;     
          //对斜出十字进行纠正
          /*if( MiddleLine[i] - MiddleLine[i + 1] > 8 && ((abs(LeftEdge[i] - LeftEdge[i + 1] > 3) || abs(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//中线向右突变
          {
            uint8_t ii = i;
            while(1)
            {
              MiddleLine[ii + 1] = MiddleLine[ii] - 1;
              ii++;
              if(ii >= 50 || (MiddleLine[ii] - MiddleLine[ii + 1] <= 1))
              {
                break;
              }
            }
          }
          if((MiddleLine[i + 1] - MiddleLine[i] > 8) && ((abs(LeftEdge[i] - LeftEdge[i + 1] > 3) || abs(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//中线向左突变
          {
            uint8_t ii = i;
            while(1)
            {
              MiddleLine[ii + 1] = MiddleLine[ii] + 1;
              ii++;
              if(ii >= 50 || (MiddleLine[ii + 1] - MiddleLine[ii] <= 1))
              {
                break;
              }
            }
          }*/
        }
        else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//只找到左
        {
          RightLose++;
          if(LeftEdge[i + 1] != 0)//上一行找到了，用偏移量
          {
            MiddleLine[i] = MiddleLine[i + 1] + (LeftEdge[i] - LeftEdge[i + 1]);
          }
          else//否则就用赛道宽度一半补
          {
            MiddleLine[i] = LeftEdge[i] + Width[i] / 2;
          }
        }
        else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//只找到右
        {
          LeftLose++;
          if(RightEdge[i + 1] != IMAGE_WIDTH)//上一行找到了，用偏移量
          {
            MiddleLine[i] = MiddleLine[i + 1] + (RightEdge[i] - RightEdge[i + 1]);
          }
          else//否则就用赛道宽度一半补
          {
            MiddleLine[i] = RightEdge[i] - Width[i] / 2;
          }
        }
        else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//两边丢线    
        {
          WhiteNum++;
          AllLose++;
          if(WhiteNum == 1)
          {
            WhiteStart = i;
          }
          MiddleLine[i] = MiddleLine[i + 1];//用上一行的
        }
      }
      if(i == 0)
      {
        setLastLine(0);
        setAvailableLine(IMAGE_HEIGHT);
        break;
      }
      int m = MiddleLine[i];
      if(m < 5)
      {
        m = 5;
      }
      if(m > IMAGE_WIDTH - 6)
      {
        m = IMAGE_WIDTH - 6;
      }
      if((LeftEdge[i] != 0 && LeftEdge[i] >= IMAGE_WIDTH - 6) || (RightEdge[i] != IMAGE_WIDTH && RightEdge[i] <= 5) || (i >= 1) && (img60_80[i - 1][m] == BLACK)) //最后一行              
      {
        setLastLine(i);//最后一行，动态前瞻
        setAvailableLine(IMAGE_HEIGHT - i);//有效行数
        break;
      }
    }
  }
}
  
//  CalcMiddleLineCurvity();
//  DetectCircle();
//  
//  

//}






//0代表 初始，1代表第一次检测到，2代表开始进环岛，3代表正在环岛，4代表出环岛
int circle_state = 0;
int run_circle_flag = 0;
int last_coord_x = 2;
int last_coord_y = IMAGE_HEIGHT - 1;

//
//void DetectCircle()
//{
//  if((abs(LeftLose - RightLose) >= 10))//左右丢线不对称 
//  {
//    if(run_circle_flag == 0)
//      run_circle_flag = 1;//开启圆环检测
//  }
//  if(run_circle_flag == 1)
//  {
//    if(circle_state == 0)
//    {
//      uint8 left_jump_point = 0;
//      uint8 right_jump_point = 0;
//      //检测第一个拐点，右
//      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 25; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] <= 0 && RightEdge[i] - RightEdge[i + 1] >= - 2 && RightEdge[i - 1] - RightEdge[i] > 10)//右边线 有突变
//        {
//          right_jump_point = 1;
//          circle_state = 1;
//          break;
//        }
//      }
//      //检测第一个拐点，左
//      /*for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 25; i--)
//      {
//        if(LeftEdge[i] - LeftEdge[i + 1] >= 0 && LeftEdge[i] - LeftEdge[i + 1] <= 2 && LeftEdge[i - 1] - LeftEdge[i] < - 10)//右边线 有突变
//        {
//          left_jump_point = 1;
//          break;
//        }
//      }
//      if(right_jump_point == 1 && left_jump_point == 1)
//      {
//        
//      }
//      else if(right_jump_point == 1)
//      {
//        circle_state = 1;//触发标志位
//      }
//      else if(left_jump_point == 1)
//      {
//        circle_state = 0;//触发标志位
//      }*/
//    }
//    else if(circle_state == 1)
//    {
//      int flag = 0;
//      //检测第一个拐点
//      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 30; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] <= 0 && RightEdge[i] - RightEdge[i + 1] >= - 3 && RightEdge[i - 1] - RightEdge[i] > 15)//右边线 有突变
//        {
//          flag = 1;//触发标志位
//          break;
//        }
//      }
//      if(flag == 0 && RightLose < 8)
//      {
//        circle_state = 2;
//      }
//      //第2步
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        int tmp = (int)(Width[i] + 6);
//        RightEdge[i] = LeftEdge[i] + tmp;
//        MiddleLine[i] = LeftEdge[i] + tmp / 2;
//      }
//    }
//    else if(circle_state == 2)
//    {
//      //检测第二个拐点， 并接着补线
//      /*for(int i = IMAGE_HEIGHT - 25; i > 15; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] < - 20 && RightEdge[i - 1] - RightEdge[i] >= - 2 && RightEdge[i - 1] - RightEdge[i] <= 0)//右边线 有突变
//        {
//          circle_state = 2;//触发标志位
//        }
//      }*/
//      
//      //第一步
//      /*int jump_num = 0;
//      int last_1_jump_num = 0;
//      int last_2_jump_num = 0;
//      for(int i = 23; i > 8; i--)
//      {
//        jump_num = 0;
//        for(int j = LeftEdge[i]; j < IMAGE_WIDTH - 5; j++)
//        {
//          if((img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK) || (img60_80[i][j] == BLACK && img60_80[i][j + 1] == WHITE))
//          {
//            jump_num++;
//          }
//        }
//        if(jump_num + last_1_jump_num + last_2_jump_num >= 12)
//        {
//          for(int k = 2; k < IMAGE_WIDTH - 5; k++)
//          {
//            if((img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK 
//                  && img60_80[i + 1][k + 1] == WHITE && img60_80[i + 1][k + 2] == WHITE) 
//             ||
//               (img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                  && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                  (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                    && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK && img60_80[i + 1][k + 2] == BLACK 
//                  && img60_80[i + 1][k + 3] == WHITE && img60_80[i + 1][k + 4] == WHITE))
//            {
//              circle_state = 3;//触发标志位
//              break;
//            }
//          }
//          break;
//        }
//        last_2_jump_num = last_1_jump_num;
//        last_1_jump_num = jump_num;
//      }*/
//      if(AllLose + RightLose > 15)
//      {
//        circle_state = 3;
//      }
//      
//      //第2步
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        /*int tmp = (int)(Width[i] + 5);
//        RightEdge[i] = LeftEdge[i] + tmp;
//        MiddleLine[i] = LeftEdge[i] + tmp / 2;*/
//        LeftEdge[i] = RightEdge[i] - Width[i];
//        MiddleLine[i] = RightEdge[i] - Width[i] / 2;
//      }
//    }
//    else if(circle_state == 3)
//    {
//      //for(int 
////      if(LeftEdge[IMAGE_HEIGHT - 1] > 15)
////        circle_state = 4;
//      
//      //run_circle_flag = 0;
//      //circle_state = 0;
//      /*int jump_num = 0;
//      int coord_x = 2;
//      int coord_y = IMAGE_HEIGHT - 1;
//      
//      int last_1_jump_num = 0;
//      int last_2_jump_num = 0;
//      
//      for(int i = IMAGE_HEIGHT - 5; i > 8; i--)
//      {
//        jump_num = 0;
//        for(int j = 1; j < IMAGE_WIDTH - 2; j++)
//        {
//          if((img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK) || (img60_80[i][j] == BLACK && img60_80[i][j + 1] == WHITE))
//          {
//            jump_num++;
//          }
//        }
//        if(jump_num + last_1_jump_num + last_2_jump_num >= 9)
//        {
//          for(int k = 2; k < IMAGE_WIDTH - 10; k++)
//          {
//            if((img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK 
//                  && img60_80[i + 1][k + 1] == WHITE && img60_80[i + 1][k + 2] == WHITE) 
//             ||
//               (img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                  && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                  (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                    && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK && img60_80[i + 1][k + 2] == BLACK 
//                  && img60_80[i + 1][k + 3] == WHITE && img60_80[i + 1][k + 4] == WHITE))
//            {
//              coord_x = k;
//              coord_y = i + 1;
//              last_coord_x = coord_x;
//              last_coord_y = coord_y;
//              break;
//            }
//          }
////          if(coord_x == 2 && coord_y == IMAGE_HEIGHT - 1)
////          {
////            img60_80[i + 1][20] = BLACK;
////            img60_80[30][40] = BLACK;
////            img60_80[i + 1][60] = BLACK;
////          }
//          break;
//        }
//        last_2_jump_num = last_1_jump_num;
//        last_1_jump_num = jump_num;
//      }
////      if(coord_x == 2 && coord_y == IMAGE_HEIGHT - 1)
////      {
////        coord_x = last_coord_x;
////        coord_y = last_coord_y;
////      }
//      
//      float k = (float)(IMAGE_HEIGHT - 1 - coord_y) / (0 - coord_x);
//      float b = IMAGE_HEIGHT - 1;
//      
//      for(int i = IMAGE_HEIGHT - 5; i > 10; i--)
//      {
//        if((img60_80[i][IMAGE_WIDTH - 1] == WHITE && img60_80[i - 1][IMAGE_WIDTH - 1] == BLACK && img60_80[i - 2][IMAGE_WIDTH - 1] == BLACK)
//           || (img60_80[i][IMAGE_WIDTH - 2] == WHITE && img60_80[i - 1][IMAGE_WIDTH - 2] == BLACK && img60_80[i - 2][IMAGE_WIDTH - 2] == BLACK))
//        {
//          setLastLine(i);
//          break;
//        }
//      }
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        if(coord_x == 2)
//          break;
//        int tmp_x = (int)((i - b) / k + 0.5);
//        tmp_x = tmp_x > IMAGE_WIDTH - 1 ? IMAGE_WIDTH - 1 : tmp_x;
//        tmp_x = tmp_x < 0 ? 0 : tmp_x;
//        
//        img60_80[i][tmp_x] = BLACK;
//        LeftEdge[i] = tmp_x + 1;
//        int j = LeftEdge[i];
//        if(j >= IMAGE_WIDTH - 3)
//        {
//          j = IMAGE_WIDTH - 3;
//        }
//        while(j <= IMAGE_WIDTH - 3)
//        {
//          if(img60_80[i][j - 1] == WHITE && img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//找白白黑跳变
//          {
//            RightEdge[i] = j + 1;
//            break;
//          }
//          j++;
//        }
//        MiddleLine[i] = RightEdge[i] - Width[i] / 2;
//      }*/
//      if(AllLose + RightLose > 18)
//      {
//        int var_x = IMAGE_WIDTH / 2;
//        int last_l = getLastLine();
//        if(last_l > IMAGE_HEIGHT / 2 + 5)
//        {
//          var_x = IMAGE_WIDTH / 2 + 15;
//        }
//        else if(last_l > IMAGE_HEIGHT / 2 - 5)
//        {
//          var_x = IMAGE_WIDTH / 2 + 10;
//        }
//        else if(last_l > IMAGE_HEIGHT / 2 - 15)
//        {
//          var_x = IMAGE_WIDTH / 2 + 5;
//        }
//        
//        float k = (float)(IMAGE_HEIGHT - 1 - getLastLine()) / (var_x - IMAGE_WIDTH + 1);
//        float b = IMAGE_HEIGHT - 1 - (var_x) * k;
//
//        for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//        {
//          int tmp_x = (int)((i - b) / k + 0.5);
//          tmp_x = tmp_x > IMAGE_WIDTH - 1 ? IMAGE_WIDTH - 1 : tmp_x;
//          tmp_x = tmp_x < 0 ? 0 : tmp_x;
//          MiddleLine[i] = tmp_x;
//        }
//      }
//      /*else if()
//      {
//        circle_state = 4;
//      }*/
//      if((getLastLine() > IMAGE_HEIGHT / 2) && LeftLose <= 1)
//      {
//        circle_state = 4;
//      }
//      
//      //2跳到3 条件 进环 到 环中
////      if(coord_y >= IMAGE_HEIGHT - 11 && coord_y <= IMAGE_HEIGHT - 2 /*|| abs(LeftLose - RightLose) < 2*/)
////      {
////        circle_state = 4;
////      }
//    }
//    else if(circle_state == 4)
//    {
//      uint8 flag = 0;
//      uint8 cnt = 0;
////      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 30; i--)
////      {
////        if((LeftEdge[i] - LeftEdge[i + 1] > 0 && LeftEdge[i - 1] - LeftEdge[i] < - 2 || LeftEdge[i] - LeftEdge[i + 1] > 0 && LeftEdge[i - 1] - LeftEdge[i] < 0) && RightLose > 15)
////        {
////          flag = 1;
////        }
////      }
//      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 40; i--)
//      {
//        if(LeftEdge[i] - LeftEdge[i + 1] < 0)
//        {
//          cnt++;
//        }
//      }
//      if(cnt > 6)// && RightLose > 8)
//      {
//        circle_state = 5;
//      }
//    }
//    else if(circle_state == 5)
//    {
//      if(/*(LeftEdge[i] - LeftEdge[i + 1] > 0 && LeftEdge[i - 1] - LeftEdge[i] < - 2 && RightLose > 20) ||*/ AllLose > 27)//右边线 有突变
//      {
//        circle_state = 6;
//      }
//      if(AllLose + RightLose > 18)
//      {
//        int var_x = getLastLine() > IMAGE_HEIGHT / 2 ? IMAGE_WIDTH / 2 + 8 : IMAGE_WIDTH / 2 + 5;
//        float k = (float)(IMAGE_HEIGHT - 1 - getLastLine()) / (var_x - IMAGE_WIDTH + 1);
//        float b = IMAGE_HEIGHT - 1 - (var_x) * k;
//
//        for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//        {
//          int tmp_x = (int)((i - b) / k + 0.5);
//          tmp_x = tmp_x > IMAGE_WIDTH - 1 ? IMAGE_WIDTH - 1 : tmp_x;
//          tmp_x = tmp_x < 0 ? 0 : tmp_x;
//          MiddleLine[i] = tmp_x;
//        }
//      }
//    }
//    else if(circle_state == 6)
//    {
//      /*if(abs(LeftLose - RightLose) > 15 && LeftLose > 20)
//      {
//        circle_state = 5;
//      }*/
//      if(AllLose + RightLose > 15)
//      {
//        int var_x = getLastLine() > IMAGE_HEIGHT / 2 ? IMAGE_WIDTH / 2 + 12 : IMAGE_WIDTH / 2 + 8;
//        float k = (float)(IMAGE_HEIGHT - 1 - getLastLine()) / (var_x - IMAGE_WIDTH + 1);
//        float b = IMAGE_HEIGHT - 1 - (var_x) * k;
//        printf("%.2f\r\n", k);
//
//        for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//        {
//          int tmp_x = (int)((i - b) / k + 0.5);
//          tmp_x = tmp_x > IMAGE_WIDTH - 1 ? IMAGE_WIDTH - 1 : tmp_x;
//          tmp_x = tmp_x < 0 ? 0 : tmp_x;
//          MiddleLine[i] = tmp_x;
//        }
//      }
//      else
//      {
//        circle_state = 0;
//        run_circle_flag = 0;
//        return;
//      }
//      if(AllLose < 3 && LeftLose < 3)
//      {
//        for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//        {
//          MiddleLine[i] = LeftEdge[i] + Width[i] / 2 + 3;
//        }
//      }
//    }
//    else if(circle_state == 7)
//    {
//      int jump_num = 0;
//      int last_1_jump_num = 0;
//      int last_2_jump_num = 0;
//      for(int i = IMAGE_HEIGHT - 20; i > 8; i--)
//      {
//        jump_num = 0;
//        for(int j = 4; j < IMAGE_WIDTH - 5; j++)
//        {
//          if((img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK) || (img60_80[i][j] == BLACK && img60_80[i][j + 1] == WHITE))
//          {
//            jump_num++;
//          }
//        }
//        if(jump_num + last_1_jump_num + last_2_jump_num >= 12)
//        {
//          /*for(int k = 2; k < IMAGE_WIDTH - 5; k++)
//          {
//            if((img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK 
//                  && img60_80[i + 1][k + 1] == WHITE && img60_80[i + 1][k + 2] == WHITE) 
//             ||
//               (img60_80[i + 1][k - 2] == WHITE && img60_80[i + 1][k - 1] == WHITE 
//                && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                  && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                  (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK 
//                    && img60_80[i + 1][k + 2] == WHITE && img60_80[i + 1][k + 3] == WHITE)
//              || 
//                (img60_80[i + 1][k - 3] == WHITE && img60_80[i + 1][k - 2] == WHITE 
//                && img60_80[i + 1][k - 1] == BLACK && img60_80[i + 1][k] == BLACK && img60_80[i + 1][k + 1] == BLACK && img60_80[i + 1][k + 2] == BLACK 
//                  && img60_80[i + 1][k + 3] == WHITE && img60_80[i + 1][k + 4] == WHITE))
//            {*/
//              circle_state = 6;//触发标志位
//              /*break;
//            }
//          }*/
//          break;
//        }
//        last_2_jump_num = last_1_jump_num;
//        last_1_jump_num = jump_num;
//      }
//      
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        int tmp = (int)(Width[i]);
//        RightEdge[i] = LeftEdge[i] + tmp;
//        MiddleLine[i] = LeftEdge[i] + tmp / 2;
//      }
//    }
//    else if(circle_state == 7)
//    {
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        int tmp = (int)(Width[i]);
//        RightEdge[i] = LeftEdge[i] + tmp;
//        MiddleLine[i] = LeftEdge[i] + tmp / 2;
//      }
//      if(abs(LeftLose - RightLose) <= 2)
//      {
//        circle_state = 0;
//        run_circle_flag = 0;
//      }
//    }
//  }
//  if(0)
//  {
//    run_circle_flag = 0;
//  }
//  car_info.circle_state = circle_state;
//}

////对十字进行识别和进行特殊处理      
//void NormalCrossConduct(void)
//{
//  unsigned char i;
//
//  LastLastCrossFlag=LastCrossFlag;//记录上上次是否是十字
//  LastCrossFlag=CrossFlag;//记录上一次是否是十字
//  CrossFlag=0;//清零
//
//
//  if((AllLose>=15))//左右同时丢线 
//  {
//    CrossFlag = 1;//标记十字
//  }
//  if(CrossFlag)
//  {
//    //这里应该找十字特征点，进行拉线
//    
//    /*if((BlackEndM>=BlackEndL)&&(BlackEndM>=BlackEndR))//中间白线最多
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=40;//直走    
//      }
//    }
//    else if((BlackEndL>BlackEndM)&&(BlackEndL>BlackEndR))//看到左边白的最多，车体偏右了，往左跑
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=30; 
//      }
//    }
//    else if((BlackEndR>BlackEndM)&&(BlackEndR>BlackEndL))//看到右边白的最多，车体偏左了，往右跑
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=50;
//      }
//    }*/
//  }
//  //IsOrNotCrossLoop();
//}


