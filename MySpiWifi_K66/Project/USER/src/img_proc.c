#include "img_proc.h"

struct Camera_Info camera_info;  //ȫ�ֱ����ṹ�壬����extern�﷨����

extern uint8 image[ROW][COL];      //������ڲ�ʹ�ûҶ�ͼ��λ��

//camera.c������õ�ȫ�ֱ���
uint8 colour[2] = {255, 0}; //��ֵ�������ɫ
uint8 black = 1,white = 0;  //0 �� 1 �ֱ��Ӧ����ɫ


void struct_clear(unsigned char *pta, int size )
{
    while(size>0)
    {
        *pta++ = 0;
        size --;
    }
}


int32 range_protect(int32 duty, int32 min, int32 max)//�޷�����
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
  u8 temp_far=79;  //����Ѱ�ҡ���¼��ͼ��Զ��

  //�ṹ���ڲ�Ԫ�س�ʼ������
  struct_clear(( unsigned char *)&camera_info,sizeof(camera_info));
  
  //�ô�����ɶ�ֵ��ͼ�� ���� camera_info.image01[ROW][COL]                        �������α���
  Create_Image01(image, COL, ROW);
  //�Զ�ֵ��ͼ�����ѹ�� ����   camera_info.smallimg                                 ����һ�α���
//  img_recontract(camera_info.smallimg, camera_info.image01,COL * ROW /8);
  Image_Compression(camera_info.image01, camera_info.smallimg);
  
  camera_info. Fariest_Point = ROW - 1;  //�Ƚ���Զ���ʼ��Ϊ���
  
  for(j = 0; j < COL ;j++)                                                      //����һ�α���
  {
    temp_far = Limit_Scan(j, camera_info.image01);   //��ÿ�дӽ���Զ��һ�α�ڵĵ��λ��
    if(temp_far < camera_info. Fariest_Point)
    {
      camera_info. Fariest_Point = temp_far;         //����ȫͼ��Զ�ڵ�λ��
    }
  }
  
  //��temp_far ��������  ��¼��ͼ��ɨ��߽����Զ��λ��
  if(camera_info. Fariest_Point <= ROW/4)   //�����ͼ������Ұ������ȡ��40�� ɨ�����Ҳ�߽�
    temp_far = ROW/4;
  else 
    temp_far = camera_info. Fariest_Point; //�����ͼ���������ڿ��·���ȡȫͼ��Զ������ ɨ�����Ҳ�߽�
  
  
  for(i = ROW - 1; i >= temp_far ;i--)                                          //��� (2/3)*3 �α���
  {
//    Traversal_Left_Right(i, camera_info.image01,1, COL - 1);    //����е���Զ�м�¼���ҡ��⡱�߽� �� ��¼��������ȡ�
    Traversal_Line_Line(i, camera_info.image01);    //����һ�����ұ߽�Ϊ��׼�������ҷֱ��ұ߽� ����һ�������Ϊ��׼
    //����� ȡ��ֵΪmid
//    if(i == ROW - 1)
//      Traversal_Mid_Side(i, COL/2, camera_info.image01);
//    else
//      Traversal_Mid_Side(i, camera_info.Out_Width[i+1], camera_info.image01);
    
//    if(camera_info.Out_Width[i] < COL)  
    if(camera_info.Left_Line[i]<camera_info.Right_Line[i])      //��һ�гɹ�ִ��
    {
      //���ݵõ������ҡ��⡱�߽���и�ʴ�˲�����¼������ʵ�������  True_Jump_Num[i]
      camera_info.True_Jump_Num[i] = Corrode_Filter(i, camera_info.image01, camera_info.Left_Line[i], camera_info.Right_Line[i]);
      //��¼��i�и�ʴ�˲��󣬷�������Ķ�Ӧ����
      Get_Jump(i, camera_info.image01, camera_info.Left_Line[i], camera_info.Right_Line[i]);
    }
  } 
}



