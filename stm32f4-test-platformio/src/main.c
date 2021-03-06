
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "math.h"
#include "ssd1306.h"
//#include "i2c-lcd.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C2_Init(void);

/* USER CODE BEGIN PFP */
void read_MPU_6050_data(void);
void display_mode(void);
void display_light(void);
void spi_send(void);
void digipot_registration(void);
void MPU_6050_registragion(void);
void oled_registration(void);
void oled_display(void);

/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */


// SPI Variables
uint8_t spiRegister[] = {0x18,0x03};
uint8_t spiCheck[] = {0x03,0xFF};
uint8_t spiWrite[] = {0x04,0x00};
uint16_t rStep;
uint8_t spiResistance[2];
//uint16_t rDigipot[1024];
uint16_t rDigipot[33] = {152/*1008*/, 889, 366, 208, 152, 114, 89, 75, 64, 56, 49, 43, 39, 34, 31, 28, 26, 24, 21, 20, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 1};
float rDisplay[33] = {1.7/*1.3*/, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.1, 4.2, 4.4, 4.6, 4.8, 5.0};
uint8_t spiSent = 0;


// Interupt Variables
uint16_t gpioPin = GPIO_PIN_12;


// I2C Variables
uint8_t i2cData[2];
uint8_t address = 0x68;
uint8_t readData[2];
unsigned char buffer[6];
int16_t gyro_x, gyro_y, gyro_z;
//int16_t  angle_pitch_buffer,angle_roll_buffer;
int16_t acc_x, acc_y, acc_z, acc_total_vector;
//int32_t gyro_x_cal=0,gyro_y_cal=0,gyro_z_cal=0; // gyro calibration value
//float angle_pitch, angle_roll, angle_pitch_acc,angle_roll_acc;
//float angle_pitch_output,angle_roll_output;
uint8_t maxAccel[33] = {10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74};



// Stimulation
uint8_t enableStim = 0;
volatile int32_t x = 0;
volatile uint16_t stimPin = 0x0001;
uint16_t state = 0x0001;
//uint32_t limitDuration[1024];
uint16_t limitDuration[33] = {501/*1*/, 2, 4, 5, 62, 94, 120, 162, 196, 230, 264, 298, 331, 365, 399, 433, 467, 501, 535, 569, 603, 637, 671, 705, 739, 773, 807, 841, 875, 909, 943, 976, 1010};
float displayDuration[33] = {30/*0.3*/, 0.5, 1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60};
//uint16_t limitFrequency[10] = {52549, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t stimCount = 0;
uint16_t stimRec = 0;
uint16_t limitAccel[33] = {10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74};


// Display
uint8_t modeNumber = 0;
uint16_t lightNumber[3] = {0, 0, 0};
uint16_t stepExper = 33;
uint8_t lcdCheck;
char strInten[10];
char strDurat[10];
char strAccel[15];



int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_I2C2_Init();
  HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE BEGIN 2 */

  oled_registration();
  digipot_registration();
  MPU_6050_registragion();

  /* Infinite loop */
  /*uint16_t i;
  for(i=0 ; i<stepExper ; i++)
  {
    limitDuration[i] = 100*i;
  }*/
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    oled_display();

    if(spiSent == 1) spi_send();

    read_MPU_6050_data();

    if(acc_total_vector > limitAccel[lightNumber[2]]) enableStim = 1;
  }
  /* USER CODE END 3 */
}


