#include "tsc2046.h"
#include <math.h>


/*
#define TSC_START_BP    (7U)
#define TSC_CHSEL_BP    (4U)
#define TSC_MODE_BP     (3U)
#define TSC_REF_BP      (2U)
#define TSC_PWR_BP      (0U)


#define TSC_START_BM    (1 << TSC_START_BP)
#define TSC_CHSEL_BM    (0x7 << TSC_CHSEL_BP)
#define TSC_MODE_BM     (1 << TSC_MODE_BP)
#define TSC_REF_BM      (1 << TSC_REF_BP)
#define TSC_PWR_BM      (0x3 << TSC_PWR_BP)
*/


static void _wait_us (int c);

enum {
    TSC_IDLE,
    TSC_CD,
    TSC_CLICK,
    TSC_TOUCH,
    TSC_RELEASE,
    TSC_STATE_ENUM,
} TSC_STATE;

enum {
    TSC_ON,
    TSC_OFF,
    TSC_INPUT_ENUM,
} TSC_INPUT;

int8_t tsc_state[TSC_STATE_ENUM][TSC_INPUT_ENUM];



void tsc2046Drv::init (bool it_ena)
{
    this->it_enabled = it_ena;
    this->ll_init(it_ena);
    this->ll_sel(true);
    this->invoke_flag = true;
}

tsc2046Drv::tsc2046Drv ()
{
    tsc_state[TSC_IDLE][TSC_OFF]        = TSC_IDLE;
    tsc_state[TSC_IDLE][TSC_ON]         = TSC_CLICK;
    tsc_state[TSC_CD][TSC_OFF]          = TSC_CD;
    tsc_state[TSC_CD][TSC_ON]           = TSC_CLICK;
    tsc_state[TSC_CLICK][TSC_OFF]       = TSC_RELEASE;
    tsc_state[TSC_CLICK][TSC_ON]        = TSC_TOUCH;
    tsc_state[TSC_TOUCH][TSC_OFF]       = TSC_RELEASE;
    tsc_state[TSC_TOUCH][TSC_ON]        = TSC_TOUCH;
    tsc_state[TSC_RELEASE][TSC_OFF]     = TSC_CD;
    tsc_state[TSC_RELEASE][TSC_ON]      = TSC_CD;
}

uint16_t tsc2046Drv::read (uint8_t command)
{

  // send control byte
  this->ll_sel(false);
  this->ll_rw(command);

  // wait
  _wait_us(TSC2046_SPI_BUSY_WAIT_DURATION);

  // receive
  uint16_t value = 0;
  uint8_t buf[2] = {0, 0};

  // if 8bit mode
  if (command & TSC2046_CB_8BIT)
  {
      buf[0] = this->ll_rw(0xff);
      value = buf[0];
  }
  else
  {
      buf[0] = this->ll_rw(0xff);
      buf[1] = this->ll_rw(0xff);
      // [0] = {x 11 10 9 8 7 6 5}
      // [1] = {4  3  2 1 0 x x x} x = dummy fill
      value = (buf[0] << 5 | buf[1] >> 3);
  }

  // unselect
  this->ll_sel(true);

#ifdef TSC2046_DEBUG
  // debug
  char debugbuf[15];
  sprintf(debugbuf, "read %04d\r\n", value);
  TSC2046_DEBUG_PRINT(debugbuf);
#endif
  return value;
}

int16_t tsc2046Drv::get_raw_z1 ()
{
    return this->read(TSC2046_COMMAND_READ_Z1);
}

int16_t tsc2046Drv::get_raw_z2 ()
{
    return this->read(TSC2046_COMMAND_READ_Z2);
}

bool tsc2046Drv::isTouching ()
{
    #ifdef TSC2046_IRQ_PIN
    return TSC2046_Touch_Status > 0 ? true : false;
    #else
    return this->get_raw_z1() > TSC2046_PRESSURE_THRESHOLD ? true : false;
    #endif
}

