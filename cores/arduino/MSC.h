#if defined (USBCON)

#ifndef __MSC_H__
#define __MSC_H__

#include "api/USBAPI.h"

//================================================================================
//================================================================================
//	MSC 'Driver'

int		MSC_GetInterface(uint8_t* interfaceNum);
int		MSC_GetDescriptor(int i);
bool	MSC_Setup(USBSetup& setup);
bool	MSC_Data(uint8_t rx,uint8_t tx);

#endif

#endif