uint8 OtsuThreshold( uint8 *image, uint16 col, uint16 row)
{
    uint16 OtsuThresholdOut;
    uint16 GrayList[256]= {0};  //�Ҷ�ֵ����ͳ������
    uint16 i, pixelSum = col * row;   //ͼ���С
    uint16 Camera_Graylevel=0;
    uint16 Graylevel=0;         //�Ҷ�ֵ
    uint16 cnts=0,cnt=0;
    float avfor=0,avback=0;
    float total=0,total_low=0,total_high=0,Variance=0,VarianceMax=0;
    //���ֱ��ͼ
    for(Graylevel=0; Graylevel<256; Graylevel++)
    {
        GrayList[Graylevel]=0;
    }
    //����ֱ��ͼ

    for(i=0; i<pixelSum; i++)
    {
        Camera_Graylevel=*(image+i);//��ȡ�Ҷ�ֵ
        GrayList[Camera_Graylevel]+=1;//ֱ��ͼͳ��ͼ���������ص�
        total+=Camera_Graylevel;//total����ͼ��ĻҶ�ֵ�ܺ�
    }

    for(Graylevel=0; Graylevel<256; Graylevel++)
    {
        cnt=GrayList[Graylevel];//��ǰ�Ҷ�ֵ�ļ�������
        if(cnt==0)   continue;
        total_low+=cnt*Graylevel;//����Ҷ�ֵ��0����ǰֵ�����ص�Ҷ�ֵ���ܺ�
        cnts+=cnt;//����Ҷ�ֵ��0����ǰֵ�����ص���Ŀ���ܺ�

        if(cnts==pixelSum)  break;//ͳ�����������ص㣬֮��ĻҶ�ֵ�Ѿ�û���κ����ص���ϣ�������ϣ��˳�

        total_high=total-total_low;//�����Ҷ�=�Ҷ��ܺ�-ǰ���Ҷ�(��û�й�һ��)
        avfor=(int)(total_low/cnts);//ǰ����һ��

        avback=(int)(total_high/(pixelSum-cnts));//������һ��
        Variance=(avback-avfor)*(avback-avfor)*(cnts*pixelSum-cnts*cnts)/(pixelSum*pixelSum);//���㷽��

        if(VarianceMax<Variance)
        {
            VarianceMax=Variance;
            OtsuThresholdOut=Graylevel;//�������ĻҶ�ֵ��Ϊ�����ֵ
        }
    }
    return OtsuThresholdOut;
}


