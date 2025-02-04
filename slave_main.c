임베디드 최종 3

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
#include "ssd1306_conf.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
//#include "ssd1306_conf.h"
//#include "ssd1306_fonts.h"
//#include "ssd1306.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#project_CONTROLLER SYSTEM

#define SPI_BUFFER_SIZE 100
#define UART_BUFFER_SIZE 100
#define stepsperrev 4096
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t UART_TxBuffer[UART_BUFFER_SIZE];
uint8_t SPI2_RxBuffer[SPI_BUFFER_SIZE];
bool task_requested = false;	//task_request flag
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------UART2 with TERA TERM-------------------------
void UART_sendMSG(const uint8_t *str){
	HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 1000);
}
//-----------------------------------stepping motor control history1------------
//---------------STEPPING MOTOR DRIVE--------------------------
//------------STEPPER MOTOR DRIVING (28BYJ-48)---
//BLUE == PA12 / PINK == PA11 / YELLOW == PA10 / ORANGE == PA9
//FULL STEP SIGNALS
//ORANGE	YELLOW	PINK	BLUE
//0			0		0		1
//0			0		1		0
//0			1		0		0
//1			0		0		0
//------------------------------------------------
//void full_step_motor(int x){
//	int i;
//	int degree = (512*x)/360;
//	for (i=0;i<degree;i++){
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//		HAL_Delay(1);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//		HAL_Delay(1);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//		HAL_Delay(1);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
//		HAL_Delay(1);
//	}
//}

//void run_step_motor(){
//    if (strcmp((char *)SPI2_RxBuffer, "XYLITOR GUM, A-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-XYLITOR GUM to A-section! \n");
//        display_Emoticon_MOTOR();
//        full_step_motor(360*6); //6cycle
//        full_step_motor1(360*6);
//    }
//    else if (strcmp((char *)SPI2_RxBuffer, "WOW GUM, B-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-WOW GUM to B-section! \n");
//        display_Emoticon_MOTOR();
//        full_step_motor(360*4); //4cycle
//        full_step_motor1(360*4);
//    }
//    else if (strcmp((char *)SPI2_RxBuffer, "ID GUM, C-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-ID GUM to C-section! \n");
//        display_Emoticon_MOTOR();
//        full_step_motor(360*2); //2cycle
//        full_step_motor1(360*2);
//    }
//    else{
//    	full_step_motor(0);
//    	full_step_motor1(0);
//    }
//    task_requested = false;
//}
//-----------------------------------------UNCLOCK DIRECTION----------------------------------------------------------
void step_motor(GPIO_TypeDef *port, uint16_t pins[], int step) {
    // 모든 핀을 초기화
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(port, pins[i], GPIO_PIN_RESET);
    }
    // 현재 스텝만 활성화
    HAL_GPIO_WritePin(port, pins[step], GPIO_PIN_SET);
}

