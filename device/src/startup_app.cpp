#include "startup_app.h"
#include "time.h"


APPLICATION_CONTROL applicationControl;
uint16_t batteryValue;
uint32_t sleepTimeout = 0;

FontArray fontArray;



extern void usbdStart (void);
INT_T VM_SYS_THREAD (WORD_T size, void *arg)
{
    time::delay_ms(200);
    control_init ();
    tft.init();
    sleepTimeout = 0;
    /*
    usbdStart();
    for (;;) {
        
    }
    */
    int tsc2046_cal_res = TSC2046_CAL_FILE_NOT_FOUND;
    tsc2046_cal_res = tsc2046.loadCalData("tsc2046.cal");
    if (tsc2046_cal_res == TSC2046_CAL_FILE_NOT_FOUND) {
        tsc2046_cal_res = tsc2046.calibration("tsc2046.cal");
    }
    if (tsc2046_cal_res != TSC2046_CAL_OK) {
        for (;;) {}
    }
    
    init_adc_bat();
    
    static THREAD_HANDLE th;
    th.Arg = 0;
    th.argSize = 0;
    th.Callback = render_app;
    th.Name = "render\0";
    th.Priority = 2;
    th.StackSize = 512;
    ARG_STRUCT_T ret = vm::create(&th);
    th.Callback = main_app;
    th.Name = "main\0";
    th.Priority = 5;
    th.StackSize = 8196;
    ret = vm::create(&th);
    th.Arg = &tsc2046;
    th.Callback = SENSOR_THREAD;
    th.Name = "sensor\0";
    th.Priority = 3;
    th.StackSize = 512;
    ret = vm::create(&th);
    for (;;) {  
        vm::yield();     
    }   
}

static INT_T SENSOR_THREAD (WORD_T size, void *argv)
{
    tsc2046Drv *sensor = (tsc2046Drv *)argv;
    sensor->addListener([](abstract::Event e) -> void {
            static TouchPointTypeDef tp;
            static int cause;
            if (sleepTimeout >= applicationControl.sleepTimeout) {
                if (e.getCause() == SENSOR_RELEASE) {
                    applicationControl.powerControl.powerConsumption = POWER_CONSUMPTION_FULL;
                    applicationControl.tftControl.backlight = TFT_BACKLIGHT_ON;
                    sleepTimeout = 0;
                    vm::fire_event("wakeup");
                }
                return;
            }
            tp = *(TouchPointTypeDef *)e.getSource();
            cause = e.getCause();
            mainAppTouchSensor.setEvent(tp, cause);  
            camAppTouchSensor.setEvent(tp, cause);   
            explAppTouchSensor.setEvent(tp, cause);  
            infoAppTouchSensor.setEvent(tp, cause); 
            execAppTouchSensor.setEvent(tp, cause);
            analogAppTouchSensor.setEvent(tp, cause);
            settingsAppTouchSensor.setEvent(tp, cause);
            pnesxAppTouchSensor.setEvent(tp, cause);
            snakeAppTouchSensor.setEvent(tp, cause);
            if ((e.getCause() == SENSOR_CLICK)) {
                if (applicationControl.audioControl.soundOn == SOUND_ON) {
                    if (clickWave != nullptr) {
                        clickWave->play();   
                    }
                }
            }
            sleepTimeout = 0;
            
    });
    for (;;) {
        vm::lock(SENSOR_LOCK_ID);
        sensor->invoke(&sp, TFT_WIDTH, TFT_HEIGHT);
        vm::unlock(SENSOR_LOCK_ID);
        vm::sleep(1);
        if (sleepTimeout < applicationControl.sleepTimeout) {
            sleepTimeout++;
            if (sleepTimeout > (applicationControl.sleepTimeout / 2)) {
                applicationControl.tftControl.backlightValue = BRIGHTNESS_LOW_LEVEL;
            } else {
                applicationControl.tftControl.backlightValue = applicationControl.tftControl.defaultBacklightValue;
            }
        } else {
            applicationControl.powerControl.powerConsumption = POWER_CONSUMPTION_SLEEP;
            if (applicationControl.tftControl.backlightLock == 0) {
                applicationControl.tftControl.backlight = TFT_BACKLIGHT_OFF;
            }
        }
        batteryValue = measureBattery();
    }
    //return 0;
}

static WaveSample *sample = nullptr;;
void updateAudio (WaveSample **s, char *path)
{
    sample = *s;
    *s = nullptr;
    FIL *file = new FIL;
    if (file == nullptr) {
        *s = sample;
        return;
    }
    WAVE *wave = new WAVE(path);
    if (wave == nullptr) {
        delete file;
        *s = sample;
        return;
    }
    if (wave->open(file) != WAVE_OK) {
        delete file;
        delete wave;
        *s = sample;
        return;
    }
    WAVE_DSC waveDsc = wave->getInfo();
    uint8_t *buffer = new uint8_t[waveDsc.dataSize];
    if (buffer == nullptr) {
        delete file;
        delete wave;
        *s = sample;
        return;
    }
    *s = new WaveSample(path, waveDsc, buffer);
    if (*s == nullptr) {
        delete file;
        delete wave;
        delete buffer;
        *s = sample;
        return;
    }
    if (sample != nullptr) {
        delete sample->getBuffer();
        delete sample;
    }
    wave->loadSample(file, buffer);
    delete wave;
    delete file;
}

void appOnActionHook ()
{
    if (applicationControl.audioControl.soundOn == SOUND_OFF) {
        return;
    }
    time::delay_ms(200);
    if (confirmWave != nullptr) {
        confirmWave->play();
    }
}

static void control_init ()
{
    applicationControl.audioControl.soundOn                 = SOUND_OFF;
    applicationControl.graphicControl.quality               = GRAPHIC_QUALITY_BEST;
    applicationControl.graphicControl.acceleration          = GRAPHIC_ACCELERATION_ON;
    applicationControl.powerControl.powerConsumption        = POWER_CONSUMPTION_FULL;
    applicationControl.tftControl.backlightLock             = 0;
    applicationControl.tftControl.backlight                 = TFT_BACKLIGHT_ON;
    applicationControl.tftControl.backlightValue            = 0;
    applicationControl.tftControl.defaultBacklightValue     = BRIGHTNESS_MAX_LEVEL;
    applicationControl.sleepTimeout                         = 10000;
    applicationControl.snapshotBpp                          = 1;
    
    strcpy(applicationControl.networkControl.ipv4Str, "192.168.0.1");
    strcpy(applicationControl.networkControl.macStr, "00.00.00.00.00.00"); 
    
    update_ipv4("192.168.0.1", applicationControl.networkControl.ipv4_ip);
    update_mac("00.00.00.00.00.00", applicationControl.networkControl.mac);
}



#include "stm32f4xx_hal.h"

ADC_HandleTypeDef    AdcHandle;

static void init_adc_bat ()
{
  __ADC1_CLK_ENABLE();
    
  ADC_ChannelConfTypeDef sConfig;
  AdcHandle.Instance                   = ADC1;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;
  AdcHandle.Init.ContinuousConvMode    = DISABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;
      
  if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* Initialization Error */
    for (;;);
  }
  
  /*##-2- Configure ADC regular channel ######################################*/  
  sConfig.Channel      = ADC_CHANNEL_VBAT;
  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
  sConfig.Offset       = 0;
  
  if(HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    for(;;);
  }
}

static uint16_t measureBattery ()
{
  uint16_t value = 0;
  if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
  {
    for(;;);
  }
  HAL_ADC_PollForConversion(&AdcHandle, 10);
  value = HAL_ADC_GetValue(&AdcHandle);
  
  return value;
}
