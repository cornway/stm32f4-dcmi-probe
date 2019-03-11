#ifndef VM_MAIL_H
#define VM_MAIL_H

#include "machM4.h"

typedef _PACKED struct {
    char *message;
    void *object;
    WORD_T type;
} MAIL_HANDLE;

#endif /*VM_MAIL_H*/