void full_step_motors(int x1, int x2) {
    int i;
    int degree1 = (512 * x1) / 360; // 모터 1의 스텝 수
    int degree2 = (512 * x2) / 360; // 모터 2의 스텝 수
    int max_degree = (degree1 > degree2) ? degree1 : degree2;

    // 모터 핀 배열 설정
    uint16_t motor1_pins[4] = {GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9};  // 모터 1
    uint16_t motor2_pins[4] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4};     // 모터 2

    for (i = 0; i < max_degree; i++) {
        // 모터 1 스텝
        if (i < degree1) {
            step_motor(GPIOA, motor1_pins, i % 4);
        } else {
            step_motor(GPIOA, motor1_pins, -1); // 모터 1 정지
        }

        // 모터 2 스텝
        if (i < degree2) {
            step_motor(GPIOB, motor2_pins, i % 4);
        } else {
            step_motor(GPIOB, motor2_pins, -1); // 모터 2 정지
        }

        HAL_Delay(1); // 각 스텝 간 딜레이
    }

    // 모터 정지
    step_motor(GPIOA, motor1_pins, -1);
    step_motor(GPIOB, motor2_pins, -1);
}
//------------------------------CLOCK DIRECTION---------------------
void full_step_motors_reverse(int x1, int x2) {
    int i;
    int degree1 = (512 * x1) / 360; // 모터 1의 스텝 수
    int degree2 = (512 * x2) / 360; // 모터 2의 스텝 수
    int max_degree = (degree1 > degree2) ? degree1 : degree2;

    // 모터 핀 배열 설정
    uint16_t motor1_pins[4] = {GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9};  // 모터 1
    uint16_t motor2_pins[4] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4};     // 모터 2

    for (i = 0; i < max_degree; i++) {
        // 모터 1 역회전 스텝
        if (i < degree1) {
            step_motor(GPIOA, motor1_pins, (3 - (i % 4))); // 역회전 순서
        } else {
            step_motor(GPIOA, motor1_pins, -1); // 모터 1 정지
        }

        // 모터 2 역회전 스텝
        if (i < degree2) {
            step_motor(GPIOB, motor2_pins, (3 - (i % 4))); // 역회전 순서
        } else {
            step_motor(GPIOB, motor2_pins, -1); // 모터 2 정지
        }

        HAL_Delay(1); // 각 스텝 간 딜레이
    }

    // 모터 정지
    step_motor(GPIOA, motor1_pins, -1);
    step_motor(GPIOB, motor2_pins, -1);
}
//---------------HISTORY1------------------------------
//void run_step_motor() {
//    if (strcmp((char *)SPI2_RxBuffer, "XYLITOR GUM, A-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-XYLITOR GUM to A-section! \n");
//        display_Emoticon_MOTOR();
//        //full_step_motors(360 * 35, 360 * 35);
//        full_step_motors_reverse(360 * 40, 360 * 40);
//    } else if (strcmp((char *)SPI2_RxBuffer, "WOW BLUE GUM, B-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-WOW BLUE GUM to B-section! \n");
//        display_Emoticon_MOTOR();
//        //full_step_motors(360 * 23, 360 * 23); // 4 cycle
//        full_step_motors_reverse(360 * 25, 360 * 25);
//    } else if (strcmp((char *)SPI2_RxBuffer, "JUICY FRESH GUM, C-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-JUICY FRESH GUM to C-section! \n");
//        display_Emoticon_MOTOR();
//        //full_step_motors(360 * 5, 360 * 5); // 2 cycle
//        full_step_motors_reverse(360 * 12, 360 * 12);
//    } else {
//        //full_step_motors(0, 0);
//    	full_step_motors_reverse(0, 0);
//    }
//    task_requested = false;
//}

//----------------------------------OLED ANIMATION--------------------
//---------------------------HISTORY1---------------------------------
//void conveyor_belt_animation() {
//	//display_I2C_Emoticon();
//	//display_OLED_Emoticon();
//    uint8_t belt_y = 40; // 벨트 위치를 위로 올림
//    uint8_t box_width = 20; // 상자 너비
//    uint8_t box_height = 10; // 상자 높이
//    uint8_t box_y = belt_y - box_height; // 상자 y 좌표는 벨트 바로 위에 위치
//    char OLED_TEXT[] = "-> -> A"; // 박스 위에 표시할 문자
//    ssd1306_Init(); // OLED 초기화
//    ssd1306_Fill(Black); // 화면을 검은색으로 채움
//    // "A" 문자는 계속해서 윗줄에 표시
//    ssd1306_SetCursor(2, 2); // "A" 문자 위치 (윗줄)
//    ssd1306_WriteString(OLED_TEXT, Font_16x24, White);
//    ssd1306_UpdateScreen(); // 화면 업데이트
//    for (int x = 0; x <= 128; x++) { // 왼쪽에서 오른쪽으로 이동
//        ssd1306_Fill(Black); // 화면 초기화
//        // "A"는 계속 화면에 표시
//        ssd1306_SetCursor(2, 2); // "A" 문자 위치 (윗줄)
//        ssd1306_WriteString(OLED_TEXT, Font_16x24, White);
//        // 컨베이어 벨트 그리기
//        ssd1306_DrawRectangle(0, belt_y, 127, belt_y + 4, White);
//        // 상자 그리기
//        ssd1306_FillRectangle(x, box_y, x + box_width, box_y + box_height, White);
//        ssd1306_UpdateScreen(); // 화면 업데이트
//        HAL_Delay(25); // 애니메이션 속도 조절
//    }task_requested = false;
//}

