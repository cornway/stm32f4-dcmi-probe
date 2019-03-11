#ifndef APP_DEF_H
#define APP_DEF_H


#include <stdint.h>


#ifndef _PACKED
#define _PACKED __packed
#endif

#define EXEC_APP_PASSWORD_REQUEST   (0U)
#define EXEC_APP_EXECUTE_REQUEST    (1U)
#define EXEC_APP_EDIT_REQUEST       (2U)


#define     UNSUPPORTED_FILE_TYPE      (1U)
#define     FILE_OPEN_OK               (2U)
#define     FILE_OPEN_ERROR            (3U)
#define     ADV_CONF_OK                (4U)
#define     ADV_CONF_FAILED            (5U)

#define ALERT_TEXT_COLOR    (RGB24_TO_RGB16(0,0,0))
#define ALERT_BACK_COLOR    (RGB24_TO_RGB16(173,216,230))
#define ALERT_KEY_COLOR     (RGB24_TO_RGB16(65,105,225))

#define PRIVILEGE_USER      (0U)
#define PRIVILEGE_ADMIN     (1U)



#define EXPLORER_APP_USE_FILTER     (1U)
#define EXPLORER_APP_NO_FILTER      (0U)
#define EXPLORER_APP_GET_PATH       (0x80U)

#define PASSWORD_MAX_SIZE           24
#define DEFAULT_ADMIN_PASSWORD      ("112")
#define CONF_DATA_FILE_SIZE         (128)

#define ACCOUNT_ACCESS_FAILURE          (-1)
#define ACCOUNT_ACCESS_GRANTED_ONCE     (0)
#define ACCOUNT_ACCESS_GRANTED_FOREVER  (1U)


#pragma anon_unions

typedef struct {
    uint8_t seconds, minutes, hours;
} DAY_TIME;

typedef struct {
    uint8_t privilege;
    char password[PASSWORD_MAX_SIZE];
} ACCESS_CONTROL;

char *execute (char *);


#define     FRAME_RENDER_LOCK_ID    (0x00000001U)
#define     FILE_SYSTEM_LOCK_ID     (0x00000002U)
#define     MEMORY_ALLOC_LOCK_ID    (0x00000003U)
#define     SENSOR_LOCK_ID          (0x00000004U)
#define     USB_LOCK_ID             (0x00000005U)


typedef _PACKED struct {
    
} ANALOG_CONTROL;

#define GRAPHIC_QUALITY_BEST    (2U)
#define GRAPHIC_QUALITY_GOOD    (1U)
#define GRAPHIC_QUALITY_FAST    (0U)

#define GRAPHIC_ACCELERATION_ON  (1U)
#define GRAPHIC_ACCELERATION_OFF (0U)

typedef _PACKED struct {
    unsigned quality : 2;
    unsigned acceleration : 1;
} GRAPHIC_CONTROL;

typedef _PACKED struct {
    
} VIDEO_CONTROL;


#define SOUND_ON     (1U)
#define SOUND_OFF    (0U)

typedef _PACKED struct {
    unsigned soundOn : 1;
} AUDIO_CONTROL;

#define POWER_CONSUMPTION_REDUCED   (1U)
#define POWER_CONSUMPTION_FULL      (0U)
#define POWER_CONSUMPTION_SLEEP     (2U)

typedef _PACKED struct {
    unsigned powerConsumption : 2;
} POWER_CONTROL;

#define TFT_BACKLIGHT_ON    (1U)
#define TFT_BACKLIGHT_OFF   (0U)

typedef _PACKED struct {
    unsigned backlightLock : 1;
    unsigned backlight : 1;
    unsigned backlightValue : 7;
    unsigned defaultBacklightValue : 7;
} TFT_CONTROL;


typedef _PACKED struct {
    char ipv4Str[17];
    _PACKED struct {
        uint8_t ipv4_ip[4];
    };
    
    char macStr[20];
    _PACKED struct {
        uint8_t mac[6];
    };
    
    bool useDHCP;
} NETWORK_CONTROL;

typedef _PACKED struct {
    ANALOG_CONTROL analogControl;
    GRAPHIC_CONTROL graphicControl;
    VIDEO_CONTROL videoControl;
    AUDIO_CONTROL audioControl;
    POWER_CONTROL powerControl;
    TFT_CONTROL tftControl;
    NETWORK_CONTROL networkControl;
    int32_t sleepTimeout;
    int8_t snapshotBpp;
} APPLICATION_CONTROL;

    
extern APPLICATION_CONTROL applicationControl;
extern void appOnActionHook ();
extern int snapshot_1bpp (char *name);
extern int snapshot_4bpp (char *name);

int logon_once ();

#endif /*APP_DEF_H*/

