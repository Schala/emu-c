#ifndef _GB_H
#define _GB_H

#include "../cpu.h"

uint8_t gb_read(const GB *, uint16_t);

void gb_write(GB *, uint16_t, uint8_t);

#endif // _GB_H
