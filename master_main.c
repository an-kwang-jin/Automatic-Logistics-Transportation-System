/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
//#include "clcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#.project_controller system
#define UART_TIMEOUT	1000
#define AUTO_RELOAD_MAX	65535
#define SPI_BUFFER_SIZE 100
#define UART_BUFFER_SIZE 100
#define BARCODE_BUFFER_SIZE 20
//------+++ultrasonic waves-------------------
#define TRIG_PIN GPIO_PIN_1
#define TRIG_PORT GPIOC
//#define MASTER_LED_PIN GPIO_PIN_0
//#define MASTER_LED_PORT GPIOC
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//#ifdef __GNUC__
//  /* With GCC, small printf (option LD Linker->Libraries->Small printf
//     set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
//
//  return ch;
//}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
//------ULTRA WAVES
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  // is the first value captured ?
uint8_t Distance  = 0;

//#project_CONTROLLER system
//------uart2(Terminal Serial Monitor) intro0
volatile uint8_t Intro_TxBuffer[]="\n\n\n\n----------------------------------------------------------------------\n"
		"----------------------------------------------------------------------\n"
		"----------------------------------------------------------------------\n"
		"\nHello, Embedded System Class.\n\nTITLE : <UART&SPI_Controller>.\n\nTeam 5_Project.\n"
		"\n----------------------------------------------------------------------\n"
		"----------------------------------------------------------------------\n"
		"----------------------------------------------------------------------\n\n\n";
//------uart2(Terminal Serial Monitor) intro1
uint8_t msg1[]="****HOW TO USE?****\n";
uint8_t msg2[]="Follow 3 steps to use the system @MASTER DEVICE\n";
uint8_t msg3[]="--------------------\n";
uint8_t msg4[]="1. Connect with your phone.(@Blue-Tooth)\n";
uint8_t msg5[]="--------------------\n";
uint8_t msg6[]="2. Enter Your Name.\n";
uint8_t msg7[]="--------------------\n";
uint8_t msg8[]="3. Enter the PASSWORD(last Student number ****).\n";
uint8_t msg9[]="-------------------\n";
uint8_t msg10[]="...................\n\n\n\n";

//uart2 : terminal serial monitor & stm32 board
uint8_t UART_RxBuffer[UART_BUFFER_SIZE];	//Uart2 Receive Buffer
uint8_t UART_Input_Buffer[UART_BUFFER_SIZE]; //data of input(intro)
uint8_t UART_TxBuffer[UART_BUFFER_SIZE];	//Uart2 Transmit Buffer
volatile uint16_t RxIndex =0; //current location
volatile uint8_t DataReady =0;	//input_OK Flag

// UART3 : Bluetooth & stm32 board , Buffers & MOTOR CONTROL
uint8_t RxBuffer1[UART_BUFFER_SIZE];	//UART3 buffer_motor