//-------------HISTORY2--------------
//void conveyor_belt_animation(const char *section_label) {
//    uint8_t belt_y = 40; // 벨트 위치
//    uint8_t box_width = 20; // 상자 너비
//    uint8_t box_height = 10; // 상자 높이
//    uint8_t box_y = belt_y - box_height; // 상자 y 좌표는 벨트 바로 위에 위치
//
//    ssd1306_Init(); // OLED 초기화
//    ssd1306_Fill(Black); // 화면을 검은색으로 채움
//
//    // 섹션 레이블 표시
//    ssd1306_SetCursor(2, 2); // "A", "B", "C" 문자 위치 (윗줄)
//    ssd1306_WriteString(section_label, Font_16x24, White);
//    ssd1306_UpdateScreen(); // 화면 업데이트
//
//    for (int x = 0; x <= 128; x++) { // 왼쪽에서 오른쪽으로 이동
//        ssd1306_Fill(Black); // 화면 초기화
//        // 섹션 레이블 유지
//        ssd1306_SetCursor(2, 2); // 문자 위치 유지
//        ssd1306_WriteString(section_label, Font_16x24, White);
//
//        // 컨베이어 벨트 그리기
//        ssd1306_DrawRectangle(0, belt_y, 127, belt_y + 4, White);
//        // 상자 그리기
//        ssd1306_FillRectangle(x, box_y, x + box_width, box_y + box_height, White);
//        ssd1306_UpdateScreen(); // 화면 업데이트
//        HAL_Delay(25); // 애니메이션 속도 조절
//    }
//    task_requested = false;
//}
//void run_step_motor() {
//    const char *section_label = "-> -> "; // 기본값
//
//    if (strcmp((char *)SPI2_RxBuffer, "XYLITOR GUM, A-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-XYLITOR GUM to A-section! \n");
//        section_label = "-> -> A";
//        display_Emoticon_MOTOR();
//        full_step_motors_reverse(360 * 40, 360 * 40);
//    } else if (strcmp((char *)SPI2_RxBuffer, "WOW BLUE GUM, B-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-WOW BLUE GUM to B-section! \n");
//        section_label = "-> -> B";
//        display_Emoticon_MOTOR();
//        full_step_motors_reverse(360 * 25, 360 * 25);
//    } else if (strcmp((char *)SPI2_RxBuffer, "JUICY FRESH GUM, C-section") == 0) {
//        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-JUICY FRESH GUM to C-section! \n");
//        section_label = "-> -> C";
//        display_Emoticon_MOTOR();
//        full_step_motors_reverse(360 * 12, 360 * 12);
//    } else {
//        full_step_motors_reverse(0, 0);
//    }
//
//    // 애니메이션 실행
//    conveyor_belt_animation(section_label);
//
//    task_requested = false;
//}
//--------HISTORY3-------------------
void conveyor_belt_animation(const char *section_label, int motor_angle1, int motor_angle2) {
    uint8_t belt_y = 40; // 벨트 위치
    uint8_t box_width = 20; // 상자 너비
    uint8_t box_height = 10; // 상자 높이
    uint8_t box_y = belt_y - box_height; // 상자 y 좌표는 벨트 바로 위에 위치

    ssd1306_Init(); // OLED 초기화
    ssd1306_Fill(Black); // 화면을 검은색으로 채움

    // 섹션 레이블 표시
    ssd1306_SetCursor(2, 2); // "A", "B", "C" 문자 위치
    ssd1306_WriteString(section_label, Font_16x24, White);
    ssd1306_UpdateScreen(); // 화면 업데이트

    // 애니메이션: 왼쪽에서 오른쪽으로 이동
    for (int x = 0; x <= 128; x++) {
        ssd1306_Fill(Black); // 화면 초기화
        ssd1306_SetCursor(2, 2);
        ssd1306_WriteString(section_label, Font_16x24, White); // 섹션 유지
        ssd1306_DrawRectangle(0, belt_y, 127, belt_y + 4, White);
        ssd1306_FillRectangle(x, box_y, x + box_width, box_y + box_height, White);
        ssd1306_UpdateScreen();
        HAL_Delay(25); // 애니메이션 속도 조절
    }

    // 애니메이션이 완료된 후 모터 동작 호출
    full_step_motors_reverse(motor_angle1, motor_angle2);

    task_requested = false; // 작업 완료
}
void run_step_motor() {
    if (strcmp((char *)SPI2_RxBuffer, "XYLITOR GUM, A-section") == 0) {
        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-XYLITOR GUM to A-section! \n");
        //display_Emoticon_MOTOR();
        display_Emoticon_Conveyor();
        conveyor_belt_animation("-> -> A", 360 * 40, 360 * 40); // A 구역 애니메이션 후 모터
    } else if (strcmp((char *)SPI2_RxBuffer, "WOW BLUE GUM, B-section") == 0) {
        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-WOW BLUE GUM to B-section! \n");
        //display_Emoticon_MOTOR();
        display_Emoticon_Conveyor();
        conveyor_belt_animation("-> -> B", 360 * 25, 360 * 25); // B 구역 애니메이션 후 모터
    } else if (strcmp((char *)SPI2_RxBuffer, "JUICY FRESH GUM, C-section") == 0) {
        UART_sendMSG((uint8_t *)"\n\nOkay. I'll transport the product-JUICY FRESH GUM to C-section! \n");
        //display_Emoticon_MOTOR();
        display_Emoticon_Conveyor();
        conveyor_belt_animation("-> -> C", 360 * 12, 360 * 12); // C 구역 애니메이션 후 모터
    } else {
        UART_sendMSG((uint8_t *)"\n\nInvalid product section! \n");
        conveyor_belt_animation("Invalid", 0, 0); // 잘못된 데이터 처리
    }
}

