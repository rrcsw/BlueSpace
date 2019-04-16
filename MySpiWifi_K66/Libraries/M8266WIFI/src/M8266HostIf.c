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


extern SPI_Type* SPIN[3]; //定义三个指针数组保存 SPIx 的地址

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
                                        | SPI_MCR_CLR_TXF_MASK     //清空 Tx FIFO 计数器
                                        | SPI_MCR_CLR_RXF_MASK     //清空 Rx FIFO 计数器
                                        | SPI_MCR_HALT_MASK        //停止SPI传输
                                        );
  
  //-----------------------清标志位-------------------------
  SPIN[SPI_NUM]->SR = ( 0
                                        | SPI_SR_EOQF_MASK    //发送队列空了，发送完毕
                                        | SPI_SR_TFUF_MASK    //传输FIFO下溢标志位，SPI为从机模式，Tx FIFO为空，而外部SPI主机模式启动传输，标志位就会置1，写1清0
                                        | SPI_SR_TFFF_MASK    //传输FIFO满标志位。 写1或者DMA控制器发现传输FIFO满了就会清0。 0表示Tx FIFO满了
                                        | SPI_SR_RFOF_MASK    //接收FIFO溢出标志位。
                                        | SPI_SR_RFDF_MASK    //接收FIFO损耗标志位，写1或者DMA控制器发现传输FIFO空了就会清0。0表示Rx FIFO空
                                      );
  
  
  //    SPI_MCR_REG(M8266WIFI_INTERFACE_SPI) = ( 0
  //                                                | SPI_MCR_CLR_TXF_MASK     // Clear Tx FIFO Counter
  //                                                | SPI_MCR_CLR_RXF_MASK     // Clear Rx FIFO Counter
  //                                                | SPI_MCR_HALT_MASK        // Stop SPI Operation
  //                                                  );
  //    SPI_SR_REG(M8266WIFI_INTERFACE_SPI) = (0
  //                                                | SPI_SR_EOQF_MASK        // 发送队列空了，发送完毕
  //                                                | SPI_SR_TFUF_MASK        // 传输FIFO下溢标志位，SPI为从机模式，Tx FIFO为空，而外部SPI主机模式启动传输，标志位就会置1，写1清0
  //                                                | SPI_SR_TFFF_MASK        // 传输FIFO满标志位。 写1或者DMA控制器发现传输FIFO满了就会清0。 0表示Tx FIFO满了
  //                                                | SPI_SR_RFOF_MASK        // 接收FIFO溢出标志位。
  //                                                | SPI_SR_RFDF_MASK        // 接收FIFO损耗标志位，写1或者DMA控制器发现传输FIFO空了就会清0。0表示Rx FIFO空
  //                                          );
  
  SPIN[SPI_NUM]->MCR = ( 0
                                        |  SPI_MCR_MSTR_MASK        //Master,主机模式
                                        |  SPI_MCR_PCSIS_MASK
                                        );
  
  //    SPI_MCR_REG(M8266WIFI_INTERFACE_SPI)= (0
  //                                                |  SPI_MCR_MSTR_MASK      // Master,主机模式
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
  
  
  get_clk();//获取内核时钟便于后面设置
  clk = bus_clk_mhz * 1000000 / baud;
  
  for(br = 0;br < 0x10;br++)
  {
    for(pbr = 0;pbr < 4;pbr++)
    {
      tmp = Scaler[br] * Prescaler[pbr];
      diff = abs(tmp - clk);
      if(min_diff > diff)
      {
        //记住 最佳配置
        min_diff = diff;
        fit_br = br;
        fit_pbr = pbr;
        
        if(min_diff == 0)	
          break;//刚好匹配
      }
    }
    if(min_diff == 0)	
      break;//刚好匹配
  }
  
  fit_clk =  bus_clk_mhz * 1000000 /(Scaler[fit_br] * Prescaler[fit_pbr]);
  
  //需要算一下 CS 到 CLK 的时间（波特率的半周期）,满足条件： (1<<(CSSCK + 1 ))*( 2*PCSSCK+1) = fclk /(2*  baud)
  clk =  bus_clk_mhz * 1000000 / fit_clk / 2;
  
