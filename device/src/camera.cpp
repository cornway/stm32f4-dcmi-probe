#include "main.h"
#include "camera.h"
#include "gpio.h"

static DCMI_HandleTypeDef DCMIHandle;
static CameraFrame DCMIcurrentFrame;
static bool DCMIbusy;
        
HAL_StatusTypeDef halDcmiStartDma (uint32_t mode);
HAL_StatusTypeDef halMemStartDma ();
HAL_StatusTypeDef halDmaStopIt ();

void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{

}

static void dcmiDmaXFerCplt (DMA_HandleTypeDef *dma)
{ 
    DCMI_HandleTypeDef* hdcmi = ( DCMI_HandleTypeDef* )((DMA_HandleTypeDef* )dma)->Parent;
  
    __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);
    
    if((hdcmi->Instance->CR & DCMI_CR_CM) == DCMI_MODE_SNAPSHOT)
    {  
      hdcmi->State= HAL_DCMI_STATE_READY;
    }
    HAL_DMA_Abort_IT(DCMIHandle.DMA_Handle);
    DCMIbusy = false;  
}

static void dcmiDmaError (DMA_HandleTypeDef *dma)
{
  DCMI_HandleTypeDef* hdcmi = ( DCMI_HandleTypeDef* )((DMA_HandleTypeDef* )dma)->Parent;
  
  if(hdcmi->DMA_Handle->ErrorCode != HAL_DMA_ERROR_FE)
  {
    /* Initialize the DCMI state*/
    hdcmi->State = HAL_DCMI_STATE_READY;
  }

  /* DCMI error Callback */
  HAL_DCMI_ErrorCallback(hdcmi);
}



gpio::gpio_dsc dcmi_pin_dsc[] = {
    {GPIOA, GPIO_PIN_6},//clk
    {GPIOE, GPIO_PIN_4},//4
    {GPIOE, GPIO_PIN_5},//6
    {GPIOE, GPIO_PIN_6},//7
    {GPIOH, GPIO_PIN_8},//hs
    {GPIOH, GPIO_PIN_9},//0
    {GPIOH, GPIO_PIN_10},//1
    {GPIOH, GPIO_PIN_11},//2
    {GPIOH, GPIO_PIN_12},//3
    {GPIOI, GPIO_PIN_5},//vs
    {GPIOI, GPIO_PIN_4}//5
};

DMA_HandleTypeDef hdma;
int DCMIinit ()
{ 
    DCMIbusy = false;
  
  __GPIOA_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();  
  __GPIOH_CLK_ENABLE();
  __GPIOI_CLK_ENABLE();
    
  for (auto dsc : dcmi_pin_dsc) {
      gpio::init_af(dsc, GPIO_PULLDOWN, GPIO_SPEED_HIGH, GPIO_AF13_DCMI, GPIO_MODE_AF_OD);
  }
  
  __DCMI_CLK_ENABLE();
  __DMA2_CLK_ENABLE(); 
  
  hdma.Init.Channel             = DMA_CHANNEL_1;
  hdma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma.Init.MemInc              = DMA_MINC_ENABLE;
  hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  hdma.Init.Mode                = DMA_NORMAL;
  hdma.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;         
  hdma.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma.Init.MemBurst            = DMA_MBURST_SINGLE;
  hdma.Init.PeriphBurst         = DMA_PBURST_SINGLE; 

  hdma.Instance = DMA2_Stream1;

  __HAL_LINKDMA(&DCMIHandle, DMA_Handle, hdma);
  
  DCMIHandle.DMA_Handle->XferCpltCallback = dcmiDmaXFerCplt;
  DCMIHandle.DMA_Handle->XferErrorCallback = dcmiDmaError;
  DCMIHandle.DMA_Handle->XferAbortCallback = NULL;
  
  if (HAL_DMA_Init(DCMIHandle.DMA_Handle) != HAL_OK) {
      return -1;
  }    

      
  DCMIHandle.Init.CaptureRate      = DCMI_CR_ALL_FRAME;//DCMI_CR_ALTERNATE_4_FRAME;  
  DCMIHandle.Init.HSPolarity       = DCMI_HSPOLARITY_LOW;
  DCMIHandle.Init.SynchroMode      = DCMI_SYNCHRO_HARDWARE;//DCMI_SYNCHRO_EMBEDDED;
  DCMIHandle.Init.VSPolarity       = DCMI_VSPOLARITY_HIGH;
  DCMIHandle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  DCMIHandle.Init.PCKPolarity      = DCMI_PCKPOLARITY_RISING;
  DCMIHandle.Init.JPEGMode         = DCMI_JPEG_DISABLE;
  DCMIHandle.Instance              = DCMI;  
          
      /* DCMI Initialization */
  if (HAL_DCMI_Init(&DCMIHandle) != HAL_OK) {
      HAL_DMA_DeInit(DCMIHandle.DMA_Handle);
      return -1;
  }
      
  DCMIHandle.Instance->CR          |= DCMI_CR_CROP;
  DCMIHandle.Instance->ESCR        = (CAMERA_FSC_CODE << 0) | (CAMERA_LSC_CODE << 8) | (CAMERA_LEC_CODE << 16) | (CAMERA_FEC_CODE << 24);
  DCMIHandle.Instance->ESUR        = (CAMERA_MASK_CODE << 0) | (CAMERA_MASK_CODE << 8) | (CAMERA_MASK_CODE << 16) | (CAMERA_MASK_CODE << 24);
  return 0;
}

