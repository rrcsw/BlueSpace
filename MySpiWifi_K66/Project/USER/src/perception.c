#include "perception.h"

void Perception(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info)
{
  PreprocessImage(img_2d, img60_80, img_buff, car_info);//Ԥ����ͼ����ԭʼ�Ҷ�ͼ�������˲���Ķ�ֵ��ͼ��
  //GetInductorValue();//��ȡ���ֵ����������������ϼ���������
  ExtractRawFeature(img60_80);//��ȡԭʼ����,�������ұ��ߣ�������������ߣ����Ҷ�������������
  ProcessSpecialElement(img60_80);//�ڵõ�ԭʼ�����Ļ����ϴ�������Ԫ��,��������ʮ�֣��ϰ����ϣ������ڵ�����Ԫ��
  //CalculateCurvityAndMiddleLineFitting();//�����������ʲ�����������ƽ������
  //AnalyzeRoadType();//�жϵ�ǰ�������ͣ����ڶ��ٶȵĿ��ƣ��Լ�pid����������
  UpdateCarInfo(car_info);//����car_info
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

int   MidPri            = IMAGE_WIDTH / 2;//�������
int   LastLine          = 0;//ǰհ������
int   LongestCol        = 0;//ǰհ������
int   AvailableLines    = 0;//��������
int   LeftLose          = 0;//��߶�������
int   RightLose         = 0;//�ұ߶�������
int   AllLose           = 0;//����ͬʱ���� ����
int   LeftLoseStart     = 0;//��¼��߶��ߵĿ�ʼ��
int   RightLoseStart    = 0;//��¼�ұ߱߶��ߵĿ�ʼ��
int   WhiteStart        = 0;//ȫ�׿�ʼ��
int   WhiteNum          = 0;//��������

float  curvity         = 0;//�򵥵�Ч����



void PreprocessImage(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE], CarInfo *car_info)
{
//  LocalThresh(img_2d, img60_80, 23, 1, car_info);//��ֵ��ͼ��
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
//  ErodeImage(img60_80);//��ʴ�˲�
  CalcValidLine(img60_80);//��������ǰհ
  Image_Compress(img60_80, img_buff);//ͼ��ѹ��
}

void ExtractRawFeature(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  SearchBorderLineAndMidLine(img60_80);//Ѱ�����ұ��ߣ����������ߣ��������Ҷ�������������
  SearchInflexion(img60_80);//Ѱ�����ҹյ�
}