/*!      ���ɶ�ֵ��ͼ�� ��image01[ROW][COL]��ȥ
*  @brief      ���ɶ�ֵ��ͼ��
*  @param      image           ԭͼ���ַ
*  @param      image01         ��ֵ��ͼ���ַ
*  @param      col             ͼ��col
*  @param      row             ͼ��row
*  @since      v5.0            image_threshold = OtsuThreshold(image[0],COL,ROW);
*  Sample usage:
*/
void Create_Image01(uint8 *image,  uint16 col, uint16 row)
{
  uint8 i,j;                      //������
  uint8 *pic;                   //����ͼ���
  uint8 image_threshold;        //��ֵ����ֵ
  
  //��򷨼����ֵ����ֵ
  image_threshold = OtsuThreshold( image, col, row ); 
  //ָ��ָ��ԭͼ���׵�ַ
  pic = &image[0];
  //���ɶ�ֵ��ͼ��
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


/*!      ��ֵ��ͼ���ѹ
*  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
*  @param      dst             ͼ���ѹĿ�ĵ�ַ
*  @param      src             ͼ���ѹԴ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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


/*!      ��ֵ��ͼ��ѹ��
*  @brief      ��ֵ��ͼ��ѹ�����ռ� �� ʱ�� ѹ����
*  @param      dst             ͼ��ѹ��Ŀ�ĵ�ַ
*  @param      src             ͼ��ѹ��Դ��ַ
*  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
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


/*      ��ʴ�˲� 
*	��ʴ�˲�                                                      
*	˵�����������˳�����ͳ�ƺڰ���������������������߼��
*       i                Ҫ�����������1 ���� ��1�У�
*       *data            ������ͼ�� 
*       Left_Min         ���������
*       Right_Max        ���Ҳ�����   
*/
uchar Corrode_Filter(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;
  //  uchar Test_Jump = 0;
  uchar White_Flag = 0; //�׵��ҵ���־λ
  uchar Jump_Count = 0;	// ��������
  
  if(Left_Min>=Right_Max)
    return 0;
  
  Right_Max = range_protect(Right_Max, 1, COL - 1);	// �����Ҳಿ�����򣬷�ֹ���
  
  for (j = Left_Min; j <= Right_Max; j++)	// ��������ɨ�裬����Ӱ����
  {
    if (!White_Flag)	// �Ȳ��Ұ׵㣬ֻ�˺ڵ㣬���˰׵�
    {
      if (data[i * COL + j] == colour[white])	// i��j�� ��⵽�׵�
      {
        White_Flag = 1;	// ��ʼ�Һڵ�
      }
    }
    else
    {
      if (data[i * COL + j] == colour[black])	// ��⵽�ڵ�
      {
        Jump_Count++;	// ��Ϊһ������
        
        //        Test_Jump = Jump_Count;
        
        if (!data[i*COL + j+1] && j+1 <= Right_Max)	// ���������ڵ�
        {
          if (!data[i*COL + j+2] && j+2 <= Right_Max)	// ���������ڵ�
          {
            if (!data[i*COL + j+3] && j+3 <= Right_Max)	// �����ĸ��ڵ�
            {
              if (!data[i*COL + j+4] && j+4 <= Right_Max)	// ��������ڵ�
              {
                if (!data[i*COL + j+5] && j+5 <= Right_Max)	// ���������ڵ�
                {
                  White_Flag = 0;	// ��Ϊ���Ǹ��ţ������κδ����´������׵�
                  j += 5;
                }
                else if (j+5 <= Right_Max)
                {
                  data[i*COL + j] = colour[white];	// ������������ڵ㣬�˳���
                  data[i*COL+j+1] = colour[white];	// ������������ڵ㣬�˳���
                  data[i*COL+j+2] = colour[white];	// ������������ڵ㣬�˳���
                  data[i*COL+j+3] = colour[white];	// ������������ڵ㣬�˳���
                  data[i*COL+j+4] = colour[white];	// ������������ڵ㣬�˳���
                  
                  j += 5;
                }
                else
                {
                  j += 5;
                }
              }
              else if (j+4 <= Right_Max)
              {
                data[i*COL + j] = colour[white];	// ���������ĸ��ڵ㣬�˳���
                data[i*COL+j+1] = colour[white];	// ���������ĸ��ڵ㣬�˳���
                data[i*COL+j+2] = colour[white];	// ���������ĸ��ڵ㣬�˳���
                data[i*COL+j+3] = colour[white];	// ���������ĸ��ڵ㣬�˳���
                
                j += 4;
              }
              else
              {
                j += 4;
              }
            }
            else if (j+3 <= Right_Max)
            {
              data[i*COL + j] = colour[white];	// �������������ڵ㣬�˳���
              data[i*COL+j+1] = colour[white];	// �������������ڵ㣬�˳���
              data[i*COL+j+2] = colour[white];	// �������������ڵ㣬�˳���
              
              j += 3;
            }
            else
            {
              j += 3;
            }
          }
          else if (j+2 <= Right_Max)
          {
            data[i*COL + j] = colour[white];	// �������������ڵ㣬�˳���
            data[i*COL+j+1] = colour[white];	// �������������ڵ㣬�˳���
            
            j += 2;
          }
          else
          {
            j += 2;
          }
        }
        else if (j+1 <= Right_Max)
        {
          data[i*COL + j] = colour[white];	// ��һ���ڵ㣬�˳���
          
          j += 1;
        }
        else
        {
          j += 1;
        }
      }
    }
  }
  
  return Jump_Count;	// ������������
}


/*      ��ȡ��i�����������λ��
*	��ȡ��i���������                                                     
*       i                Ҫ�����������1 ���� ��1�У�
*       *data            ������ͼ�� 
*       Left_Min         ���������
*       Right_Max        ���Ҳ�����   
*/
uchar Get_Jump(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j=0;  //�м�����
  uchar jump_count=0;
  
  for(j = Left_Min; j < Right_Max; j++)
  {
    if(data[i * COL +j] != data[i * COL +(j + 1)])  //��������
    {
      camera_info.Jump_Location[i * 10 + jump_count] = j;  //��¼�����Ӧ�������������.�ף���¼�ڵ�������
      jump_count++;
    }
  }
}


/*      ������������߽�
*	������������߽磬��¼���⡱������� �Ͳ²�����λ��   �ɹ��� 1�� ʧ�ܻ� 0�������ް׵㣩
*
*	˵������¼������ȣ�����߽�����
*/
uchar Traversal_Left_Right(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;                    //�к�
  uchar White_Flag = 0;       //�׵��־,�����һ�����򣨴����ң����ҵ��׵�
  
  //�����ұ߽��ʼ��
  camera_info.Left_Line[i]  = COL-1;
  camera_info.Right_Line[i] = 0;
  
  
  for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 1��79
  {  
    if (data[i*COL + j] == colour[white] )	// ��⵽�׵�
    {
      camera_info.Left_Line[i] = j;	// ��¼��ǰjֵΪ������߽�     
      
      break;	// ������ѭ���� ��߽����ҵ�����ʼ���ұ߽�
    }
    
//    if(j == Right_Max)
//      return 0;    //�����ѱ�����δ�ҵ��׵㣬���� 0 0 Ϊ���ұ߽�ֵ������û�ҵ�
  }
  
  
  for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 79 �� 1
  {
    if ( data[i * COL + j] == colour[white] )	// ��⵽�׵�
    {
      camera_info.Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ҡ��⡱�߽�
      break;    //���ҵ����ұ߽磬����
    }
  }
  
  //��¼��ǰ�� �²��� ���ο�
  camera_info.Out_Width[i] = camera_info.Right_Line[i] - camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      ���Һ��������߽�
*	���Һ��������߽磬��¼���⡱�������    �ɹ��� 1�� ʧ�ܻ� 0�������ް׵㣩
*
*	˵������¼������ȣ�����߽�����
*/
uchar Traversal_Right_Left(uchar i, uchar *data, uchar Left_Min, uchar Right_Max)
{
  uchar j;                    //�к�
//  uchar White_Flag = 0;       //�׵��־,�����һ�����򣨴����ң����ҵ��׵�
  
  //�����ұ߽��ʼ��
  camera_info.Left_Line[i]  = COL-1;
  camera_info.Right_Line[i] = 0;
  
  
  for (j = Right_Max; j >= Left_Min; j--)	// �߽����� 1��79
  {  
    if (data[i*COL + j] == colour[white] )	// ��⵽�׵�
    {
      camera_info.Right_Line[i] = j;	// ��¼��ǰjֵΪ�����ұ߽�     
      
      break;	// ������ѭ���� �ұ߽����ҵ�����ʼ����߽�
    }
    
//    if(j == Left_Min)
//      return 0;    //�����ѱ�����δ�ҵ��׵㣬���� 0 0 Ϊ���ұ߽�ֵ������û�ҵ�
  }
   
  for (j = Left_Min; j <= Right_Max; j++)	// �߽����� 79 �� 1
  {
    if ( data[i * COL + j] == colour[white] )	// ��⵽�׵�
    {
      camera_info.Left_Line[i] = j;	// ��¼��ǰjֵΪ�������⡱�߽�     
      break;    //���ҵ����ұ߽磬����
    }
  }
  
    //��¼��ǰ�� �²��� ���ο�
  camera_info.Out_Width[i] = camera_info.Right_Line[i] -camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      ����һ���е�Ϊ��׼��������Ѱ�ұ�Ϊ�ڵ�λ�ã������������߽�                  д�ĺܲ�����
*	��¼������ȣ������е�λ��              
*
*	˵������¼������ȣ�����߽�����
*/
uchar Traversal_Mid_Side(uchar i, uchar mid, uchar *data)
{
  uchar j;                    //�к�
  uchar Mid_Black_Flag = 0;       //�ڵ��־,����ǰһ���е��ڱ���Ϊ��
  
    //�����ұ߽��ʼ��
  camera_info.Left_Line[i]  = 1;
  camera_info.Right_Line[i] = COL - 1;
  
  for(j=mid; j>0; j--)
  {
    if (data[i*COL + j] == colour[black] )
    {
      camera_info.Left_Line[i] = j;  //��¼j+1Ϊ������߽�
      break;            //���ҵ������ұ߽�
    }
  }
   
  for(j=mid; j< COL - 1; j++)
  {
    if (data[i*COL + j] == colour[black] )
    {
      camera_info.Right_Line[i] = j;  //��¼j+1Ϊ�����ұ߽�
      break;           //���ҵ��������ұ߽�
    }
  }
  
  
  
   //��¼��ǰ�� �²���  ��ŵ��е� ���ο�
  camera_info.Out_Width[i] = camera_info.Right_Line[i] -camera_info.Left_Line[i];
  camera_info.Guess_Mid_Line[i] = camera_info.Left_Line[i] + camera_info.Out_Width[i]/2;
}


/*      ����һ�����ұ߽�Ϊ��׼�������ҷֱ��ұ߽� ����һ�������Ϊ��׼               
*	��¼������ȣ������е�λ�� ���б߽�λ��              
*
*	˵������¼������ȣ�����߽�����
*/
uchar Traversal_Line_Line(uchar i,  uchar *data)
{
  u8 j=0;
  
  if(i != ROW - 1) //���������
  {  
    //��һ����߽�λ�ñ���Ϊ�ף���������߽�
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
    //��һ����߽�λ�ñ���Ϊ�ڣ���������߽�
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
    
    //��һ���ұ߽�λ�ñ���Ϊ�ף��������ұ߽�
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
    //��һ���ұ߽�λ�ñ���Ϊ�ڣ��������ұ߽�
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
  else  //����У�������������м���
    Traversal_Left_Right(i,data, 1, COL-1);
  
  //��¼��ǰ�� �²��� ���ο�
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

/*      ��ͷ����
*	��ͷ��������¼���д������ϵ�һ�������ڵ���кţ��ɹ����к� ���ɹ��� COL-1
*
*	˵������ͼ�����¶˿�ʼ��ֱ�������������ظ�����Զ������
*/
uchar Limit_Scan(uchar j, uchar *data)
{
  u8 i=0;
  
  camera_info.Far_Point[j] = ROW -1;        //��ʼ����Զ��Ϊ59�������һ��
  
  for(i = ROW - 1 ;i >= 0 ;i-- )//��������������
  {
    if(data[i * COL + j] == colour[black])
    {
      camera_info.Far_Point[j] = i;  //��¼���С���Զ�㡱����һ�������ڵ��λ�ã�
      return i;          //��������
    }
    
    if(i == 0)
    {
      return 0;    //�ѱ������У�δ�ҵ��ڵ�
    }
  }
  
}



char Error_Transform(uchar Data, uchar Set)                                     //��Ӧ���ڿ��Ʋ㣩
{
    char Error;

    Error = Set - Data;
    if (Error < 0)
    {
        Error = -Error;
    }

    return Error;
}
