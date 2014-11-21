#include "usart.h"
#include "main.h"
#include <string.h>


char    string[10];
u8      U1_RxData;
u8      buf[10];
int     i=0;
FunctionalState storestate;

void RCC_Configuration(void)
{
      /* --------------------------- System Clocks Configuration -----------------*/
      /* USART1 clock enable */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      /* GPIOA clock enable */
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*-------------------------- GPIO Configuration ----------------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Connect USART pins to AF */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);   // USART1_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);  // USART1_RX
}

void USART1_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* USARTx configuration ------------------------------------------------------*/
    /* USARTx configured as follow:
     *  - BaudRate = 115200 baud
     *  - Word Length = 8 Bits
     *  - One Stop Bit
     *  - No parity
     *  - Hardware flow control disabled (RTS and CTS signals)
     *  - Receive and transmit enabled
     */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                  ///< Enable USART2 receiver interrupt 
}

void DMA2_Stream7_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    /*Enable DMA1 clock*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
    /*Configure DMA2 steram7*/
    DMA_DeInit(DMA2_Stream7);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)string;
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_BASE+0x04;
    DMA_InitStructure.DMA_BufferSize = strlen(string);
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /*Enable DMA1 channel4*/
    DMA_Cmd(DMA2_Stream7, ENABLE);
}

void USART1_puts(char* s)
{
    while(*s) {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *s);
        s++;
    }
}

void NVIC_Configuration(void)
{ 
     NVIC_InitTypeDef NVIC_InitStructure;                                                                                                                                                                               
  #ifdef  VECT_TAB_RAM                                                                                          
    /* Set the Vector Table base location at 0x20000000 */                                                                                        
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);                                                                                         
  #else  /* VECT_TAB_FLASH  */                                                                                        
    /* Set the Vector Table base location at 0x08000000 */                                                                                        
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);                                                                                           
  #endif  
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /*preemption:2 sub:8*/

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                                                                                       
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                                                                                        
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                                                                       
  NVIC_Init(&NVIC_InitStructure);   

}


void USART1_IRQHandler(void)                
{ 

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {

      U1_RxData = USART_ReceiveData(USART1); //Get Data
    
      if(U1_RxData=='\n')
      {
        strcpy(string,buf);
        storestate = ENABLE;
        USART1_puts(buf);
        i=0;
        memset(buf,0,strlen(buf));      
      }
      else
      {
        buf[i]= U1_RxData;
        i++;
      }
 
      //USART_SendData(USART1,U1_RxData);   //Send back the Rx Data
    

    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)  ;
  }
  else
  {
  U1_RxData =USART_ReceiveData(USART1); //Get Data Á×§K¦]¬°¹L¸ü¤¤Â_²£¥Í·í¾÷¦ý¬OÁÙ¬O¤£§¹¾ã                   
  }
}