void oled_registration()
{
  lcdCheck = SSD1306_Init();
  /*int a = 1234;
  itoa(a, strInten, 10);
  float f = 15.6789;
  gcvt(f, 4, strDurat);*/

  SSD1306_Fill(0);  // fill color 0 ie black
  SSD1306_UpdateScreen();   // print the changes on the display

  SSD1306_GotoXY(15,20);  // goto 10,20
  SSD1306_Puts("-TLNS-", &Font_16x26, 1);

  SSD1306_UpdateScreen();   // print the changes on the display
  HAL_Delay(1000);

  SSD1306_Fill(0);
  SSD1306_UpdateScreen();
}
void oled_display()
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
  if(modeNumber == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  else if(modeNumber == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
  if(enableStim == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
  else if(enableStim == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

  // Intensity display
  SSD1306_GotoXY(5,2);
  if(modeNumber == 0) SSD1306_Puts("> I:", &Font_11x18, 1);
  else if(modeNumber == 1) SSD1306_Puts("  I:", &Font_11x18, 1);
  else if(modeNumber == 2) SSD1306_Puts("  I:", &Font_11x18, 1);
  //itoa(rDigipot[lightNumber[0]], strInten, 10); // 10 is decimal
  gcvt(rDisplay[lightNumber[0]], 3, strInten);
  SSD1306_Puts(strInten, &Font_11x18, 1);
  SSD1306_Puts(" mA    ", &Font_11x18, 1);
  
  // Pulse width display
  SSD1306_GotoXY(5,22);
  if(modeNumber == 0) SSD1306_Puts(" PW:", &Font_11x18, 1);
  else if(modeNumber == 1) SSD1306_Puts(">PW:", &Font_11x18, 1);
  else if(modeNumber == 2) SSD1306_Puts(" PW:", &Font_11x18, 1); 
  //itoa(limitDuration[lightNumber[1]], strDurat, 10); // 10 is decimal
  gcvt(displayDuration[lightNumber[1]], 3, strDurat);
  SSD1306_Puts(strDurat, &Font_11x18, 1);
  SSD1306_Puts(" uS    ", &Font_11x18, 1);

  // Acceleration display
  SSD1306_GotoXY(5,42);
  if(modeNumber == 0) SSD1306_Puts("  A:", &Font_11x18, 1);
  else if(modeNumber == 1) SSD1306_Puts("  A:", &Font_11x18, 1);
  else if(modeNumber == 2) SSD1306_Puts("> A:", &Font_11x18, 1);
  //gcvt(acc_total_vector, 3, strAccel);
  itoa(limitAccel[lightNumber[2]], strAccel,10); // 10 is decimal
  SSD1306_Puts(strAccel, &Font_11x18, 1);
  SSD1306_Puts("     ", &Font_11x18, 1);
  SSD1306_UpdateScreen();
}


void MPU_6050_registragion()
{
  // Gyroscope Registration
  buffer[0] = 0x6B; // Send request to the register you want to access
	buffer[1] = 0x00; // Set the requested register
	HAL_I2C_Master_Transmit(&hi2c2,0x68<<1,buffer,2,100);
	// Configure gyro(500dps full scale)
	buffer[0] = 0x1B;  // Send request to the register you want to access
	buffer[1] = 0x08;  // Set the requested register
	HAL_I2C_Master_Transmit(&hi2c2,0x68<<1,buffer,2,100);
	// Configure accelerometer(+/- 8g)
	buffer[0] = 0x1C;  // Send request to the register you want to access
	buffer[1] = 0x10;  // Set the requested register
	HAL_I2C_Master_Transmit(&hi2c2,0x68<<1,buffer,2,100);
  HAL_Delay(10);
	// Finish setup MPU-6050 register
}
void read_MPU_6050_data()
{
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	buffer[0] = 0x3B;//0x3B
	HAL_I2C_Master_Transmit(&hi2c2,0x68<<1,buffer,1,100);
	HAL_I2C_Master_Receive(&hi2c2,0x68<<1,buffer,6,100);
	
	acc_x = buffer[0]<<8 | buffer[1];
	acc_y = buffer[2]<<8 | buffer[3];
	acc_z = buffer[4]<<8 | buffer[5];
	
	buffer[0] = 0x43;//0x43
	HAL_I2C_Master_Transmit(&hi2c2,0x68<<1,buffer,1,100);
	HAL_I2C_Master_Receive(&hi2c2,0x68<<1,buffer,6,100);
	
	gyro_x = buffer[0]<<8 | buffer[1];
	gyro_y = buffer[2]<<8 | buffer[3];
	gyro_z = buffer[4]<<8 | buffer[5];

  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));
  acc_total_vector /= 100;
  acc_total_vector -= 37;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
  HAL_Delay(10);
}


void digipot_registration()
{
  // Digipot Registration
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, spiRegister, 2, 50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, spiRegister, 2, 50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}
void spi_send()
{
  rStep = rDigipot[lightNumber[0]];
  // SPI Packaging
  spiResistance[0] = rStep>>8;
  spiResistance[1] = rStep;
  // Package checking
  spiResistance[0] = (spiResistance[0]&spiCheck[0])|spiWrite[0];
  spiResistance[1] = (spiResistance[1]&spiCheck[1])|spiWrite[1];
  // SPI Change Digipot value
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, spiResistance, 2, 50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  spiSent = 0;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(TIM1==htim->Instance) // 312.5 us
  {
    //if(enableStim == 1)
    //{
      if(stimCount)
      {

        /*if(stimPin == GPIO_PIN_3)*/ HAL_GPIO_WritePin(GPIOE, stimPin, GPIO_PIN_RESET); // Stop being ground
        /*if(stimPin == GPIO_PIN_3)*/ HAL_GPIO_WritePin(GPIOD, stimPin, GPIO_PIN_SET);   // Start stimulation

        if(limitDuration[lightNumber[1]]<=5)
        
        {
          x=1;
          switch(limitDuration[lightNumber[1]])
          {
            case 1: x=1;x++;break;
            case 2: x++;x++;x++;x++;x++;x++;x++;x++;break;
            case 3: x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;break;
            case 4: x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;break;
            case 5: x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
                    x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;x++;
                    x++;x++;x++;x++;x++;x++;break;
          }
        }
        else
        {
          x=1;
          while(x<=limitDuration[lightNumber[1]]) {x++;}
        }
        /*if(stimPin == GPIO_PIN_3)*/ HAL_GPIO_WritePin(GPIOD, stimPin, GPIO_PIN_RESET); // Stop stimulation
        /*if(stimPin == GPIO_PIN_3)*/ HAL_GPIO_WritePin(GPIOE, stimPin, GPIO_PIN_SET);   // Start being ground
      }
      stimPin = stimPin << 1;
      if(stimPin == 0x0000)
      {
        stimPin = 0x0001;
        stimCount++;
        stimCount %= 4;

        /*stimRec++;
        if(stimRec == 400 && lightNumber[0] < stepExper-1) // 100 for 0.5 second
        {
          spiSent = 1;
          lightNumber[0]++;
          stimRec = 0;
        }*/
      }
    //}
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C2 init function */
static void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 52549;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3*/
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB15 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11 
                           PD12 PD13 PD14 PD15 
                           PD0 PD1 PD2 PD3 
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE9 PE10 PE11 
                           PE12 PE13 PE14 PE15 
                           PE0 PE1 PE2 PE3 
                           PE4 PE5 PE6 PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 
                          |GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
