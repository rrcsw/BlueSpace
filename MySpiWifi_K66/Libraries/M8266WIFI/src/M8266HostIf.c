/********************************************************************
* M8266HostIf.c
* .Description
*     Source file of M8266WIFI Host Interface 
* .Copyright(c) Anylinkin Technology 2015.5-
*     IoT@anylinkin.com
*     http://www.anylinkin.com
*     http://anylinkin.taobao.com
*  Author
*     wzuo
*  Date
*  Version
********************************************************************/
#include "common.h"
#include "headfile.h"


extern SPI_Type* SPIN[3]; //��������ָ�����鱣�� SPIx �ĵ�ַ

/***********************************************************************************
* M8266HostIf_GPIO_SPInCS_nRESET_Pin_Init                                         *
* Description                                                                     *
*    To initialise the GPIOs for SPI nCS and nRESET output for M8266WIFI module   *
*    You may update the macros of GPIO PINs usages for nRESET from brd_cfg.h      *
*    You are not recommended to modify codes below please                         *
* Parameter(s):                                                                   *
*    None                                                                         *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/
void M8266HostIf_GPIO_CS_RESET_Init(void)
{
  gpio_init(M8266WIFI_nRESET_GPIO, GPO, 1);  // set nRESET pin output and initially output high
  gpio_init(M8266WIFI_nCS_GPIO,    GPO, 1);  // set nCS    pin output and initially output high
}

/***********************************************************************************
* M8266HostIf_SPI_Init                                                            *
* Description                                                                     *
*    To Initialise the SPI Host IF for M8266WIFI module                           *
* Parameter(s):                                                                   *
*    None                                                                         *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/
uint32 M8266HostIf_SPI_Init(uint32 baud)
{
  uint32 fit_clk;
  
  SIM->SCGC6 |=SIM_SCGC_DSPIx_MASK;
//  SIM_SCGCx_FOR_SPI |= SIM_SCGC_DSPIx_MASK;           // SIM(System integration module) low-power logic. Enable DSPI clock
  
  port_init(SPI_SCK_GPIO,     ALT2  );                // Configure the GPIO to SPI alternative functions
  port_init(SPI_MOSI_GPIO,    ALT2  );
  port_init(SPI_MISO_GPIO ,   ALT2  );
  
  
  SPIN[SPI_NUM]->MCR = ( 0
                                        | SPI_MCR_CLR_TXF_MASK     //��� Tx FIFO ������
                                        | SPI_MCR_CLR_RXF_MASK     //��� Rx FIFO ������
                                        | SPI_MCR_HALT_MASK        //ֹͣSPI����
                                        );
  
  //-----------------------���־λ-------------------------
  SPIN[SPI_NUM]->SR = ( 0
                                        | SPI_SR_EOQF_MASK    //���Ͷ��п��ˣ��������
                                        | SPI_SR_TFUF_MASK    //����FIFO�����־λ��SPIΪ�ӻ�ģʽ��Tx FIFOΪ�գ����ⲿSPI����ģʽ�������䣬��־λ�ͻ���1��д1��0
                                        | SPI_SR_TFFF_MASK    //����FIFO����־λ�� д1����DMA���������ִ���FIFO���˾ͻ���0�� 0��ʾTx FIFO����
                                        | SPI_SR_RFOF_MASK    //����FIFO�����־λ��
                                        | SPI_SR_RFDF_MASK    //����FIFO��ı�־λ��д1����DMA���������ִ���FIFO���˾ͻ���0��0��ʾRx FIFO��
                                      );
  
  
  //    SPI_MCR_REG(M8266WIFI_INTERFACE_SPI) = ( 0
  //                                                | SPI_MCR_CLR_TXF_MASK     // Clear Tx FIFO Counter
  //                                                | SPI_MCR_CLR_RXF_MASK     // Clear Rx FIFO Counter
  //                                                | SPI_MCR_HALT_MASK        // Stop SPI Operation
  //                                                  );
  //    SPI_SR_REG(M8266WIFI_INTERFACE_SPI) = (0
  //                                                | SPI_SR_EOQF_MASK        // ���Ͷ��п��ˣ��������
  //                                                | SPI_SR_TFUF_MASK        // ����FIFO�����־λ��SPIΪ�ӻ�ģʽ��Tx FIFOΪ�գ����ⲿSPI����ģʽ�������䣬��־λ�ͻ���1��д1��0
  //                                                | SPI_SR_TFFF_MASK        // ����FIFO����־λ�� д1����DMA���������ִ���FIFO���˾ͻ���0�� 0��ʾTx FIFO����
  //                                                | SPI_SR_RFOF_MASK        // ����FIFO�����־λ��
  //                                                | SPI_SR_RFDF_MASK        // ����FIFO��ı�־λ��д1����DMA���������ִ���FIFO���˾ͻ���0��0��ʾRx FIFO��
  //                                          );
  
  SPIN[SPI_NUM]->MCR = ( 0
                                        |  SPI_MCR_MSTR_MASK        //Master,����ģʽ
                                        |  SPI_MCR_PCSIS_MASK
                                        );
  
  //    SPI_MCR_REG(M8266WIFI_INTERFACE_SPI)= (0
  //                                                |  SPI_MCR_MSTR_MASK      // Master,����ģʽ
  //                                                |  SPI_MCR_PCSIS_MASK
  //                                          );
  
  // searching the closet frequency scalers
  uint8  br,pbr;
  uint32 clk = 0; 
  //90000kHz = 90MHz =0.5 * 180MHz
  
  uint32 Scaler[] = {2,4,6,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};
  uint8  Prescaler[] = {2,3,5,7};
  uint32 fit_br=0,fit_pbr,min_diff =~0,diff;
  uint32 tmp;
  
  
  get_clk();//��ȡ�ں�ʱ�ӱ��ں�������
  clk = bus_clk_mhz * 1000000 / baud;
  
  for(br = 0;br < 0x10;br++)
  {
    for(pbr = 0;pbr < 4;pbr++)
    {
      tmp = Scaler[br] * Prescaler[pbr];
      diff = abs(tmp - clk);
      if(min_diff > diff)
      {
        //��ס �������
        min_diff = diff;
        fit_br = br;
        fit_pbr = pbr;
        
        if(min_diff == 0)	
          break;//�պ�ƥ��
      }
    }
    if(min_diff == 0)	
      break;//�պ�ƥ��
  }
  
  fit_clk =  bus_clk_mhz * 1000000 /(Scaler[fit_br] * Prescaler[fit_pbr]);
  
  //��Ҫ��һ�� CS �� CLK ��ʱ�䣨�����ʵİ����ڣ�,���������� (1<<(CSSCK + 1 ))*( 2*PCSSCK+1) = fclk /(2*  baud)
  clk =  bus_clk_mhz * 1000000 / fit_clk / 2;
  
//  min_diff = ~0;
//  fit_cssck = 0x0F;
//  fit_pcssck = 3;
//  
//  for(cssck = 0;cssck<=0x0F;cssck++)
//  {
//    tmp = 1<<( cssck + 1 ) ;
//    pcssck = (clk / tmp - 1) / 2;
//    if(pcssck > 3) continue;       //���ܳ��� 3
//    tmp = tmp * ( 2 * pcssck + 1 );
//    diff = abs( tmp - clk );
//    if( min_diff > diff )
//    {
//      //��ס �������
//      min_diff = diff;
//      fit_cssck = cssck;
//      fit_pcssck = pcssck;
//      
//      if(min_diff == 0)	break;//�պ�ƥ��
//    }
//  }
  
  //       for(br = 0;br < 0x10;br++)
  //       {
  //            for(pbr = 0;pbr < 4;pbr++)
  //            {
  //                tmp = Scaler[br] * Prescaler[pbr];
  //                diff = abs(tmp - clk);
  //                if(min_diff > diff)
  //                {
  //                    //��ס �������
  //                    min_diff = diff;
  //                    fit_br = br;
  //                    fit_pbr = pbr;
  //                    
  //                    if(min_diff == 0)
  //                    {
  //                        //�պ�ƥ��
  //                        goto SPI_CLK_EXIT;
  //                    }
  //                }
  //            }
  //       }
  //       SPI_CLK_EXIT:
  //            fit_clk =  bus_clk_khz *1000 /(Scaler[fit_br] * Prescaler[fit_pbr]);
 
  
  SPIN[SPI_NUM]->CTAR[M8266WIFI_SPI_USE_CATRx_No] = (0
      //| SPI_CTAR_DBR_MASK             //˫������ ������ DBR=1��CPHA=1��PBR=00����SCK Duty Cycle Ϊ 50/50
      //| SPI_CTAR_CPHA_MASK            //������SCK�����ظı䣨����������½��ر���׽�������ȡ���������0����֮��  w25x16�������ض�ȡ���ݣ�NRF24L01�������ض�ȡ����
      | SPI_CTAR_PBR(fit_pbr)           //�����ʷ�Ƶ�� ��0~3 ��Ӧ�ķ�ƵֵPrescalerΪ 2��3��5��7
      | SPI_CTAR_PDT(0x00)              //��ʱ����Ϊ PDT*2+1 ������PDTΪ3������ʱ����Ϊ7��PDTΪ2bit
      | SPI_CTAR_BR(fit_br)             //�����ʼ�����ֵ ,��BR<=3,��ƵScaler Ϊ 2*��BR+1�� ����BR>=3����ƵScaler Ϊ 2^BR  ��BRΪ4bit
      //SCK ������ = (Bus clk/Prescaler) x [(1+DBR)/Scaler ]          fSYS Ϊ Bus clock
      //50M / 2 x [1 / 2] = 25M   ��������������
      
      //| SPI_CTAR_CPOL_MASK            //ʱ�Ӽ��ԣ�1��ʾ SCK ����Ծ״̬Ϊ�ߵ�ƽ,   NRF24L01 ����ԾΪ�͵�ƽ
      | SPI_CTAR_FMSZ(0x07)             //ÿ֡���� 7bit+1 ����8bit ��FMSZĬ�Ͼ���8��
      // | SPI_CTAR_LSBFE_MASK          //1Ϊ��λ��ǰ��
        
      // �������������ǵ��� CS �ź����˵���һ��CLK��ʱ��
      //| SPI_CTAR_CSSCK(fit_cssck)     // x ��0~0x0F
      //| SPI_CTAR_PCSSCK(fit_pcssck)   //����Ƭѡ�ź���Ч��ʱ�ӵ�һ�����س��ֵ���ʱ��Ԥ��Ƶֵ��tcsc��ʱԤ��Ƶ 2*x+1�� x 0~3
      );
  
  
//  SPI_CTAR_REG(M8266WIFI_INTERFACE_SPI, M8266WIFI_SPI_USE_CATRx_No) 
//    = (0
//       //| SPI_CTAR_DBR_MASK       // ˫������ ������ DBR=1��CPHA=1��PBR=00����SCK Duty Cycle Ϊ 50/50
//       //| SPI_CTAR_CPHA_MASK      // ������SCK�����ظı䣨����������½��ر���׽�������ȡ���������0����֮��M8266WIFI �������ض�ȡ����
//       | SPI_CTAR_PBR(fit_pbr)   // �����ʷ�Ƶ�� ��0~3 ��Ӧ�ķ�ƵֵPrescalerΪ 2��3��5��7
//       | SPI_CTAR_PDT(0x00)      // ��ʱ����Ϊ PDT*2+1 ������PDTΪ3������ʱ����Ϊ7��PDTΪ2bit
//       | SPI_CTAR_BR(fit_br)     // �����ʼ�����ֵ ,��BR<=3,��ƵScaler Ϊ 2*��BR+1�� ����BR>=3����ƵScaler Ϊ 2^BR  ��BRΪ4bit
//       // SCK ������ = (Bus clk/Prescaler) x [(1+DBR)/Scaler ]
//       // 50M / 2  x [ 1  /  2  ] = 25M   ��������������
//       //| SPI_CTAR_CPOL_MASK      // ʱ�Ӽ��ԣ�1��ʾ SCK ����Ծ״̬Ϊ�ߵ�ƽ, 0 ����ԾΪ�͵�ƽ, M8266WIFI����ԾΪ�͵�ƽ
//       | SPI_CTAR_FMSZ(0x07)     // ÿ֡���� 7bit+1 ����8bit ��FMSZĬ�Ͼ���8��
//         // | SPI_CTAR_LSBFE_MASK     // 1Ϊ��λ��ǰ��M8266WIFIΪMSB First
//         // | SPI_CTAR_CSSCK(1)       //
//         // | SPI_CTAR_PCSSCK(2)      // ����Ƭѡ�ź���Ч��ʱ�ӵ�һ�����س��ֵ���ʱ��Ԥ��Ƶֵ��tcsc��ʱԤ��Ƶ 2*x+1��
//         );
  
  
  SPIN[SPI_NUM]->MCR &= ~SPI_MCR_HALT_MASK;     //����SPI���䡣1Ϊ��ͣ��0Ϊ����
//  SPI_MCR_REG(M8266WIFI_INTERFACE_SPI) &= ~SPI_MCR_HALT_MASK;     //����SPI���䡣1Ϊ��ͣ��0Ϊ����
  
  return fit_clk;
  
}


/***********************************************************************************
* M8266HostIf_SPI_SetSpeed                                                        *
* Description                                                                     *
*    To setup the SPI Clock Speed for M8266WIFI module                            *
* Parameter(s):                                                                   *
*    SPI_BaudRatePrescaler: SPI BaudRate Prescaler                                *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/
void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler)
{
} 

/***********************************************************************************
* M8266HostIf_Init                                                                *
* Description                                                                     *
*    To initialise the Host interface for M8266WIFI module                        *
* Parameter(s):                                                                   *
*    baud: baud rate to set                                                       *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/ 
void M8266HostIf_Init(void)
{
  uint32 clk=0;
  M8266HostIf_GPIO_CS_RESET_Init();
  //    printf("fit clk:%ld\r\n",spi_init(spi1,SPI_PCS1, MASTER,225*100*1000));  //22,500,000 = 90,000,000 / 4
  clk = M8266HostIf_SPI_Init(15000000);  // 15 MHz  // = 60 / 2*2
  printf("fit clk:%ld\r\n",clk);
}

//////////////////////////////////////////////////////////////////////////////////////
// BELOW FUNCTIONS ARE REQUIRED BY M8266WIFIDRV.LIB. 
// PLEASE IMPLEMENTE THEM ACCORDING TO YOUR HARDWARE
//////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* M8266HostIf_Set_nRESET_Pin                                                      *
* Description                                                                     *
*    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI nRESET                *
*    You may update the macros of GPIO PIN usages for nRESET from brd_cfg.h       *
*    You are not recommended to modify codes below please                         *
* Parameter(s):                                                                   *
*    1. level: LEVEL output to nRESET pin                                         *
*              0 = output LOW  onto nRESET                                        *
*              1 = output HIGH onto nRESET                                        *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/
void M8266HostIf_Set_nRESET_Pin(u8 level)
{
  if(level!=0)
    gpio_set(M8266WIFI_nRESET_GPIO, 1);
  else
    gpio_set(M8266WIFI_nRESET_GPIO, 0);
}
/***********************************************************************************
* M8266HostIf_Set_SPI_nCS_PIN                                                     *
* Description                                                                     *
*    To Outpout HIGH or LOW onto the GPIO pin for M8266WIFI SPI nCS               *
*    You may update the macros of GPIO PIN usages for SPI nCS from brd_cfg.h      *
*    You are not recommended to modify codes below please                         *
* Parameter(s):                                                                   *
*    1. level: LEVEL output to SPI nCS pin                                        *
*              0 = output LOW  onto SPI nCS                                       *
*              1 = output HIGH onto SPI nCS                                       *
* Return:                                                                         *
*    None                                                                         *
***********************************************************************************/
void M8266HostIf_Set_SPI_nCS_Pin(u8 level)
{
  if(level!=0)
    gpio_set(M8266WIFI_nCS_GPIO, 1);
  else
    gpio_set(M8266WIFI_nCS_GPIO, 0);
}

