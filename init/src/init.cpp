#include "main.h"
#include "init.h"
#include "gpio.h"
#include "time.h"

 
gpio::gpio_dsc mco_pin_dsc = {GPIOA, GPIO_PIN_8};


gpio::gpio_dsc  spi_2_pin_dsc[] = 
{
    {GPIOI, GPIO_PIN_1},
    {GPIOI, GPIO_PIN_2},
    {GPIOI, GPIO_PIN_3},
        
};

gpio::gpio_dsc led_pin_dsc = {GPIOC, GPIO_PIN_13};

void init::hal ()
{
    HAL_Init();
}

void init::clock ()
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    for (;;);
  }
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    for (;;);
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

void init::clock_enable ()
{
  
  __GPIOC_CLK_ENABLE();
  __GPIOF_CLK_ENABLE();
  
}

void init::nvic ()
{
  HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DCMI_IRQn);  
  
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);  
    
  HAL_NVIC_SetPriority(TSC_TIM_IRQn, 1, 3);
  HAL_NVIC_EnableIRQ(TSC_TIM_IRQn);
    
  HAL_NVIC_SetPriority(HAL_TIM_MILISn, 1, 0xf);
  HAL_NVIC_EnableIRQ(HAL_TIM_MILISn);
    
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn); 
    
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  
}

TIM_HandleTypeDef             TimHandle_tscUpdate;
void init::tim_tsc_update ()
{
  uint32_t __prescaler = (uint32_t) ((SystemCoreClock / 2) / 10000) - 1;
  __TIM3_CLK_ENABLE();
    
  TimHandle_tscUpdate.Instance = TIM3; 
  TimHandle_tscUpdate.Init.Period = 1000 - 1;
  TimHandle_tscUpdate.Init.Prescaler = __prescaler;
  TimHandle_tscUpdate.Init.ClockDivision = 0;
  TimHandle_tscUpdate.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&TimHandle_tscUpdate) != HAL_OK)
  {
    for (;;);
  }
	
  if(HAL_TIM_Base_Start_IT(&TimHandle_tscUpdate) != HAL_OK)
  {
    for(;;);
  }
}


TIM_HandleTypeDef             TimHandle_milis;
void init::tim_hal_milis ()
{
    uint32_t __prescaler = (uint32_t) ((SystemCoreClock / 2) / 10000) - 1;
  __TIM2_CLK_ENABLE();
    
  TimHandle_milis.Instance = TIM2; 
  TimHandle_milis.Init.Period = 20 - 1;
  TimHandle_milis.Init.Prescaler = __prescaler;
  TimHandle_milis.Init.ClockDivision = 0;
  TimHandle_milis.Init.CounterMode = TIM_COUNTERMODE_UP;
    
  if(HAL_TIM_Base_Init(&TimHandle_milis) != HAL_OK)
  {
    for (;;);
  }
  
  if(HAL_TIM_Base_Start_IT(&TimHandle_milis) != HAL_OK)
  {
    for(;;);
  }
}


gpio::gpio_dsc fsmc_pin_dsc[] = {
    {GPIOD, GPIO_PIN_0},
    {GPIOD, GPIO_PIN_1},
    {GPIOD, GPIO_PIN_4},
    {GPIOD, GPIO_PIN_5},
    {GPIOD, GPIO_PIN_6},
    {GPIOD, GPIO_PIN_7},
    {GPIOD, GPIO_PIN_8},
    {GPIOD, GPIO_PIN_9},
    {GPIOD, GPIO_PIN_10},
    {GPIOD, GPIO_PIN_11},
    {GPIOD, GPIO_PIN_12},
    {GPIOD, GPIO_PIN_13},
    {GPIOD, GPIO_PIN_14},
    {GPIOD, GPIO_PIN_15},
    
    {GPIOE, GPIO_PIN_0},
    {GPIOE, GPIO_PIN_1},
    {GPIOE, GPIO_PIN_7},
    {GPIOE, GPIO_PIN_8},
    {GPIOE, GPIO_PIN_9},
    {GPIOE, GPIO_PIN_10},
    {GPIOE, GPIO_PIN_11},
    {GPIOE, GPIO_PIN_12},
    {GPIOE, GPIO_PIN_13},
    {GPIOE, GPIO_PIN_14},
    {GPIOE, GPIO_PIN_15},
    
    {GPIOB, GPIO_PIN_7}, /*nadv*/
};

SRAM_HandleTypeDef                   hsram;
FMC_NORSRAM_TimingTypeDef            SRAM_Timing;

void init::fsmc ()
{
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __FSMC_CLK_ENABLE();
  for (auto dsc : fsmc_pin_dsc) {
      gpio::init_af(dsc, GPIO_NOPULL, GPIO_SPEED_HIGH, GPIO_AF12_FSMC, GPIO_MODE_AF_PP);
  }
  
  hsram.Instance  = FSMC_NORSRAM_DEVICE;
  hsram.Extended  = FSMC_NORSRAM_EXTENDED_DEVICE;
  
  SRAM_Timing.AddressSetupTime       = 0;
  SRAM_Timing.AddressHoldTime        = 0;
  SRAM_Timing.DataSetupTime          = 5;
  SRAM_Timing.BusTurnAroundDuration  = 0;
  SRAM_Timing.CLKDivision            = 2;
  SRAM_Timing.DataLatency            = 0;
  SRAM_Timing.AccessMode             = FSMC_ACCESS_MODE_C;

  hsram.Init.NSBank             = FSMC_NORSRAM_BANK1;
  hsram.Init.DataAddressMux     = FSMC_DATA_ADDRESS_MUX_ENABLE;
  hsram.Init.MemoryType         = FSMC_MEMORY_TYPE_NOR;
  hsram.Init.MemoryDataWidth    = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram.Init.BurstAccessMode    = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_HIGH;
  hsram.Init.WrapMode           = FSMC_WRAP_MODE_DISABLE;
  hsram.Init.WaitSignalActive   = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram.Init.WriteOperation     = FSMC_WRITE_OPERATION_ENABLE;
  hsram.Init.WaitSignal         = FSMC_WAIT_SIGNAL_ENABLE;
  hsram.Init.ExtendedMode       = FSMC_EXTENDED_MODE_DISABLE;
  hsram.Init.AsynchronousWait   = FSMC_ASYNCHRONOUS_WAIT_ENABLE;
  hsram.Init.WriteBurst         = FSMC_WRITE_BURST_DISABLE;
  /* Initialize the SRAM controller */
  if(HAL_SRAM_Init(&hsram, &SRAM_Timing, &SRAM_Timing) != HAL_OK)
  {
    /* Initialization Error */
    for (;;); 
  }  
}