//  min_diff = ~0;
//  fit_cssck = 0x0F;
//  fit_pcssck = 3;
//  
//  for(cssck = 0;cssck<=0x0F;cssck++)
//  {
//    tmp = 1<<( cssck + 1 ) ;
//    pcssck = (clk / tmp - 1) / 2;
//    if(pcssck > 3) continue;       //不能超过 3
//    tmp = tmp * ( 2 * pcssck + 1 );
//    diff = abs( tmp - clk );
//    if( min_diff > diff )
//    {
//      //记住 最佳配置
//      min_diff = diff;
//      fit_cssck = cssck;
//      fit_pcssck = pcssck;
//      
//      if(min_diff == 0)	break;//刚好匹配
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
  //                    //记住 最佳配置
  //                    min_diff = diff;
  //                    fit_br = br;
  //                    fit_pbr = pbr;
  //                    
  //                    if(min_diff == 0)
  //                    {
  //                        //刚好匹配
  //                        goto SPI_CLK_EXIT;
  //                    }
  //                }
  //            }
  //       }
  //       SPI_CLK_EXIT:
  //            fit_clk =  bus_clk_khz *1000 /(Scaler[fit_br] * Prescaler[fit_pbr]);
 
  
  SPIN[SPI_NUM]->CTAR[M8266WIFI_SPI_USE_CATRx_No] = (0
      //| SPI_CTAR_DBR_MASK             //双波特率 ，假设 DBR=1，CPHA=1，PBR=00，得SCK Duty Cycle 为 50/50
      //| SPI_CTAR_CPHA_MASK            //数据在SCK上升沿改变（输出），在下降沿被捕捉（输入读取）。如果是0，则反之。  w25x16在上升沿读取数据；NRF24L01在上升沿读取数据
      | SPI_CTAR_PBR(fit_pbr)           //波特率分频器 ，0~3 对应的分频值Prescaler为 2、3、5、7
      | SPI_CTAR_PDT(0x00)              //延时因子为 PDT*2+1 ，这里PDT为3，即延时因子为7。PDT为2bit
      | SPI_CTAR_BR(fit_br)             //波特率计数器值 ,当BR<=3,分频Scaler 为 2*（BR+1） ，当BR>=3，分频Scaler 为 2^BR  。BR为4bit
      //SCK 波特率 = (Bus clk/Prescaler) x [(1+DBR)/Scaler ]          fSYS 为 Bus clock
      //50M / 2 x [1 / 2] = 25M   这里以最大的来算
      
      //| SPI_CTAR_CPOL_MASK            //时钟极性，1表示 SCK 不活跃状态为高电平,   NRF24L01 不活跃为低电平
      | SPI_CTAR_FMSZ(0x07)             //每帧传输 7bit+1 ，即8bit （FMSZ默认就是8）
      // | SPI_CTAR_LSBFE_MASK          //1为低位在前。
        
      // 下面两个参数是调整 CS 信号来了到第一个CLK的时间
      //| SPI_CTAR_CSSCK(fit_cssck)     // x ：0~0x0F
      //| SPI_CTAR_PCSSCK(fit_pcssck)   //设置片选信号有效到时钟第一个边沿出现的延时的预分频值。tcsc延时预分频 2*x+1； x 0~3
      );
  
  
//  SPI_CTAR_REG(M8266WIFI_INTERFACE_SPI, M8266WIFI_SPI_USE_CATRx_No) 
//    = (0
//       //| SPI_CTAR_DBR_MASK       // 双波特率 ，假设 DBR=1，CPHA=1，PBR=00，得SCK Duty Cycle 为 50/50
//       //| SPI_CTAR_CPHA_MASK      // 数据在SCK上升沿改变（输出），在下降沿被捕捉（输入读取）。如果是0，则反之。M8266WIFI 在上升沿读取数据
//       | SPI_CTAR_PBR(fit_pbr)   // 波特率分频器 ，0~3 对应的分频值Prescaler为 2、3、5、7
//       | SPI_CTAR_PDT(0x00)      // 延时因子为 PDT*2+1 ，这里PDT为3，即延时因子为7。PDT为2bit
//       | SPI_CTAR_BR(fit_br)     // 波特率计数器值 ,当BR<=3,分频Scaler 为 2*（BR+1） ，当BR>=3，分频Scaler 为 2^BR  。BR为4bit
//       // SCK 波特率 = (Bus clk/Prescaler) x [(1+DBR)/Scaler ]
//       // 50M / 2  x [ 1  /  2  ] = 25M   这里以最大的来算
//       //| SPI_CTAR_CPOL_MASK      // 时钟极性，1表示 SCK 不活跃状态为高电平, 0 不活跃为低电平, M8266WIFI不活跃为低电平
//       | SPI_CTAR_FMSZ(0x07)     // 每帧传输 7bit+1 ，即8bit （FMSZ默认就是8）
//         // | SPI_CTAR_LSBFE_MASK     // 1为低位在前。M8266WIFI为MSB First
//         // | SPI_CTAR_CSSCK(1)       //
//         // | SPI_CTAR_PCSSCK(2)      // 设置片选信号有效到时钟第一个边沿出现的延时的预分频值。tcsc延时预分频 2*x+1；
//         );
  
  
  SPIN[SPI_NUM]->MCR &= ~SPI_MCR_HALT_MASK;     //启动SPI传输。1为暂停，0为启动
//  SPI_MCR_REG(M8266WIFI_INTERFACE_SPI) &= ~SPI_MCR_HALT_MASK;     //启动SPI传输。1为暂停，0为启动
  
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
