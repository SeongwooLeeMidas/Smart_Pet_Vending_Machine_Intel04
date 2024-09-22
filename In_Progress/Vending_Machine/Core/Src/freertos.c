/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_PORT GPIOA
#define MOTOR_PIN_1 GPIO_PIN_0
#define MOTOR_PIN_2 GPIO_PIN_1
#define MOTOR_PIN_3 GPIO_PIN_4
#define MOTOR_PIN_4 GPIO_PIN_5
#define MOTOR_PIN_5 GPIO_PIN_6
#define MOTOR_PIN_6 GPIO_PIN_7
#define NUM_MOTOR_PINS 6

#define IR_PORT GPIOB
#define IR_PIN_1 GPIO_PIN_0
#define IR_PIN_2 GPIO_PIN_1
#define IR_PIN_3 GPIO_PIN_2
#define IR_PIN_4 GPIO_PIN_13
#define IR_PIN_5 GPIO_PIN_14
#define IR_PIN_6 GPIO_PIN_15
#define NUM_IR_PINS 6

#define SAMPLE_COUNT 5

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#define ARR_CNT 5
#define CMD_SIZE 50
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern volatile unsigned char rx2Flag;
extern volatile char rx2Data[50];
extern volatile unsigned char btFlag;
extern char btData[50];

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
///////////// IR Sensor//////////////////
GPIO_PinState currentIrValues[NUM_IR_PINS];
GPIO_PinState previousIrValues[NUM_IR_PINS];
uint16_t irPins[NUM_IR_PINS] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6};
uint16_t motorPins[NUM_MOTOR_PINS] = {MOTOR_PIN_1, MOTOR_PIN_2, MOTOR_PIN_3, MOTOR_PIN_4, MOTOR_PIN_5, MOTOR_PIN_6};
GPIO_PinState sampleValues[NUM_IR_PINS][SAMPLE_COUNT];
int sampleIndex[NUM_IR_PINS] = {0};
/* USER CODE END Variables */
osThreadId UART_TaskHandle;
osThreadId IR_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void bluetooth_Event(void);
GPIO_PinState majorityVote(GPIO_PinState* values, int size);
/* USER CODE END FunctionPrototypes */

void UART_Task_Func(void const * argument);
void IR_Task_Func(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of UART_Task */
  osThreadDef(UART_Task, UART_Task_Func, osPriorityNormal, 0, 256);
  UART_TaskHandle = osThreadCreate(osThread(UART_Task), NULL);

  /* definition and creation of IR_Task */
  osThreadDef(IR_Task, IR_Task_Func, osPriorityHigh, 0, 256);
  IR_TaskHandle = osThreadCreate(osThread(IR_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_UART_Task_Func */
/**
  * @brief  Function implementing the UART_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_UART_Task_Func */
void UART_Task_Func(void const * argument)
{
  /* USER CODE BEGIN UART_Task_Func */
  /* Infinite loop */
  for(;;)
  {
		UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		printf("UART Task stack high watermark: %lu\r\n", uxHighWaterMark);
  	if(rx2Flag)
		{
			printf("recv2 : %s\r\n",rx2Data);
			rx2Flag =0;
	//	    HAL_UART_Transmit(&huart6, (uint8_t *)buf, strlen(buf), 0xFFFF);
		}
		if(btFlag)
		{
//		printf("bt : %s\r\n",btData);
			btFlag =0;
			bluetooth_Event();
		}
    osDelay(500);
  }
  /* USER CODE END UART_Task_Func */
}

/* USER CODE BEGIN Header_IR_Task_Func */
/**
* @brief Function implementing the IR_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IR_Task_Func */
void IR_Task_Func(void const * argument)
{
  /* USER CODE BEGIN IR_Task_Func */

  /* Infinite loop */
  for(;;)
  {
  	UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);  // ?��?�� ?��?��?��?�� ?��?�� ?��?��?��
  	printf("IR Task stack high watermark: %lu\r\n", uxHighWaterMark);
  	/*
  	for (int i = 0; i < NUM_IR_PINS; i++) {
			sampleValues[i][sampleIndex[i]] = HAL_GPIO_ReadPin(IR_PORT, irPins[i]);
			sampleIndex[i] = (sampleIndex[i] + 1) % SAMPLE_COUNT;

			if (sampleIndex[i] == 0) {
				GPIO_PinState majorityValue = majorityVote(sampleValues[i], SAMPLE_COUNT);
				previousIrValues[i] = currentIrValues[i];
				currentIrValues[i] = majorityValue;
				//printf("1:%d, 2:%d, 3:%d, 4:%d, 5:%d, 6:%d\r\n",currentIrValues[0],currentIrValues[1],currentIrValues[2],currentIrValues[3],currentIrValues[4],currentIrValues[5]);
				if (previousIrValues[i] == GPIO_PIN_RESET && currentIrValues[i] == GPIO_PIN_SET) {
					printf("Motor %d OFF\r\n",i);
					HAL_GPIO_WritePin(MOTOR_PORT, motorPins[i], GPIO_PIN_RESET);
				}
			}
		}
		*/

  	osDelay(50);
  }
  /* USER CODE END IR_Task_Func */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void PrintTaskStatus(void)
{
    char taskList[512];
    vTaskList(taskList);
    printf("Task\t\tState\tPrio\tStack\tID\r\n");
    printf("%s\r\n", taskList);
}
GPIO_PinState majorityVote(GPIO_PinState *samples, int count) {
  int highCount = 0;
  for (int i = 0; i < count; i++) {
    if (samples[i] == GPIO_PIN_SET) {
      highCount++;
    }
  }
  return (highCount > count / 2) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void bluetooth_Event()
{

  int i=0;
  int motorNumber = 0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  size_t recvLength = strlen(btData);
  char *recvBuf = pvPortMalloc(recvLength+1);
  char *sendBuf = pvPortMalloc(recvLength+1);
  strcpy(recvBuf,btData);
  printf("btData : %s\r\n",btData);

  pToken = strtok(recvBuf,"[@]");
  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }

  if(!strcmp(pArray[1],"MOTOR"))
  {
  	motorNumber = atoi(pArray[2]);
  	if(motorNumber >= 1 && motorNumber <= NUM_MOTOR_PINS) {
				//HAL_GPIO_WritePin(MOTOR_PORT, motorPins[motorNumber - 1], GPIO_PIN_SET);
				printf("MOTOR %d ON\r\n",motorNumber);
				UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
				printf("BT Func stack high watermark: %lu\r\n", uxHighWaterMark);
			  if(uxHighWaterMark == 0)
			  {
			  	printf("Stack overflow detected in task: BT Func\r\n");
			  }
		}
  }
  else if(!strncmp(pArray[1]," New conn",sizeof(" New conn")))
  {
      return;
  }
  else if(!strncmp(pArray[1]," Already log",sizeof(" Already log")))
  {
      return;
  }
  else
      return;

  sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
  HAL_UART_Transmit(&huart6, (uint8_t *)sendBuf, strlen(sendBuf), 100);
}
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART6 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END Application */