/***********************************************************************************
* M8266WIFIHostIf_delay_us                                                        *
* Description                                                                     *
*    To loop delay some micro seconds.                                            *
* Parameter(s):                                                                   *
*    1. nus: the micro seconds to delay                                           *
* Return:                                                                         *
*    none                                                                         *
***********************************************************************************/
void M8266HostIf_delay_us(u8 nus)
{
  systick_delay_us(nus);
}

/***********************************************************************************
* M8266HostIf_SPI_ReadWriteByte                                                   *
* Description                                                                     *
*    To write a byte onto the SPI bus from MCU MOSI to the M8266WIFI module       *
*    and read back a byte from the SPI bus MISO meanwhile                         *
*    You may update the macros of SPI usage from brd_cfg.h                        *
* Parameter(s):                                                                   *
*    1. TxdByte: the byte to be sent over MOSI                                    *
* Return:                                                                         *
*    1. The byte read back from MOSI meanwhile                                    *                                                                         *
***********************************************************************************/
u8 M8266HostIf_SPI_ReadWriteByte(u8 TxdByte)
{
  u8 data;
  //   spi_mosi(SPI1,SPIn_PCS0,&TxdByte,&data,1); 
  spi_mosi(spi1,SPI_PCS1,&TxdByte,&data,1);
  return data;
}