void DCMIDeInit ()
{
    HAL_DMA_DeInit(DCMIHandle.DMA_Handle);
    HAL_DCMI_DeInit(&DCMIHandle);
    __DCMI_CLK_DISABLE();
}

void DCMIrestart ()
{
    HAL_DMA_Abort_IT(DCMIHandle.DMA_Handle);
}

HAL_StatusTypeDef halDcmiStartDma (uint32_t mode)
{
  DCMI_HandleTypeDef* hdcmi = &DCMIHandle;
  uint32_t Length = CAMERA_WORDS_PER_FRAME(DCMIcurrentFrame.lineWidth, DCMIcurrentFrame.frameHeight);
  uint32_t pData = DCMIcurrentFrame.memory;
    

  __HAL_LOCK(hdcmi);
  hdcmi->State = HAL_DCMI_STATE_BUSY;

  hdcmi->Instance->CR &= ~(DCMI_CR_CM);
  hdcmi->Instance->CR |=  (uint32_t)(mode);

  hdcmi->DMA_Handle->XferCpltCallback = dcmiDmaXFerCplt;
  hdcmi->DMA_Handle->XferErrorCallback = dcmiDmaError;

  HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, Length);
  hdcmi->Instance->CR |= DCMI_CR_CAPTURE;

  __HAL_UNLOCK(hdcmi);
  return HAL_OK;
}


int DCMIsnapshotStart (CameraFrame cf)
{
    if (DCMIbusy == true) {
        return HAL_BUSY;
    }
    DCMIcurrentFrame = cf;
    DCMIbusy = true;
    
    DCMIHandle.Instance->CWSTRTR     = (CAMERA_GET_CLOCKS_PER_LINE(cf.lineStart) << 0) | (CAMERA_GET_LINES_PER_FRAME(cf.frameStart) << 16);
    DCMIHandle.Instance->CWSIZER     = (CAMERA_GET_CLOCKS_PER_LINE(cf.lineWidth) << 0) | (CAMERA_GET_LINES_PER_FRAME(cf.frameHeight) << 16);
    halDcmiStartDma (DCMI_MODE_SNAPSHOT); 
    __HAL_DCMI_ENABLE(&DCMIHandle);
    return HAL_OK;
}

int DCMIcontinuousStart (CameraFrame cf)
{
    return HAL_OK;
}


void DCMI_DMA_IrqHandler ()
{
    HAL_DMA_IRQHandler(DCMIHandle.DMA_Handle);  
}

void DCMIIrqHandler ()
{
    HAL_DCMI_IRQHandler(&DCMIHandle);
}

bool DCMIisBusy ()
{
    return DCMIbusy;
}