//uart2 & spi2 : Master -> Slave1
uint8_t SPI2_TxBuffer[SPI_BUFFER_SIZE] = "HELLO FROM SPI2 MASTER";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
//--------------ULTRASONIC WAVE-----------------------------------
void delay (uint16_t time){
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while (__HAL_TIM_GET_COUNTER (&htim2) < time);
}
void HCSR04_Read (void){
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//#.project_CONTROLLER system
//-------terminal serial monitor display----
void display_serial_monitor_intro(){
	HAL_UART_Transmit(&huart2, msg1, sizeof(msg1), 1000);
	HAL_UART_Transmit(&huart2, msg2, sizeof(msg2), 1000);
	HAL_UART_Transmit(&huart2, msg3, sizeof(msg3), 1000);
	HAL_UART_Transmit(&huart2, msg4, sizeof(msg4), 1000);
	HAL_UART_Transmit(&huart2, msg5, sizeof(msg5), 1000);
	HAL_UART_Transmit(&huart2, msg6, sizeof(msg6), 1000);
	HAL_UART_Transmit(&huart2, msg7, sizeof(msg7), 1000);
	HAL_UART_Transmit(&huart2, msg8, sizeof(msg8), 1000);
	HAL_UART_Transmit(&huart2, msg9, sizeof(msg9), 1000);
	HAL_UART_Transmit(&huart2, msg10, sizeof(msg10), 1000);
}
//-------terminal serial USART2 communication-----
void UART_sendMSG(const uint8_t *MSG){
	HAL_UART_Transmit(&huart2, (uint8_t *)MSG, strlen(MSG), 1000);
}
//-------BlueTooth APP USART3 communication-----
void UART_BLUETOOTH_sendMSG(const uint8_t *MSG){
	HAL_UART_Transmit(&huart3, (uint8_t *)MSG, strlen(MSG), 1000);
}
//-------------------------UART2---------------------------------------
//------- USART2 INPUT SYNTAX!!!!!-----
uint8_t validate_name_input(const uint8_t *input){
    if ((strcmp((char *)input, "AKJ") == 0)||(strcmp((char *)input, "akj") == 0)) return 1;
    else return 0;
}
//input SCHOOL ID(password ****) only number
uint8_t validate_password_input(const uint8_t *input){
	if(strlen(input)!=4)	return 0;
	for(size_t i=0; i<strlen(input); i++){
		if(!isdigit(input[i]))	return 0;
	}
    if (strcmp((char *)input, "0495") == 0) return 1;
    else return 0;
}
//input BARCODE(13LENGTH)
uint8_t validate_barcode_input(const uint8_t *input){
	if(strlen(input)!=13)	return 0;	//barcode string length == 13
	for(size_t i=0; i<strlen(input); i++){
		if(!isdigit(input[i]))	return 0;
	}
	return 1;
}
void display_barcode_info(const uint8_t *input){
	if (strcmp((char *)input, "8801062318551") == 0){
		UART_sendMSG("XYLITOR GUM, A-section.\n\n\n");
		UART_BLUETOOTH_sendMSG("XYLITOR GUM, A-section.\n\n");
        memset(SPI2_TxBuffer, 0, SPI_BUFFER_SIZE);
        snprintf((char *)SPI2_TxBuffer, SPI_BUFFER_SIZE, "XYLITOR GUM, A-section");
	}
	else if (strcmp((char *)input, "8801117636401") == 0){
		UART_sendMSG("WOW BLUE GUM, B-section.\n\n\n");
		UART_BLUETOOTH_sendMSG("WOW BLUE GUM, B-section.\n\n");
        memset(SPI2_TxBuffer, 0, SPI_BUFFER_SIZE);
        snprintf((char *)SPI2_TxBuffer, SPI_BUFFER_SIZE, "WOW BLUE GUM, B-section");
	}
	else if (strcmp((char *)input, "8801062320639") == 0){
		UART_sendMSG("JUICY FRESH GUM, C-section.\n\n\n");
		UART_BLUETOOTH_sendMSG("JUICY FRESH GUM, C-section.\n\n");
        memset(SPI2_TxBuffer, 0, SPI_BUFFER_SIZE);
        snprintf((char *)SPI2_TxBuffer, SPI_BUFFER_SIZE, "JUICY FRESH GUM, C-section");
	}
	else {
		UART_sendMSG("This product has not been registered.\n\n");
		UART_BLUETOOTH_sendMSG("This product has not been registered.\n\n");
	}
}
////input data's syntax error check
void get_validate_input(const uint8_t *prompt, uint8_t *buffer, size_t buffer_size, uint8_t(*validate_func)(const uint8_t*)){
	while(1){
		UART_sendMSG(prompt);
		while(!DataReady);	//data ready ok
		DataReady=0;	//DataReady flag initialization
		if(validate_func(UART_Input_Buffer)){
			strcpy(buffer, UART_Input_Buffer);
			break;
		}
		else
			UART_sendMSG("Invalid INPUT. Try Again.\n");
	}
}

////-------------------------------------------------------------------
////uart2 communication in Tera Term
void process_UART_intro(){
	uint8_t name_input[20];
	uint8_t password_input[10];
	//1. Enter the Name
	get_validate_input(">> Enter the Name(only Alphabet & CAPS Lock on): ", name_input, sizeof(name_input), validate_name_input);
	sprintf(UART_TxBuffer, "Name : %s\n", name_input);
	UART_sendMSG(UART_TxBuffer);

	//2. Enter the PASSWORD(PHONE NUMBER 4 ****)
	get_validate_input(">> Enter the PassWord(only Back 4 Student_number ****): ", password_input, sizeof(password_input), validate_password_input);
	sprintf(UART_TxBuffer, "Password : %s\n", password_input);
	UART_sendMSG(UART_TxBuffer);

	printf("\n\nWelcome!!!! This is the UART&SPI_Communication Controller!\n\n");
	printf("You can use diverse functions such as UART2,3, SPI2, stepping_MOTOR, LED and BUZZER\n\n");

	//STM32 BOARD -> Blue Tooth App (UART3)
	sprintf(UART_TxBuffer, "\nName >> %s\nPassWord >> %s\nRegistration Complete!\n", name_input, password_input);
	UART_BLUETOOTH_sendMSG("\n\n----------------------------------------------------------------------\n\n");
	UART_BLUETOOTH_sendMSG(UART_TxBuffer);
}
//USB communication with Tera Term
void process_USB_BARCODE(){
	uint8_t barcode_input[20];
	get_validate_input("Barcode >> ", barcode_input, sizeof(barcode_input), validate_barcode_input);
	sprintf(UART_TxBuffer, "Barcode >> %s\n", barcode_input);
	UART_BLUETOOTH_sendMSG(UART_TxBuffer);
	display_barcode_info(barcode_input);
}
//-------------SPI2 COMMUNICATION STM32->STM32->TERA TERM-----------------------
void process_SPI2_Transmission() {
	  HAL_SPI_Transmit(&hspi2, SPI2_TxBuffer, sizeof(SPI2_TxBuffer), 1000);
	  HAL_UART_Transmit(&huart2, (uint8_t *)"MASTER -> SLAVE SPI2 Communication Succeed!\n", 45, 1000);
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
	  HAL_Delay(1000);
	  display_SPI_Emoticon();
	  UART_BLUETOOTH_sendMSG("\n\n----------------------------------------------------------------------\n\n");
	  UART_BLUETOOTH_sendMSG("Master , SLAVE COMMUNICATION START!!!!\n");
	  UART_BLUETOOTH_sendMSG("Master Over, then control the Slave Device\n\n");
	  UART_BLUETOOTH_sendMSG("----------------------------------------------------------------------\n\n");
}
//------------------------------------------------------------------------------
//----------------------EMOTICON SECTION-------------------
void display_Emoticon_CAT(){
	UART_sendMSG("	   /\\_/\\    	   /\\_/\\    	   /\\_/\\    \n");
	UART_sendMSG("	  ( o.o )   	  ( o.o )   	  ( o.o )   \n");
	UART_sendMSG("	   > ^ <    	   > ^ <    	   > ^ <    \n\n");
	UART_sendMSG("	   >AKJ<    	   >KNA<    	   >KNY<    \n\n");
}

void display_Emoticon_MOTOR(){
    UART_sendMSG("  M     M   OOO   TTTTT   OOO   RRRR   \n");
    UART_sendMSG("  MM   MM  O   O    T    O   O  R   R  \n");
    UART_sendMSG("  M M M M  O   O    T    O   O  RRRR   \n");
    UART_sendMSG("  M  M  M  O   O    T    O   O  R  R   \n");
    UART_sendMSG("  M     M   OOO     T     OOO   R   R  \n\n");
    //UART_sendMSG("\nMOTOR\n\n");
}
void display_Emoticon_PWM(){
	UART_sendMSG("\n       DUTY CYCLE 30~100%, FIRST = 60%       \n\n\n");
	UART_sendMSG(" PPPP   W   W  M   M  \n");
    UART_sendMSG(" P   P  W   W  M   M  \n");
    UART_sendMSG(" P   P  W   W  MM MM  \n");
    UART_sendMSG(" PPPP   W W W  M M M  \n");
    UART_sendMSG(" P      WW WW  M   M  \n");
    UART_sendMSG(" P      W   W  M   M  \n\n\n");
	UART_sendMSG(" High  ____________       ____________       ____________       \n");
	UART_sendMSG("       |          |       |          |       |          |       \n");
	UART_sendMSG("       |          |       |          |       |          |       \n");
	UART_sendMSG("       |          |_______|          |_______|          |_______ \n");
	UART_sendMSG("       |<-- High -->|<-- Low -->|<-- High -->|<-- Low -->|<-- High -->|\n\n\n");
}

void display_Emoticon_BUZZER(){
    UART_sendMSG(" BBBB    U   U   ZZZZZ   ZZZZZ   EEEEE    RRRR    \n");
    UART_sendMSG(" B   B   U   U      Z       Z    E        R   R   \n");
    UART_sendMSG(" BBBB    U   U     Z       Z     EEEE     RRRR    \n");
    UART_sendMSG(" B   B   U   U    Z       Z      E        R  R    \n");
    UART_sendMSG(" BBBB     UUU    ZZZZZ   ZZZZZ   EEEEE    R   R   \n\n");
}
void display_Emoticon_FAN(){
	UART_sendMSG(" FFFFF    AAA    N   N  \n");
	UART_sendMSG(" F       A   A   NN  N  \n");
	UART_sendMSG(" FFFF    AAAAA   N N N  \n");
	UART_sendMSG(" F       A   A   N  NN  \n");
	UART_sendMSG(" F       A   A   N   N  \n\n");
}
void display_Emoticon_LED1() {
    UART_sendMSG(" L       EEEEE  DDDD      1111     \n");
    UART_sendMSG(" L       E      D   D      111     \n");
    UART_sendMSG(" L       EEEE   D   D      111     \n");
    UART_sendMSG(" L       E      D   D      111     \n");
    UART_sendMSG(" LLLLL   EEEEE  DDDD     1111111   \n\n");
}
void display_Emoticon_LED2() {
    UART_sendMSG(" L       EEEEE  DDDD      22222   \n");
    UART_sendMSG(" L       E      D   D         2   \n");
    UART_sendMSG(" L       EEEE   D   D     22222   \n");
    UART_sendMSG(" L       E      D   D     2   	\n");
    UART_sendMSG(" LLLLL   EEEEE  DDDD      22222   \n\n");
}
void display_Emoticon_LED3() {
    UART_sendMSG(" L       EEEEE  DDDD      33333    \n");
    UART_sendMSG(" L       E      D   D         3    \n");
    UART_sendMSG(" L       EEEE   D   D      3333    \n");
    UART_sendMSG(" L       E      D   D         3    \n");
    UART_sendMSG(" LLLLL   EEEEE  DDDD      33333    \n\n");
}
void display_Emoticon_LED4() {
    UART_sendMSG(" L       EEEEE  DDDD        444   \n");
    UART_sendMSG(" L       E      D   D      4  4   \n");
    UART_sendMSG(" L       EEEE   D   D     444444  \n");
    UART_sendMSG(" L       E      D   D         4    \n");
    UART_sendMSG(" LLLLL   EEEEE  DDDD          4   \n\n");
}
void display_SPI_Emoticon()
{
	UART_sendMSG("\n\n");
    UART_sendMSG("	 SSSSS		PPPPP 		IIIIII  \n");
    UART_sendMSG("	S     		PP   P		  II    \n");
    UART_sendMSG("	 SSSSS		PPPPP 		  II    \n");
    UART_sendMSG("	      S		PP   		  II    \n");
    UART_sendMSG("	SSSSSS		PP    		IIIIII ..... \n\n\nCOMMUNICATION\n\n\n");
}
void display_UART_Emoticon()
{
	UART_sendMSG("\n\n");
    UART_sendMSG("	 U   U   	 AAAAA    	RRRRR 	    TTTTTT \n");
    UART_sendMSG("	 U   U   	A     A   	R    R	      TT   \n");
    UART_sendMSG("	 U   U   	AAAAAAA   	RRRRR 	      TT   \n");
    UART_sendMSG("	 U   U   	A     A   	R   R 	      TT   \n");
    UART_sendMSG("	  UUU    	A     A   	R    R	      TT   .....\n\n\nCOMMUNICATION\n\n\n");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
//#.project_in&out data&control system
  HAL_UART_Transmit(&huart2, Intro_TxBuffer, sizeof(Intro_TxBuffer), 1000); //intro0
  HAL_Delay(2000);
  display_Emoticon_CAT();
  display_serial_monitor_intro();	//intro1
  HAL_UART_Receive_IT(&huart2, (uint8_t *)UART_RxBuffer, 1);
  process_UART_intro();	//intro2
  UART_sendMSG("\n\n----------------------------------------------------------------------\n\n");
  UART_sendMSG("-----Check in your BLUE TOOTH App!----------\n\n");
  display_UART_Emoticon();
  HAL_UART_Transmit(&huart2, (uint8_t *)"MASTER Initialized\n\n\n", 20, 1000);
  UART_BLUETOOTH_sendMSG("\n\n----------------------------------------------------------------------\n\n");
  UART_BLUETOOTH_sendMSG("\n-----*****Information of Product*****-----\n");
  UART_BLUETOOTH_sendMSG("\----------------------------------------------------------------------\n\n");
  process_USB_BARCODE();	//com_usb->teraterm->stm32->bluetooth
  HAL_SPI_Transmit(&hspi2, SPI2_TxBuffer, sizeof(SPI2_TxBuffer), 1000);
  process_SPI2_Transmission();
  //-----ULTRA SONIC WAVES CONTROL-------------------------
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
  UART_sendMSG("-----Check about Box's location----------\n\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1){
	  //#.project_controller system
	  HCSR04_Read();
	  if ((Distance >0)&&(Distance <= 9)){
	      sprintf(UART_TxBuffer, "The product is located in A-Section: %d cm\r\n", Distance);  // A와 거리 출력
	      HAL_UART_Transmit(&huart2, (uint8_t *)UART_TxBuffer, strlen(UART_TxBuffer), 1000);
	  }
	  else if ((Distance > 12)&&(Distance <= 18)){
	      sprintf(UART_TxBuffer, "The product is located in B-Section: %d cm\r\n", Distance);  // B와 거리 출력
	      HAL_UART_Transmit(&huart2, (uint8_t *)UART_TxBuffer, strlen(UART_TxBuffer), 1000);
	  }
	  else if ((Distance > 23)&&(Distance <= 28)){
	      sprintf(UART_TxBuffer, "The product is located in C-Section: %d cm\r\n", Distance);  // C와 거리 출력
	      HAL_UART_Transmit(&huart2, (uint8_t *)UART_TxBuffer, strlen(UART_TxBuffer), 1000);
	  }
	  else	;
	  HAL_Delay(1000);  // 1초 대기
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 58982;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffff-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, AIN2_Pin|LED0_Pin|Trig_Pin|GPIO_PIN_2
                          |LED3_Pin|FND_SEL2_Pin|CLCD_D4_Pin|CLCD_D5_Pin
                          |CLCD_D6_Pin|CLCD_D7_Pin|CLCD_RS_Pin|BIN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, FND0_Pin|FND1_Pin|FND2_Pin|BIN1_Pin
                          |FND3_Pin|FND4_Pin|FND5_Pin|FND6_Pin
                          |FND7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, FND_SEL0_Pin|FND_SEL1_Pin|CLCD_E_Pin|AIN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : AIN2_Pin LED0_Pin Trig_Pin PC2
                           LED3_Pin FND_SEL2_Pin CLCD_D4_Pin CLCD_D5_Pin
                           CLCD_D6_Pin CLCD_D7_Pin CLCD_RS_Pin BIN2_Pin */
  GPIO_InitStruct.Pin = AIN2_Pin|LED0_Pin|Trig_Pin|GPIO_PIN_2
                          |LED3_Pin|FND_SEL2_Pin|CLCD_D4_Pin|CLCD_D5_Pin
                          |CLCD_D6_Pin|CLCD_D7_Pin|CLCD_RS_Pin|BIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SW2_Pin */
  GPIO_InitStruct.Pin = SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : FND0_Pin FND1_Pin FND2_Pin BIN1_Pin
                           FND3_Pin FND4_Pin FND5_Pin FND6_Pin
                           FND7_Pin */
  GPIO_InitStruct.Pin = FND0_Pin|FND1_Pin|FND2_Pin|BIN1_Pin
                          |FND3_Pin|FND4_Pin|FND5_Pin|FND6_Pin
                          |FND7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : FND_SEL0_Pin FND_SEL1_Pin CLCD_E_Pin AIN1_Pin */
  GPIO_InitStruct.Pin = FND_SEL0_Pin|FND_SEL1_Pin|CLCD_E_Pin|AIN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SW1_Pin */
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//#.project_CONTROLLER system
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART2){
		uint8_t received_char = UART_RxBuffer[0];	//receive char

		if(received_char == '\r'){	//if press the 'Enter key'->input_ok
			UART_Input_Buffer[RxIndex]='\0'; //string end
			DataReady = 1;	//data ready_ok flag set
			RxIndex =0; //input index initialization
			UART_sendMSG("\n");	//LineFeed
		}
		else if(received_char =='\b'){	//if press the 'Backspace Key'
			if(RxIndex > 0){
				RxIndex--;	//Back space
				UART_sendMSG("\b \b");
			}
		}
		else{	//normal string input
			if(RxIndex < UART_BUFFER_SIZE-1){
				UART_Input_Buffer[RxIndex++] = received_char;
				HAL_UART_Transmit(&huart2, (uint8_t *)&received_char,1, 1000);
			}
		}
		HAL_UART_Receive_IT(&huart2,(uint8_t *)UART_RxBuffer, 1);
		//Interrupt Enable for next Interrupt
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) { // if the interrupt source is channel1
		if (Is_First_Captured==0) {// if the first value is not captured
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
			Is_First_Captured = 1;  // set the first captured as true
			// Now change the polarity to falling edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if (Is_First_Captured==1) {  // if the first is already captured
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter
			if (IC_Val2 > IC_Val1){
				Difference = IC_Val2-IC_Val1;
			}
			else if (IC_Val1 > IC_Val2){
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}
			Distance = Difference * .034/2;
			Is_First_Captured = 0; // set it back to false
			// set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
		}
	}
}
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
	if(ch == '\n') HAL_UART_Transmit(&huart2, (uint8_t *)"\r",1,1000);
	return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
