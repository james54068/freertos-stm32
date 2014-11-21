#ifndef __USART_H
#define __USART_H
#include "main.h"

void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART1_Configuration(void);
void DMA2_Stream5_Configuration(void);
void USART1_puts(char* s);
void NVIC_Configuration(void);

extern char	string[10];
extern FunctionalState storestate;

#endif