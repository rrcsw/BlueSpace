#ifndef __IMG_PROC_H__
#define __IMG_PROC_H__

#include "common.h"
#include "SEEKFREE_FUN.h"    //��Ҫ�������е��޷�����
#include "SEEKFREE_MT9V032.h"


#define S_SIZE 600
/*--��������ͷ�ɼ����� �ṹ��--*/
struct Camera_Info
{
  u8 image01[ROW * COL];    //��ֵ��ͼ��
  u8 smallimg[ S_SIZE ];   //ѹ����Ķ�ֵ��ͼ��
  
  uchar Far_Point[COL];     //ÿ�а����ص���ĵ���Զ�㣨�ɽ���Զ��һ���кڵ��������
  uchar Fariest_Point;      //��֡ͼ������ص������Զ�㣨�ɽ���Զ��
  
  uchar Left_Line[ROW];     // ԭʼ��"��"�߽�����(�����ҵ�һ�ΰ׵�λ��)
  uchar Right_Line[ROW];    // ԭʼ��"��"�߽����ݣ����ҵ����һ�ΰ׵�λ�ã�
  uchar Guess_Mid_Line[ROW];//�������ұ߽�����Ԥ��������λ��
  uchar Out_Width[ROW];     //���⡱������� ������������֮��
 
  uchar True_Jump_Num[ROW]; //ԭʼ��ֵ��ͼ��ÿ���������
  uchar Jump_Location[ROW * 10];//����ʴ�˲�����ÿ�и��������λ�� һ��һ��ͼ�����䲻����10��
  
//  uchar Jump_B_W[ROW][10];  //����ʴ�˲�����ÿ�С������ס������λ�� һ��һ��ͼ�����䲻����10��
//  uchar Jump_W_B[ROW][10];  //����ʴ�˲�����ÿ�С������ڡ������λ�� һ��һ��ͼ�����䲻����10��
};



//********************************��������*************************************//

//------��ֵ��ͼ�����ɡ��Ż����------
uint8 OtsuThreshold( uint8 *image, uint16 col, uint16 row);                     //�����ֵ
void Create_Image01(uint8 *image, uint16 col, uint16 row);      //���ɶ�ֵ��ͼ�� ��ʹ�ô�򷨣�
void img_extract(uint8 *dst, uint8 *src, uint32 srclen);                        //��ֵ��ͼ���ѹ
void img_recontract(uint8 *dst, uint8 *src, uint32 srclen);                     //��ֵ��ͼ��ѹ��
void Image_Compression(uint8 src[ROW*COL],uint8 *dest);
//-------------------------------------------------------------------


//------------�߽��������------------
//������������߽磬��¼���⡱�������    �ɹ��� 1�� ʧ�ܻ� 0
uchar Traversal_Left_Right(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);  
//���Һ��������߽磬��¼���⡱�������    �ɹ��� 1�� ʧ�ܻ� 0
uchar Traversal_Right_Left(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);
//����һ���е�Ϊ��׼��������Ѱ�ұ�Ϊ�ڵ�λ�ã������������߽�
uchar Traversal_Mid_Side(uchar i, uchar mid, uchar *data);
//����һ�����ұ߽�Ϊ��׼�������ҷֱ��ұ߽� ����һ�������Ϊ��׼
uchar Traversal_Line_Line(uchar i,  uchar *data);
//Ѱ�� ��Point�� ��Զ���к�
uchar Limit_Scan(uchar j, uchar *data);
//��ʴ�˲���������������㣬�����ص�i���������
uchar Corrode_Filter(uchar i, uchar *data, uchar Left_Min, uchar Right_Max); 
//��ȡÿ�����������λ��
uchar Get_Jump(uchar i, uchar *data, uchar Left_Min, uchar Right_Max);          
//-------------------------------------------------------------------

char Error_Transform(uchar Data, uchar Set);


#endif