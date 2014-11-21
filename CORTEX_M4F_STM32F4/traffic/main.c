/**
  ******************************************************************************
  * @file    Template/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "main.h"
#include "draw_graph.h"
#include "move_car.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "shell.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xQueueHandle t_queue; /* Traffic light queue. */
xQueueHandle t_mutex; /* Traffic light mutex. */

static int traffic_index = 0; 
static int button_change_traffic = 0;
static int states[] = {TRAFFIC_RED, TRAFFIC_YELLOW, TRAFFIC_GREEN, 
							TRAFFIC_YELLOW};

	

void
prvInit()
{
	//LCD init
	LCD_Init();
	IOE_Config();
	LTDC_Cmd( ENABLE );

	LCD_LayerInit();
	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	LCD_Clear( LCD_COLOR_BLACK );
	LCD_SetTextColor( LCD_COLOR_WHITE );

	//Button
	STM_EVAL_PBInit( BUTTON_USER, BUTTON_MODE_GPIO );
}

void send_byte(uint8_t b)
{
  /* Send one byte */
  USART_SendData(USART1, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

int _write (int fd, char *ptr, int len)
{
  /* Write "len" of char from "ptr" to file id "fd"
   * Return number of char written.
   * Need implementing with UART here. */
  
  for ( int i = 0; i<len ;i++)
  {
    send_byte(*ptr);
    ptr++;
  }
  return len;
}


static void GetTrafficState(int change_state, int *v_state, int *h_state)
{

	switch (change_state) {
	case TRAFFIC_RED:
		*v_state = TRAFFIC_RED;
		*h_state = TRAFFIC_GREEN;
		break;
	case TRAFFIC_YELLOW:
		if (*v_state == TRAFFIC_GREEN)
			*v_state = TRAFFIC_YELLOW;
		else
			*h_state = TRAFFIC_YELLOW;
		break;
	case TRAFFIC_GREEN:
		*v_state = TRAFFIC_GREEN;
		*h_state = TRAFFIC_RED;
		break;
	default:
		ReportError("out of range");
		break;
	}
}

static void DrawGraphTask( void *pvParameters)
{
	const portTickType ticks = 100 / portTICK_RATE_MS;
	int value;
	int traffic_v_state = TRAFFIC_GREEN;
	int traffic_h_state = TRAFFIC_RED;

	portBASE_TYPE status;

	DrawBackground();

	while ( 1 ) {
		/*
		 * Check if the traffic changed event is sent to
		 * the queue. If so, we need to change the traffic
		 * light.
		 */
		status = xQueueReceive(t_queue, &value, ticks);

		if (status == pdPASS) {
			GetTrafficState(value, &traffic_v_state, 
						&traffic_h_state);
		}

		MoveCar(traffic_v_state, traffic_h_state);
	}
}

static void ChgTrafficLightTask(void *pvParameters)
{
	int num_ticks;
	int states_num = sizeof(states) / sizeof(states[0]);

	portBASE_TYPE status;
	portTickType ticks = TRAFFIC_GREEN_TICK;

	while ( 1 ) {
		ticks = (states[traffic_index] == TRAFFIC_YELLOW ? 
			TRAFFIC_YELLOW_TICK : TRAFFIC_GREEN_TICK);

		num_ticks = ticks / TRAFFIC_TICK_SLICE;

		status = xQueueSendToBack(t_queue, &states[traffic_index++], 0);
	
		if (status != pdPASS)
			ReportError("Cannot send to the queue!");

		if (traffic_index >= states_num)
			traffic_index = 0;

		while (num_ticks--) { 
			xSemaphoreTake(t_mutex, portMAX_DELAY);
			
			if (button_change_traffic) {
				button_change_traffic = 0;
				xSemaphoreGive(t_mutex);
				break;
			}

			xSemaphoreGive(t_mutex);

			vTaskDelay(TRAFFIC_TICK_SLICE);
		}
	}
}

static void ButtonEventTask(void *pvParameters)
{
	while (1) {
		if( STM_EVAL_PBGetState( BUTTON_USER ) ){

			while( STM_EVAL_PBGetState( BUTTON_USER ) );

			xSemaphoreTake(t_mutex, portMAX_DELAY);
			button_change_traffic = 1;
			xSemaphoreGive(t_mutex);
		}
	}
}

static void ShellTask(void *pvParameters)
{
	char *argv[20];		/*fetch command variable*/	

	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 10;

    //Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    printf("\rWelcome to FreeRTOS Shell\r\n");

	while (1) {
        
        if(storestate == ENABLE)
        {
        	printf("\r\n");
        	int n=parse_command(string, argv);

			/* will return pointer to the command function */
			cmdfunc *fptr=do_command(argv[0]);
			
			if(fptr!=NULL)
				fptr(n, argv);
			else
			printf("\rcommand not found.\r\n");

			storestate = DISABLE;
        }	
		
		//USART1_puts(string);
     // Wait for the next cycle.
     vTaskDelayUntil( &xLastWakeTime, xFrequency );

      // Perform action here.
		
	}
}


//Main Function
int main(void)
{
	t_queue = xQueueCreate(1, sizeof(int));
	if (!t_queue) {
		ReportError("Failed to create t_queue");
		while(1);
	}

	t_mutex = xSemaphoreCreateMutex();
	if (!t_mutex) {
		ReportError("Failed to create t_mutex");
		while(1);
	}

	prvInit();

	xTaskCreate(ChgTrafficLightTask, "Traffic Light Task", 256, 
			( void * ) NULL, tskIDLE_PRIORITY + 1, NULL);

	xTaskCreate(ButtonEventTask, (char *) "Button Event Task", 256,
		   	NULL, tskIDLE_PRIORITY + 1, NULL);

	xTaskCreate(DrawGraphTask, (char *) "Draw Graph Task", 256,
		   	NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(ShellTask, (char *) "Shell Task", 256,
		   	NULL, tskIDLE_PRIORITY + 1, NULL);


	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

    RNG_Cmd(ENABLE);

    RCC_Configuration();
    GPIO_Configuration(); 
    USART1_Configuration(); 
    NVIC_Configuration();
    //DMA2_Stream7_Configuration();
    //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
   

	//Call Scheduler
	vTaskStartScheduler();
}