void init::gpio ()
{
    gpio::init(led_pin_dsc, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_HIGH);
}

void init::clock_out ()
{
    __GPIOA_CLK_ENABLE();
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);
}


USBD_HandleTypeDef USBD_Device;

void init::usb ()
{
  USBD_Init(&USBD_Device, &MSC_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
  
  /* Add Storage callbacks for MSC Class */
  USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);
  
}

void init::rtc ()
{
  RTC_HandleTypeDef RtcHandle;
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
    
  RtcHandle.Instance = RTC;
    
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = 0x7F;
  RtcHandle.Init.SynchPrediv = 0x00FF;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    for (;;);
  }

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    for (;;);  
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    for (;;); 
  }
  
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, 0x32F2);  
}

gpio::gpio_dsc DacOut1Dsc = {GPIOA, GPIO_PIN_4};

DAC_HandleTypeDef       dac1Handle;
DMA_HandleTypeDef       dma157DacHandle;

void init::audio ()
{
  __GPIOA_CLK_ENABLE();
  gpio::init(DacOut1Dsc, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_HIGH);
  
  __DMA1_CLK_ENABLE();
  
  dma157DacHandle.Instance = DMA1_Stream5;
  
  dma157DacHandle.Init.Channel                 = DMA_CHANNEL_7;
  dma157DacHandle.Init.Direction               = DMA_MEMORY_TO_PERIPH;
  dma157DacHandle.Init.PeriphInc               = DMA_PINC_DISABLE;
  dma157DacHandle.Init.MemInc                  = DMA_MINC_ENABLE;
  dma157DacHandle.Init.PeriphDataAlignment     = DMA_PDATAALIGN_BYTE;
  dma157DacHandle.Init.MemDataAlignment        = DMA_MDATAALIGN_BYTE;
  dma157DacHandle.Init.Mode                    = DMA_NORMAL;
  dma157DacHandle.Init.Priority                = DMA_PRIORITY_HIGH;
  dma157DacHandle.Init.FIFOMode                = DMA_FIFOMODE_DISABLE;         
  dma157DacHandle.Init.FIFOThreshold           = DMA_FIFO_THRESHOLD_FULL;
  dma157DacHandle.Init.MemBurst                = DMA_MBURST_SINGLE;
  dma157DacHandle.Init.PeriphBurst             = DMA_PBURST_SINGLE; 
  
  if(HAL_DMA_Init(&dma157DacHandle) != HAL_OK)
  {
    for (;;);
  }
    
  /* Associate the initialized DMA handle to the the DAC handle */
  __HAL_LINKDMA(&dac1Handle, DMA_Handle1, dma157DacHandle);

  static TIM_HandleTypeDef htim;
  TIM_MasterConfigTypeDef  sMasterConfig;
  
  __TIM6_CLK_ENABLE();
  /*##-1- Configure the TIM peripheral #######################################*/
  /* Time base configuration */
  htim.Instance = TIM6;
  
  htim.Init.Period = 0x3b88;   /*11025 Hz*/       
  htim.Init.Prescaler = 0;       
  htim.Init.ClockDivision = 0;    
  htim.Init.CounterMode = TIM_COUNTERMODE_UP; 
  HAL_TIM_Base_Init(&htim);

  /* TIM6 TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
  
  /*##-2- Enable TIM peripheral counter ######################################*/
  HAL_TIM_Base_Start(&htim);
  
  static DAC_ChannelConfTypeDef dacSConfig;
  
  __DAC_CLK_ENABLE();
  dac1Handle.Instance = DAC;
  
  HAL_DAC_DeInit(&dac1Handle);
  if(HAL_DAC_Init(&dac1Handle) != HAL_OK)
  {
    for (;;);
  }
  dacSConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  dacSConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;  

  if(HAL_DAC_ConfigChannel(&dac1Handle, &dacSConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    for (;;);
  }
}

I2C_HandleTypeDef I2cHandle;
gpio::gpio_dsc I2c_2_pin_dsc[] = {
    {GPIOB, GPIO_PIN_8},
    {GPIOB, GPIO_PIN_9},    
};

void init::i2c1 ()
{
    __I2C1_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    for (auto dsc : I2c_2_pin_dsc) {
        gpio::init_af(dsc, GPIO_PULLUP, GPIO_SPEED_LOW, GPIO_AF4_I2C1, GPIO_MODE_AF_OD);
    }
    I2cHandle.Instance             = I2C1;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.ClockSpeed      = 100000;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    I2cHandle.Init.OwnAddress1     = 0xFF;
    I2cHandle.Init.OwnAddress2     = 0xFE;
		
    if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        __I2C1_CLK_DISABLE();
        for (;;) {}
    }
}

