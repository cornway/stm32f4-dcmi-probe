#include "VM.h"
#include "vmapi.h"
#include "fgpu.h"
#include "gui.h"
#include "app_def.h"
#include <string.h>
#include "exec_interface.h"
#include "device_conf.h"
#include "explorer_app.h"
#include "cam_app.h"

#define HELP_INFO \
"CMD INTRPRETER:0.01\n\
BASIC SYNTAX :\
$[CMD] --[ARG 0],  --[ARG 1] ... \
CMD LIST : F, R, E, ...\
$RUN --[APPLICATION] --[ARGS]: \
RUN A PRE-DEFINED APPLICATION WITH [ARGS] AS ARGUMENTS; \
"

/*
cmd examples
any cmd must to begin with '$' character;
any cmd string must end with '\0' character;

$[cmd] --[option 0], --[option 1] ...etc;

$F --[file] : open a file with [file] name, expample $f --conf.txt
$R --[file] : read a file with [file] name, currently deprecated

$E : open explorer


$RUN --[application] : runs a pre-defined application, example $run --cam, - will run a cam decoder
$LOGON [PASSWORD] : logon to account
$EXIT : exit from current account

$HELP - show help iformation;


applications :
CAM - run camera decoder
DEC[0x??=0x??] - write camera decoder register
*/

#define CMD_MAX_STR_SIZE    (384U)

static char cmdString[8];
static char cmdOptionString[24];
static char cmdBuffer[CMD_MAX_STR_SIZE];

static void flush (char *dest, int size);
static int removeSpaces (char *dest, char *src);
static bool isSpace (char c);

char *execute (char *cmd)
{
    if (cmd[0] != '$') {
        return (char *)"ERROR : UNKNOWN COMMAND";
    }
    if (isSpace(cmd[1]) == true) {
        return (char *)"ERROR : TRAILING SPACES BETWEEN '$' AND CMD KEYWORD\n YOU MUST USE $[CMD] ...";
    }
    flush(cmdBuffer, CMD_MAX_STR_SIZE);
    if (removeSpaces(cmdBuffer, cmd + 1) < 0) {
        return (char *)"ERROR : TOO LARGE COMMAND STRING";
    }
    int errorCode = -1;
    char *path = "";
    char *cmdOption = "";
    WORD_T option = 0;
    switch (cmdBuffer[0]) {
        case 'H':
                    if (strcmp(cmdBuffer + 1, "ELP") == 0) { /*help cmd*/
                        if (cmdBuffer[5] != '\0') {
                            errorCode = -2;
                            break;
                        }
                        return (char *)HELP_INFO;
                    } else {
                        errorCode = -1;
                        break;
                    }
        case 'E' :  if (strncmp(cmdBuffer + 1, "XIT", 3) == 0) { /*exit from curren account*/
                        account_exit();
                        return "EXIT FROM CURRENT ACCOUNT SUCCESSFUL!";
                    } 
                    if (*(cmdBuffer + 1) == '/') {
                        _XCALL(xret, expl_app, EXPLORER_APP_USE_FILTER, cmdBuffer + 2);
                    } else {
                        _XCALL(xret, expl_app, EXPLORER_APP_USE_FILTER, cmdBuffer + 1);
                    }
                    errorCode = 0;
                    break;
        case 'F' :  if ((cmdBuffer[2] != '-') && (cmdBuffer[3] != '-')) {
                        path = cmdBuffer + 4;
                        _XCALL(_xret, file_app, 0, path);
                        switch (_xret) {
                            case UNSUPPORTED_FILE_TYPE :    return (char *)"UNKNOWN \nFILE\n TYPE";
                                
                            case ADV_CONF_OK :              return (char *)"ADV CONF \nOK";
                                
                            case  ADV_CONF_FAILED :         return (char *)"ADV CONF \nFAILED";
                               
                            default :                       return (char *)"UNKNOWN\n OPERATION";
                                
                        }
                    } else {
                        errorCode = -2;
                        break;
                    }
        case 'R' :  if ((cmdBuffer[1] == 'U') && (cmdBuffer[2] == 'N')) { /*run cmd*/
                        errorCode = 1;
                        cmdOption = cmdBuffer + 3;
                    } else {
                        errorCode = -1;
                        break;
                    }
            break;
        case 'L' :  if (strncmp(cmdBuffer + 1, "OGON", 4) == 0) { /*try to logon*/
                        if (account_logon(cmdBuffer + 5) == 0) { /*access granted*/
                            return (char *)"LOGON SUCCEESS!";
                        }
                        return (char *) "LOGON FAILED!";
                    } else {
                        errorCode = -1;
                        break;
                    }
            break;
        default :   errorCode = -1;
            break;
    }
    
    if (errorCode > 0) {
        do {
            switch (errorCode) {
                case 1 :    if ((cmdOption[0] != '-') || (cmdOption[1] != '-')) {
                                errorCode = -4;
                                break;
                            }
                            cmdOption = cmdOption + 2;
                            if (cmdOption[0] == 'C') { /*run cam decoder application ?*/
                                if ((cmdOption[1] == 'A') && (cmdOption[2] == 'M')) {
                                    _XCALL(__xret, cam_app, 1, (void *)(cmdOption + 3));
                                    errorCode = 0;
                                    break; 
                                } else {
                                    errorCode = -2;
                                    break;
                                }
                            } else if (cmdOption[0] == 'D') { /*write decoder register ? */
                                if ((cmdOption[1] == 'E') && (cmdOption[2] == 'C')) {
                                    if (writeVideoDecoderRegFromStr(cmdOption + 3) != 0) {
                                        return "ERROR : CANT PARSE VALUES\n TO WRITE VIDEO DECODER";
                                    } else {
                                        errorCode = 0;
                                        break; 
                                    }
                                } else {
                                    errorCode = -3;
                                    break; 
                                }
                            } else {
                                errorCode = -3;
                                break;
                            }
                default :   errorCode = -1;
                    break;
            }
        } while (errorCode > 0);
    }
    
    switch (errorCode) {
        case 0 :  return (char *) "OK";
            
        case -1 : return (char *)"ERROR : UNDEFINED COMMAND";
            
        case -2 : return (char *)"ERROR : GARBAGE AT END OF COMMAND";
            
        case -3 : return (char *)"ERROR : CANT RUN APPLICATION";
            
        case -4 : return (char *)"ERROR : WRONG NAME OR CMD ARGUMENT";
            
        default : return (char *)"ERROR : UNHANDLED ERROR CODE";
            
    }
}



static int removeSpaces (char *dest, char *src)
{
    int i = 0;
    int t = 0;
    while (src[i] != '\0') {
        if (isSpace(src[i]) == true) {
            i++;
            continue;
        }
        dest[t++] = src[i++];
        if (i == CMD_MAX_STR_SIZE) {
            return -1;
        }
    }
    return 0;
}

static bool isSpace (char c)
{
    if (c == ' ') {
        return true;
    }
    if (c == '\n') {
        return true;
    }
    if (c == '\t') {
        return true;
    }
    return false;
}

static void flush (char *dest, int size)
{
    while (size >= 0) {
        dest[size--] = '\0';
    }
}