int16_t tsc2046Drv::read_filter (uint8_t command, uint8_t maxSample)
{
  if (maxSample < 1)
  {
    return -1;
  }

  int16_t result = 0, i = 0;
  uint16_t buf[TSC2046_CAL_MAX_SAMPLE];

#if TSC2046_SAMPLING_MODE == 0
  // simple average
  uint32_t value = 0;
  while (++i <= maxSample && this->isTouching())
  {
    value += this->read(command);
  }
  result = value / i;
#elif TSC2046_SAMPLING_MODE == 1
  // weighted average
  for (i = 0; i < maxSample && this->isTouching(); i++)
  {
    buf[i] = this->read(command);
  }
  if (i < 2)
  {
    result = -1;
  }
  else
  {
    uint8_t j;
    uint32_t t = 0;
    for (j = 1; j < i - 1; j++)
    {
      t += this->read(command);
    }
    result = t / (i - 2);
  }
#elif TSC2046_SAMPLING_MODE == 2
  // median
  for (i = 0; i < maxSample && this->isTouching(); i++)
  {
    buf[i] = this->read(command);
  }
  // sort
  int16_t temp, n = i, j;
  for (i = 0; i < n - 1; i++)
  {
    for (j = i + 1; j < n; j++)
    {
      if (buf[j] < buf[i])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  // get median
  i = n / 2;
  if (n % 2 == 0)
  {
    result = (buf[i] + buf[i + 1]) / 2;
  }
  else
  {
    result = buf[i];
  }
#elif TSC2046_SAMPLING_MODE == 3
  // averaging the closest data
  for (i = 0; i < maxSample && this->isTouching(); i++)
  {
    buf[i] = this->read(command);
  }
  // diff
  int n = i - 1, diff[TSC2046_MAX_SAMPLE];
  for (i = 0; i < n; i++)
  {
    diff[i] = abs((float)buf[i + 1] - (float)buf[i]);
  }
  // search minimum diff
  int di = -1, dm = 4096;
  for (i = 0; i < n; i++)
  {
    if (dm > diff[i])
    {
      dm = diff[i];
      di = i;
    }
  }
  // set average
  result = (buf[di] + buf[di + 1]) / 2;

#elif TSC2046_SAMPLING_MODE == 4
  // pressure
  uint16_t z1 = this->read(TSC2046_CB_Z1_POSITION),
           z2 = this->read(TSC2046_CB_Z2_POSITION),
           x = this->read(TSC2046_CB_X_POSITION),
           q = command & TSC2046_CB_8BIT ? 256 : 4096,
           rx = 1000;// Rx X-plate resistance, temporary
  // when z1 == 0, released
  int16_t pressure = z1 > 0 ? rx * x / q * (z2 / z1 - 1) : -1;
  // if p<th, error
  result = pressure > TSC2046_PRESSURE_THRESHOLD ? this->read(command) : -1;
#endif

  return result;
}

#include <math.h>

#if TSC2046_CALIBRATION_MODE == 0
typedef struct {
  int16_t sx;
  int16_t sy;
  int16_t vx;
  int16_t vy;
} _TSC2046_Cal_Points;

typedef struct {
  double ax;
  double bx;
  double dx;
  double ay;
  double by;
  double dy;
} _TSC2046_Cal_Matrix;

volatile _TSC2046_Cal_Matrix _mcal = {1, 0, 0, 0, 1, 0};

#include "ff.h"

int tsc2046Drv::loadCalData (char *path)
{
    FIL *file = new FIL;
    if (file == nullptr) {
        TOUCH_CAL_LCD_CLEAR();
        TOUCH_CAL_LCD_DRAW_STRING("TOO SMALL CORE. DEVICE HALTED", 10, 10);
        return TSC2046_CAL_SMALL_CORE;
    }
    FRESULT res = FR_OK;
    res = f_open(file, path, FA_READ);
    if (res != FR_OK) {
        delete file;
        return TSC2046_CAL_FILE_NOT_FOUND;
    }
    uint32_t f_bytes_read = 0;
    res = f_read(file, (void *)&_mcal, sizeof(_TSC2046_Cal_Matrix), &f_bytes_read);
    if (res != FR_OK) {
        f_close(file);
        delete file;
        return TSC2046_CAL_FILE_NOT_FOUND;
    }
    f_close(file);
    delete file;
    return TSC2046_CAL_OK;
}

int tsc2046Drv::saveCalData(char *path)
{
    FIL *file = new FIL;
    if (file == nullptr) {
        TOUCH_CAL_LCD_CLEAR();
        TOUCH_CAL_LCD_DRAW_STRING("TOO SMALL CORE. DEVICE HALTED", 10, 10);
        return TSC2046_CAL_SMALL_CORE;
    }
    FRESULT res = FR_OK;
    res = f_open(file, path, FA_WRITE | FA_CREATE_NEW);
    if (res == FR_EXIST) {
        res = f_unlink(path);
        res = f_open(file, path, FA_WRITE | FA_CREATE_NEW);
    }
    if (res != FR_OK) {
        delete file;
        TOUCH_CAL_LCD_CLEAR();
        TOUCH_CAL_LCD_DRAW_STRING("FILE WRITE DENIED", 10, 10);
        return TSC2046_CAL_WRITE_DENIED;
    }
    uint32_t f_write_bytes;
    res = f_write(file, (void *)&_mcal, sizeof(_TSC2046_Cal_Matrix), &f_write_bytes);
    if (res != FR_OK) {
        f_close(file);
        delete file;
        TOUCH_CAL_LCD_CLEAR();
        TOUCH_CAL_LCD_DRAW_STRING("FILE WRITE DENIED", 10, 10);
        return TSC2046_CAL_WRITE_DENIED;
    }
    f_close(file);
    delete file;
    return TSC2046_CAL_OK;
}

// three point calibration
int tsc2046Drv::calibration (char *path)
{
  // message
  if (TOUCH_CAL_LCD_FRAME_OPEN() < 0) {
      for (;;) {} /*fatal error*/
  }
  TOUCH_CAL_LCD_CLEAR();
  TOUCH_CAL_LCD_DRAW_STRING("START CALIBRATION... (TOUCH THE SCREEN TO BEGIN)", 10, 10);
  while (this->isTouching() == false) {}
  while (this->isTouching() == true) {}

  // message
  TOUCH_CAL_LCD_CLEAR();

  // generate marker points
  uint16_t i = 0,
    gx = TSC2046_WIDTH / 4,
    gy = TSC2046_HEIGHT / 4;

  _TSC2046_Cal_Points p[3] = {
      {gx, gy},
      {gx *3, gy * 2},
      {gx, gy * 3}
  };
  while (i < 3)
  {
    // show marker
    TOUCH_CAL_LCD_DRAW_STRING("Please touch marker...", 10, 10);
    TOUCH_CAL_LCD_DRAW_CIRCLE(p[i].sx, p[i].sy, 5);

    // read
    while (this->isTouching() == false);
    p[i].vx = this->read_filter(TSC2046_COMMAND_READ_X, TSC2046_CAL_MAX_SAMPLE);
    p[i].vy = this->read_filter(TSC2046_COMMAND_READ_Y, TSC2046_CAL_MAX_SAMPLE);

    // wait release
    while (this->isTouching() == true);

    // check value
    if (p[i].vx > 0 && p[i].vy > 0)
    {
      TOUCH_CAL_LCD_CLEAR();
      i++;
    }
    else
    {
      TOUCH_CAL_LCD_DRAW_STRING("Touch error, please retry.", 10, 20);
    }
  }

  // calculate matrix determinant
  double detA = (p[0].vx - p[2].vx) * (p[1].vy - p[2].vy)
      - (p[1].vx - p[2].vx) * (p[0].vy - p[2].vy);
  double detAx1 = (p[0].sx - p[2].sx) * (p[1].vy - p[2].vy)
      - (p[1].sx - p[2].sx) * (p[0].vy - p[2].vy);
  double detAx2 = (p[0].vx - p[2].vx) * (p[1].sx - p[2].sx)
      - (p[1].vx - p[2].vx) * (p[0].sx - p[2].sx);
  double detAx3 = p[0].sx *(p[1].vx * p[2].vy - p[2].vx * p[1].vy)
      - p[1].sx *(p[0].vx * p[2].vy - p[2].vx * p[0].vy)
      + p[2].sx *(p[0].vx * p[1].vy - p[1].vx * p[0].vy);
  double detAy1 = (p[0].sy - p[2].sy) * (p[1].vy - p[2].vy)
      - (p[1].sy - p[2].sy) * (p[0].vy - p[2].vy);
  double detAy2 = (p[0].vx - p[2].vx) * (p[1].sy - p[2].sy)
      - (p[1].vx - p[2].vx) * (p[0].sy - p[2].sy);
  double detAy3 = p[0].sy *(p[1].vx * p[2].vy - p[2].vx * p[1].vy)
        - p[1].sy *(p[0].vx * p[2].vy - p[2].vx * p[0].vy)
        + p[2].sy *(p[0].vx * p[1].vy - p[1].vx * p[0].vy);

  // save
  _mcal.ax = detAx1 / detA;
  _mcal.bx = detAx2 / detA;
  _mcal.dx = detAx3 / detA;
  _mcal.ay = detAy1 / detA;
  _mcal.by = detAy2 / detA;
  _mcal.dy = detAy3 / detA;
   TOUCH_CAL_LCD_FRAME_CLOSE();
   return this->saveCalData(path);
}

// get calibrated values
int16_t tsc2046Drv::getPositionX ()
{
  // X = alphaX * rawX + betaX * rawY + deltaX;
  int16_t rawX = this->read_filter(TSC2046_COMMAND_READ_X, TSC2046_MAX_SAMPLE),
          rawY = this->read_filter(TSC2046_COMMAND_READ_Y, TSC2046_MAX_SAMPLE);
  if (rawX < 0 || rawY < 0)
  {
    return -1;
  }
  int16_t x = _mcal.ax * rawX + _mcal.bx * rawY + _mcal.dx;

  // clip to LCD resolution
  return x < 0 ? 0 : (x < TSC2046_WIDTH ? x : TSC2046_WIDTH - 1);
}

int16_t tsc2046Drv::getPositionY ()
{
  // Y = alphaY * rawX + betaY * rawY + deltaY;
  uint16_t rawX = this->read_filter(TSC2046_COMMAND_READ_X, TSC2046_MAX_SAMPLE),
           rawY = this->read_filter(TSC2046_COMMAND_READ_Y, TSC2046_MAX_SAMPLE);
  if (rawX < 0 || rawY < 0)
  {
    return -1;
  }
  int16_t y = _mcal.ay * rawX + _mcal.by * rawY + _mcal.dy;

  // clip to LCD resolution
  return y < 0 ? 0 : (y < TSC2046_HEIGHT ? y : TSC2046_HEIGHT - 1);
}

#elif TSC2046_CALIBRATION_MODE == 1
    #error "this calibration mode is not supported in current version!"
#endif

static void point_clr (TouchPointTypeDef *p)
{
    p->x = 0;
    p->y = 0;
    p->hits = 0;
    p->onPressTime = 0;
}


static int16_t state_cd = 0;
static int8_t state = TSC_IDLE;
int32_t tsc2046Drv::invoke (TouchPointTypeDef *point, uint32_t width, uint32_t height)
{
    if (this->invoke_flag) {
        this->invoke_flag = false;
        state = tsc_state[state][this->isTouching() == true ? TSC_ON : TSC_OFF];
        switch (state) {
            case TSC_IDLE :     state_cd = TSC_CD_TIMEOUT;
                break;
            case TSC_CD :       if (--state_cd < 0) {
                                    point->hits = 0;
                                    state_cd = TSC_CD_TIMEOUT;
                                    point_clr(point);
                                    state = TSC_IDLE;
                                }
                break;
            case TSC_CLICK :    state_cd = TSC_CD_TIMEOUT;
                                point->x = this->getPositionX();
                                point->y = this->getPositionY();
                                point->hits++;   
                break;
            case TSC_TOUCH :    point->x = this->getPositionX();
                                point->y = this->getPositionY();
                                point->onPressTime++;
                break;
            case TSC_RELEASE :  
                break;
        }
        
        int32_t event = -1;
        static TouchPointTypeDef __point;
        static uint16_t touchTime = 0;
        switch (state) {
            case TSC_CLICK :    event = SENSOR_CLICK;
                                __point = *point;
                break;
            case TSC_TOUCH :    if (touchTime > TSC_HOLD_TIMEOUT) {
                                    event = SENSOR_HOLD;
                                    touchTime = TSC_HOLD_TIMEOUT - (TSC_HOLD_TIMEOUT > 1 ? 1 : 0);
                                } else {
                                    event = SENSOR_TOUCH;
                                    touchTime++;
                                }
                                __point = *point;
                break;
            case TSC_RELEASE :  touchTime = 0;
                                event = SENSOR_RELEASE;
                break;
        }
        if (event >= 0) {
            this->eventBurner.fireEvents(abstract::Event((void *)&__point, event));
        }
        return 0;
    } else {
        return -1;
    }
}


void tsc2046Drv::IT_Handle ()
{
    if (!this->it_enabled)
        return;
    this->invoke_flag = true;
}

void tsc2046Drv::tim_it_handle ()
{
    if (!this->it_enabled)
        this->invoke_flag = true;
}

static void _wait_us (int c)
{
    c = c * 1000;
    while (c--) {}
}