//---------------emoticon-------------------------
void display_Emoticon_DOG() {
	UART_sendMSG("\n\n\n");
    UART_sendMSG("		   /^ ^\\       /^ ^\\       /^ ^\\    \n");
    UART_sendMSG("		  / 0 0 \\     / 0 0 \\     / 0 0 \\   \n");
    UART_sendMSG("		  V\\ Y /V     V\\ Y /V     V\\ Y /V   \n");
    UART_sendMSG("		   / - \\       / - \\       / - \\    \n");
    UART_sendMSG("		   >AKJ<       >KNA<       >KNY<    \n\n\n");
}

void display_Emoticon_Conveyor() {
    UART_sendMSG("\n\n\n");
    UART_sendMSG("           #####################            \n");
    UART_sendMSG("           ####             ####            \n");
    UART_sendMSG("           ####	  G. U. M   ####   ========>\n");
    UART_sendMSG("           ####             ####            \n");
    UART_sendMSG("           #####################            \n");
    UART_sendMSG("===============================================\n");
    UART_sendMSG("===============================================\n");
    UART_sendMSG("\n");
    UART_sendMSG("            Conveyor Belt in Operation\n");
    UART_sendMSG("\n");
}

void display_Emoticon_MOTOR(){
	UART_sendMSG("\n\n\n");
    UART_sendMSG("  M     M   OOO   TTTTTT   OOO   RRRR   \n");
    UART_sendMSG("  MM   MM  O   O    TT    O   O  R   R  \n");
    UART_sendMSG("  M M M M  O   O    TT    O   O  RRRR   \n");
    UART_sendMSG("  M  M  M  O   O    TT    O   O  R  R   \n");
    UART_sendMSG("  M     M   OOO     TT     OOO   R   R  \n\n");
}
void display_Led_SPI() {
	UART_sendMSG("\n\n");
    UART_sendMSG("	  ┌──────┐    ┌──────┐  \n");
    UART_sendMSG("	  │███│    │███│  \n");
    UART_sendMSG("	  │███│    │███│  \n");
    UART_sendMSG("	  │███│    │███│  \n");
    UART_sendMSG("	  │███│    │███│  \n");
    UART_sendMSG("	  └──────┘    └──────┘  \n");
    UART_sendMSG("	    \"MASTER\"    \"SLAVE\"   \n\n");
    UART_sendMSG("	SPI \"LED ON\"    \"LED ON\"   \n\n");
}
void display_SPI_Emoticon(){
	UART_sendMSG("\n\n");
    UART_sendMSG("  SSSS   PPPP   IIII  \t 22222 \n");
    UART_sendMSG(" S       P   P   II   \t     2  \n");
    UART_sendMSG("  SSS    PPPP    II   \t 22222  \n");
    UART_sendMSG("     S   P       II   \t 2     \n");
    UART_sendMSG("  SSSS   P      IIII  \t 22222 \n\n");
    UART_sendMSG(" SPI Communication \n\n\n");
}
void display_I2C_Emoticon(){
	UART_sendMSG("\n\n");
    UART_sendMSG("  IIII\t22222 \t CCCC   \n");
    UART_sendMSG("   II\t    2\tC       \n");
    UART_sendMSG("   II\t22222\tC       \n");
    UART_sendMSG("   II\t2    \tC       \n");
    UART_sendMSG("  IIII\t22222\t CCCC   ....\n\n");
    UART_sendMSG("  I2C Communication_OLED \n\n\n");
}
void display_BUZZER_Emoticon(){
    UART_sendMSG(" BBBB    U   U   ZZZZZ   ZZZZZ   EEEEE    RRRR    \n");
    UART_sendMSG(" B   B   U   U      Z       Z    E        R   R   \n");
    UART_sendMSG(" BBBB    U   U     Z       Z     EEEE     RRRR    \n");
    UART_sendMSG(" B   B   U   U    Z       Z      E        R  R    \n");
    UART_sendMSG(" BBBB     UUU    ZZZZZ   ZZZZZ   EEEEE    R   R   \n\n\n");
}
void display_OLED_Emoticon() {
    UART_sendMSG(" OOOOO   L        EEEEE   DDDD    \n");
    UART_sendMSG(" O   O   L        E       D   D   \n");
    UART_sendMSG(" O   O   L        EEEE    D   D   \n");
    UART_sendMSG(" O   O   L        E       D   D   \n");
    UART_sendMSG(" OOOOO   LLLLL    EEEEE   DDDD    \n");
    UART_sendMSG("\n\n");
    UART_sendMSG(" OLED ANIMATION EFFECT Display...Wait a Seconds \n\n\n");
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//--------------------WARNING---------------------IF ERASED CODE------------
//#include "ssd1306_conf.h"
//#include "ssd1306_fonts.h"
//#include "ssd1306.h"
//-----------WHILE() INSIDE---------------
//if (task_requested) {
//	  conveyor_belt_animation();
//    run_step_motor();
//}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  //-----------OLED-----------------------
  display_SPI_Emoticon();
  display_I2C_Emoticon();
  display_OLED_Emoticon();
  display_Emoticon_MOTOR();
  //---------TERA TERM UART--intro----------------------
  UART_sendMSG("\n\n\n-----------------------------------------------------\n\n");
  UART_sendMSG("\n\n");
  UART_sendMSG("\n\n\n---------------------EMBEDDED CLASS 5TEAM-------------------------\n\n");
  UART_sendMSG("------------------------@SLAVE DEVICE-----------------------------\n\n");
  UART_sendMSG("---SPI_Receive, STEPPING MOTOR, OLED_I2C, UART, BUZZER CONTROL----\n\n");
  UART_sendMSG("----------------The slave receives information from the master.------------\n\n");
  display_Emoticon_DOG();
//
//  //-------slave_spi2 & uart2 tera term-------------------
  HAL_UART_Transmit(&huart2, (uint8_t *)"SLAVE Initialized\n\n\n>>", 19, 1000);

  HAL_SPI_Receive_IT(&hspi2, SPI2_RxBuffer, sizeof(SPI2_RxBuffer));
  HAL_UART_Transmit(&huart2, (uint8_t*)"\n",2, 1000);
  HAL_UART_Transmit(&huart2, SPI2_RxBuffer, strlen((char *)SPI2_RxBuffer), 1000);

  //conveyor_belt_animation();
  //-------------------OLED-------------------------------
  //conveyor_belt_animation();
  //HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1){
    /* USER CODE END WHILE */
	  //-------HISROTY1------
//      if (task_requested) {
//    	  conveyor_belt_animation();
//          //run_step_motor();
//      }
	  //---------HISTORY2 & HISTORY3-------
	    if (task_requested) {
	        run_step_motor();
	    }
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
  hspi2.Init.Mode = SPI_MODE_SLAVE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_INPUT;
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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
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
  HAL_GPIO_WritePin(GPIOC, LED4_Pin|LED3_Pin|LED2_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|IN4_Pin|IN3_Pin|IN2_Pin
                          |IN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IN44_Pin|IN33_Pin|IN22_Pin|IN11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED4_Pin LED3_Pin LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED4_Pin|LED3_Pin|LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin IN4_Pin IN3_Pin IN2_Pin
                           IN1_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|IN4_Pin|IN3_Pin|IN2_Pin
                          |IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN44_Pin IN33_Pin IN22_Pin IN11_Pin */
  GPIO_InitStruct.Pin = IN44_Pin|IN33_Pin|IN22_Pin|IN11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
    if (hspi->Instance == SPI2)
    {
        HAL_UART_Transmit(&huart2, SPI2_RxBuffer, strlen((char *)SPI2_RxBuffer), 1000);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0,GPIO_PIN_SET);
        display_Led_SPI();
        //display_SPI_Emoticon();
        task_requested = true;
    }
}

int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
	if(ch=='\n') HAL_UART_Transmit(&huart2, (uint8_t *)"\r", 1, 1000);
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
