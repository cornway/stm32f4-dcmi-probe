#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include "app_macro.h"
#include "app_def.h"
#include "device_conf.h"
#include "vm.h"
#include "vmapi.h"
#include "gui.h"

#define APP_IPV4        (-1)
#define APP_MAC         (-2)
#define APP_SLEEP_PARAM (1U)
#define APP_GACC_PARAM  (2U)
#define APP_GQTY_PARAM  (3U)
#define APP_SOUND_PARAM (4U)
#define APP_ENRGY_PARAM (5U)
#define APP_DHCP_PARAM  (6U)
#define APP_BRI_PARAM   (7U)
#define APP_SNAP_PARAM  (8U)

extern FontArray fontArray;
extern APPLICATION_CONTROL applicationControl;
extern int update_ipv4 (char *token, uint8_t *ip);
extern int update_mac (char *token, uint8_t *mac);

INT_T settings_app (WORD_T size, void *argv);


static void setChangesSaved (bool);
static void setChangesApplied (bool);

static void update_sprites ();
static void clear_sprites ();
static inline void update_descktop ();
static int save_config (char *path);
static int load_config (char *path);
static int save_local_cfg (APPLICATION_CONTROL *, APPLICATION_CONTROL *);
static int load_local_cfg (APPLICATION_CONTROL *);
static int change_local_param (APPLICATION_CONTROL *, int);

static void checkEditable ();
static void edit (APPLICATION_CONTROL *ctl);

#endif  /*SETTINGS_APP_H*/