void ProcessSpecialElement(uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH])
{
  //��������Ԫ�������ȼ�����Բ��ʮ�֣����ϰ���ٶ�·����ϻᳵ�����ٺ�ϣ���������
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

void CalcMiddleLineCurvity(void)//������������
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

uint8_t CalcVerticalHistogram(uint8_t img_2d[IMAGE_HEIGHT][IMAGE_WIDTH])//���㴹ֱֱ��ͼ����Ѱ��ǰհ
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

uint8_t Otsu1D(uint8_t img_2d[ROW][COL])//һά��򷨣�����ȫ����ֵ
{
    float Histogram[256] = { 0 };        //����һά�Ҷ�ֱ��ͼ,����ʼ������
    uint16_t height = ROW;        //���������
    uint16_t width = COL;         //���������
    uint32_t N = height * width;              //���ص�����

    for (int i = 0; i < height; i++)
    {
            for (int j = 0; j < width; j++)
            {
                    uint8_t gray_val = img_2d[i][j];         //��ȡ��ǰ�Ҷ�ֵ
                    Histogram[gray_val]++;                  //��¼��i,j��������
            }
    }
    for (int i = 0; i < 256; i++)
    {
        Histogram[i] /= N;     //��һ����ÿһ����Ԫ��ĸ��ʷֲ�
    }
    
    float avg_val = 0.0;
    
    for (int i = 1; i < 256; i++)
    {
        avg_val += i * Histogram[i];
    }


    int threshold = 0; //��ֵ
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

void LocalThresh(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint16 win_size, uint8 is_binary, CarInfo *car_info)//��ֵ�˲�������ֲ���ֵ�����ȫ����ֵ��ͼ����ж�ֵ��
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
      if(i >= 1 && i <= r)//��i<r+1;
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

//�������ߣ����ߣ����ߵĳ�ʼ��ֵ
void initVal(void)
{
  LeftLose        = 0;
  RightLose       = 0;
  AllLose         = 0;
  WhiteNum        = 0;
  Width[IMAGE_HEIGHT] = IMAGE_WIDTH;
  for(int i = 0; i < IMAGE_HEIGHT + 1; i++)//����ֵ             
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
  initVal();//��������
  int start_search_line = getLastLine();
  if(start_search_line >= IMAGE_HEIGHT - 20)
  {
    for(int i = IMAGE_HEIGHT - 1; i > start_search_line; i--)
    {
      //int j = getLongestCol();
      int j = 0;
      if(i == IMAGE_HEIGHT - 1)//���о���ͼ��������Ϊɨ�����
      {
        j = MidPri;//40
      }
      else
      {
        j = MiddleLine[i + 1];//���������һ���е��λ����Ϊ����ɨ�����
      }
      if(j <= 3)
      {
          j = 3;
      }
      while(j >= 3)
      {
        if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE && img60_80[i][j + 1] == WHITE)//�Һڰװ�����
        {
          LeftEdge[i] = j - 1;
          break;
        }
        j--;
      }
      //j = getLongestCol();
      if(i == IMAGE_HEIGHT - 1) //�����ұ߽�
      {
        j = MidPri;//������У���ͼ�����Ŀ�ʼ��Ѱ
      }
      else
      {
        j = MiddleLine[i + 1];//�������һ������λ�ÿ�ʼ��Ѱ
      }
      if(j >= IMAGE_WIDTH - 4)//j >= IMAGE_WIDTH - 3��Ч��Χ����Ѱ����
      {
        j = IMAGE_WIDTH - 4;
      }
      while(j <= IMAGE_WIDTH - 4)
      {
        if(img60_80[i][j - 1] == WHITE && img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//�Ұװ׺�����
        {
          RightEdge[i] = j + 1;
          break;
        }
        j++;
      }
      if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)//�����жϣ�û�ж���
      {
        MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;  
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//��������
      {
        LeftLose++;//��¼ֻ�����߶�������
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//ͻ��      
        {
          MiddleLine[i] = MiddleLine[i + 1];//����һ�е��е�
        }
        else 
        {
          MiddleLine[i] = RightEdge[i] - Width[i] / 2;//�����Ļ����ð��
        }
      }
      else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//��������
      {
        RightLose++;//��¼ֻ�����߶�������
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//ͻ��
        {
          MiddleLine[i] = MiddleLine[i + 1];//����һ�е��е�
        }
        else 
        {
          MiddleLine[i] = LeftEdge[i] + Width[i] / 2;//�����Ļ����ð��
        }
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//���߶����˵Ļ�  
      {
        AllLose++;
        if(i == IMAGE_HEIGHT - 1)//��������о���ͼ��������Ϊ�е�
        {
          MiddleLine[i] = IMAGE_WIDTH / 2;//MidPri;
        }
        else
        {
          MiddleLine[i] = MiddleLine[i + 1];//����������о�����һ�е�������Ϊ�����е�
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
        MidPri = MiddleLine[IMAGE_HEIGHT - 1];//��¼��֡ͼ���59�е�����ֵ����Ϊ��һ��ͼ���59��ɨ����ʼ��
      }
    }
  }
  else
  {
    for(int i = IMAGE_HEIGHT - 1; i > IMAGE_HEIGHT - 20; i--)
    {
      //int j = getLongestCol();
      int j = 0;
      if(i == IMAGE_HEIGHT - 1)//���о���ͼ��������Ϊɨ�����
      {
        j = MidPri;//40
      }
      else
      {
        j = MiddleLine[i + 1];//���������һ���е��λ����Ϊ����ɨ�����
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
      if(i == IMAGE_HEIGHT - 1) //�����ұ߽�
      {
        j = MidPri;//������У���ͼ�����Ŀ�ʼ��Ѱ
      }
      else
      {
        j = MiddleLine[i + 1];//�������һ������λ�ÿ�ʼ��Ѱ
      }
      if(j >= IMAGE_WIDTH - 4)//j >= IMAGE_WIDTH - 3��Ч��Χ����Ѱ����
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
      if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)//�����жϣ�û�ж���
      {
        MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;  
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//��������
      {
        LeftLose++;//��¼ֻ�����߶�������
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//ͻ��      
        {
          MiddleLine[i] = MiddleLine[i + 1];//����һ�е��е�
        }
        else 
        {
          MiddleLine[i] = RightEdge[i] - Width[i] / 2;//�����Ļ����ð��
        }
      }
      else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//��������
      {
        RightLose++;//��¼ֻ�����߶�������
        if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//ͻ��
        {
          MiddleLine[i] = MiddleLine[i + 1];//����һ�е��е�
        }
        else 
        {
          MiddleLine[i] = LeftEdge[i] + Width[i] / 2;//�����Ļ����ð��
        }
      }
      else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//���߶����˵Ļ�  
      {
        AllLose++;
        if(i == IMAGE_HEIGHT - 1)//��������о���ͼ��������Ϊ�е�
        {
          MiddleLine[i] = IMAGE_WIDTH / 2;//MidPri;
        }
        else
        {
          MiddleLine[i] = MiddleLine[i + 1];//����������о�����һ�е�������Ϊ�����е�
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
        MidPri = MiddleLine[IMAGE_HEIGHT - 1];//��¼��֡ͼ���59�е�����ֵ����Ϊ��һ��ͼ���59��ɨ����ʼ��
      }
    }
    for(int i = IMAGE_HEIGHT - 20; i > start_search_line; i--)
    {
      if(LeftEdge[i + 1] != 0 && RightEdge[i + 1] != IMAGE_WIDTH) //��һ�����߶��ҵ� ���ñ���ɨ��
      {
        int j = ((LeftEdge[i + 1] + 10) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (LeftEdge[i + 1] + 10);//������߽�
        int jj = ((LeftEdge[i + 1] - 5) <= 1) ? 1 : (LeftEdge[i + 1] - 5);
        while(j >= jj)
        {
            if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)//�Һڰ�����
            {
                 LeftEdge[i] = j;
                 break;
            }
           j--;
        }
        j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//�����ұ߽�
        jj = ((RightEdge[i + 1] + 5) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (RightEdge[i + 1] + 5);    
        while(j <= jj)    
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//�Ұ׺�����
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
      }
      else if(LeftEdge[i + 1] != 0 && RightEdge[i + 1] == IMAGE_WIDTH)//��һ��ֻ�ҵ���߽�
      {
        int j  = ((LeftEdge[i + 1] + 10) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (LeftEdge[i + 1] + 10);//��߽��ñ���ɨ��
        int jj = ((LeftEdge[i + 1] - 5) <= 1) ? 1 : (LeftEdge[i + 1] - 5);
        while(j >= jj)
        {
            if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)//�Һڰ�
            {
                 LeftEdge[i] = j;
                 break;
            }
           j--;
        }
        j = MiddleLine[i + 1];//��һ�ж����ұ߽���ȫ��ɨ��
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
      else if(LeftEdge[i + 1] == 0 && RightEdge[i + 1] != IMAGE_WIDTH)//��һ��ֻ�ҵ��ұ߽�  
      {
        int j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//��Ե׷�����ұ߽�
        int jj = ((RightEdge[i + 1] + 5) >= IMAGE_WIDTH - 2) ? IMAGE_WIDTH - 2 : (RightEdge[i + 1] + 5);
        while(j <= jj)
        {
          if(img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//�Ұ׺�
          {
            RightEdge[i] = j;
            break;
          }
          j++;
        }
        j = MiddleLine[i + 1];//ȫ��ɨ������߽�
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
      else if(LeftEdge[i + 1] == 0 && RightEdge[i + 1] == IMAGE_WIDTH)//��һ��û�ҵ��߽磬������ʮ�ֻ��߻���  
      {
        int j = MiddleLine[i + 1];//ȫ������߽�
        while(j >= 2)
        {
          if(img60_80[i][j - 1] == BLACK && img60_80[i][j] == WHITE)
          {
            LeftEdge[i] = j;
            break;
          }
          j--;
        }
        j = MiddleLine[i + 1];//ȫ�����ұ߽�   
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
      if((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 1))//���������
      {
        MiddleLine[i] = MiddleLine[i + 1];//����һ��
      }
      else
      {
        if(LeftEdge[i] != 0 && RightEdge[i] != IMAGE_WIDTH)
        {
          MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;     
          //��б��ʮ�ֽ��о���
          /*if( MiddleLine[i] - MiddleLine[i + 1] > 8 && ((abs(LeftEdge[i] - LeftEdge[i + 1] > 3) || abs(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//��������ͻ��
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
          if((MiddleLine[i + 1] - MiddleLine[i] > 8) && ((abs(LeftEdge[i] - LeftEdge[i + 1] > 3) || abs(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//��������ͻ��
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
        else if(LeftEdge[i] != 0 && RightEdge[i] == IMAGE_WIDTH)//ֻ�ҵ���
        {
          RightLose++;
          if(LeftEdge[i + 1] != 0)//��һ���ҵ��ˣ���ƫ����
          {
            MiddleLine[i] = MiddleLine[i + 1] + (LeftEdge[i] - LeftEdge[i + 1]);
          }
          else//��������������һ�벹
          {
            MiddleLine[i] = LeftEdge[i] + Width[i] / 2;
          }
        }
        else if(LeftEdge[i] == 0 && RightEdge[i] != IMAGE_WIDTH)//ֻ�ҵ���
        {
          LeftLose++;
          if(RightEdge[i + 1] != IMAGE_WIDTH)//��һ���ҵ��ˣ���ƫ����
          {
            MiddleLine[i] = MiddleLine[i + 1] + (RightEdge[i] - RightEdge[i + 1]);
          }
          else//��������������һ�벹
          {
            MiddleLine[i] = RightEdge[i] - Width[i] / 2;
          }
        }
        else if(LeftEdge[i] == 0 && RightEdge[i] == IMAGE_WIDTH)//���߶���    
        {
          WhiteNum++;
          AllLose++;
          if(WhiteNum == 1)
          {
            WhiteStart = i;
          }
          MiddleLine[i] = MiddleLine[i + 1];//����һ�е�
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
      if((LeftEdge[i] != 0 && LeftEdge[i] >= IMAGE_WIDTH - 6) || (RightEdge[i] != IMAGE_WIDTH && RightEdge[i] <= 5) || (i >= 1) && (img60_80[i - 1][m] == BLACK)) //���һ��              
      {
        setLastLine(i);//���һ�У���̬ǰհ
        setAvailableLine(IMAGE_HEIGHT - i);//��Ч����
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






//0���� ��ʼ��1�����һ�μ�⵽��2����ʼ��������3�������ڻ�����4���������
int circle_state = 0;
int run_circle_flag = 0;
int last_coord_x = 2;
int last_coord_y = IMAGE_HEIGHT - 1;

//
//void DetectCircle()
//{
//  if((abs(LeftLose - RightLose) >= 10))//���Ҷ��߲��Գ� 
//  {
//    if(run_circle_flag == 0)
//      run_circle_flag = 1;//����Բ�����
//  }
//  if(run_circle_flag == 1)
//  {
//    if(circle_state == 0)
//    {
//      uint8 left_jump_point = 0;
//      uint8 right_jump_point = 0;
//      //����һ���յ㣬��
//      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 25; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] <= 0 && RightEdge[i] - RightEdge[i + 1] >= - 2 && RightEdge[i - 1] - RightEdge[i] > 10)//�ұ��� ��ͻ��
//        {
//          right_jump_point = 1;
//          circle_state = 1;
//          break;
//        }
//      }
//      //����һ���յ㣬��
//      /*for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 25; i--)
//      {
//        if(LeftEdge[i] - LeftEdge[i + 1] >= 0 && LeftEdge[i] - LeftEdge[i + 1] <= 2 && LeftEdge[i - 1] - LeftEdge[i] < - 10)//�ұ��� ��ͻ��
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
//        circle_state = 1;//������־λ
//      }
//      else if(left_jump_point == 1)
//      {
//        circle_state = 0;//������־λ
//      }*/
//    }
//    else if(circle_state == 1)
//    {
//      int flag = 0;
//      //����һ���յ�
//      for(int i = IMAGE_HEIGHT - 5; i > IMAGE_HEIGHT - 30; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] <= 0 && RightEdge[i] - RightEdge[i + 1] >= - 3 && RightEdge[i - 1] - RightEdge[i] > 15)//�ұ��� ��ͻ��
//        {
//          flag = 1;//������־λ
//          break;
//        }
//      }
//      if(flag == 0 && RightLose < 8)
//      {
//        circle_state = 2;
//      }
//      //��2��
//      for(int i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
//      {
//        int tmp = (int)(Width[i] + 6);
//        RightEdge[i] = LeftEdge[i] + tmp;
//        MiddleLine[i] = LeftEdge[i] + tmp / 2;
//      }
//    }
//    else if(circle_state == 2)
//    {
//      //���ڶ����յ㣬 �����Ų���
//      /*for(int i = IMAGE_HEIGHT - 25; i > 15; i--)
//      {
//        if(RightEdge[i] - RightEdge[i + 1] < - 20 && RightEdge[i - 1] - RightEdge[i] >= - 2 && RightEdge[i - 1] - RightEdge[i] <= 0)//�ұ��� ��ͻ��
//        {
//          circle_state = 2;//������־λ
//        }
//      }*/
//      
//      //��һ��
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
//              circle_state = 3;//������־λ
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
//      //��2��
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
//          if(img60_80[i][j - 1] == WHITE && img60_80[i][j] == WHITE && img60_80[i][j + 1] == BLACK)//�Ұװ׺�����
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
//      //2����3 ���� ���� �� ����
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
//      if(/*(LeftEdge[i] - LeftEdge[i + 1] > 0 && LeftEdge[i - 1] - LeftEdge[i] < - 2 && RightLose > 20) ||*/ AllLose > 27)//�ұ��� ��ͻ��
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
//              circle_state = 6;//������־λ
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

////��ʮ�ֽ���ʶ��ͽ������⴦��      
//void NormalCrossConduct(void)
//{
//  unsigned char i;
//
//  LastLastCrossFlag=LastCrossFlag;//��¼���ϴ��Ƿ���ʮ��
//  LastCrossFlag=CrossFlag;//��¼��һ���Ƿ���ʮ��
//  CrossFlag=0;//����
//
//
//  if((AllLose>=15))//����ͬʱ���� 
//  {
//    CrossFlag = 1;//���ʮ��
//  }
//  if(CrossFlag)
//  {
//    //����Ӧ����ʮ�������㣬��������
//    
//    /*if((BlackEndM>=BlackEndL)&&(BlackEndM>=BlackEndR))//�м�������
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=40;//ֱ��    
//      }
//    }
//    else if((BlackEndL>BlackEndM)&&(BlackEndL>BlackEndR))//������߰׵���࣬����ƫ���ˣ�������
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=30; 
//      }
//    }
//    else if((BlackEndR>BlackEndM)&&(BlackEndR>BlackEndL))//�����ұ߰׵���࣬����ƫ���ˣ�������
//    {
//      for(i=58;i>20;i--)
//      {
//        MiddleLine[i]=50;
//      }
//    }*/
//  }
//  //IsOrNotCrossLoop();
//}


