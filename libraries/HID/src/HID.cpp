/*
   Copyright (c) 2015, Arduino LLC
   Original code (pre-library): Copyright (c) 2011, Peter Barrett
   Modified code: Copyright (c) 2020, Aleksandr Bratchik

   Permission to use, copy, modify, and/or distribute this software for
   any purpose with or without fee is hereby granted, provided that the
   above copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
   WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
   BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
   OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
   SOFTWARE.
 */

#include "HID.h"

#if defined(USBCON)

HID_& HID()
{
	static HID_ obj;
	return obj;
}

int HID_::getInterface(uint8_t* interfaceCount)
{
	*interfaceCount += 1; // uses 1
	HIDDescriptor hidInterface = {
		D_INTERFACE(pluggedInterface, 2, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
		D_HIDREPORT(descriptorSize),
		D_ENDPOINT(USB_ENDPOINT_IN(HID_TX), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x14),
                D_ENDPOINT(USB_ENDPOINT_OUT(HID_RX), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x0A)
	};
	return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int HID_::getDescriptor(USBSetup& setup)
{
    
        u8 t = setup.wValueH;
        
        // HID-specific strings
        if(USB_STRING_DESCRIPTOR_TYPE == t) {
            
            // we place all strings in the 0xFF00-0xFFFE range
            HIDReport* rep = GetFeature(0xFF00 | setup.wValueL );
            if(rep) {
                return USB_SendStringDescriptor((u8*)rep->data, strlen_P((char*)rep->data), TRANSFER_PGM);
            }
            else {
                return 0;
            }
        }
        
	// Check if this is a HID Class Descriptor request
	if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
	if (HID_REPORT_DESCRIPTOR_TYPE != t) { return 0; }

	// In a HID Class Descriptor wIndex contains the interface number
	if (setup.wIndex != pluggedInterface) { return 0; }

	int total = 0;
	HIDSubDescriptor* node;
	for (node = rootNode; node; node = node->next) {
		int res = USB_SendControl(TRANSFER_PGM, node->data, node->length);
		if (res == -1)
			return -1;
		total += res;
	}
	
	// Reset the protocol on reenumeration. Normally the host should not assume the state of the protocol
	// due to the USB specs, but Windows and Linux just assumes its in report mode.
	protocol = HID_REPORT_PROTOCOL;
	
	return total;
}

uint8_t HID_::getShortName(char *name)
{
    if(serial) {
        byte seriallen=min(strlen_P(serial), ISERIAL_MAX_LEN);
        for(byte i=0; i< seriallen; i++) {
            name[i] = pgm_read_byte_near(serial + i);
        }
        return seriallen;
    }
    else {
        
        // default serial number
        
	name[0] = 'H';
	name[1] = 'I';
	name[2] = 'D';
	name[3] = 'A' + (descriptorSize & 0x0F);
	name[4] = 'A' + ((descriptorSize >> 4) & 0x0F);
	return 5;
    }
}

void HID_::AppendDescriptor(HIDSubDescriptor *node)
{
	if (!rootNode) {
		rootNode = node;
	} else {
		HIDSubDescriptor *current = rootNode;
		while (current->next) {
			current = current->next;
		}
		current->next = node;
	}
	descriptorSize += node->length;
}

int HID_::SetFeature(uint16_t id, const void* data, int len)
{
    if(!rootReport) {
        rootReport = new HIDReport(id, data, len);
    } else {
        HIDReport* current;
        int i=0;
        for ( current = rootReport; current; current = current->next, i++) {
            if(current->id == id) {
                return i;
            }
            // check if we are on the last report
            if(!current->next) {
                current->next = new HIDReport(id, data, len);
                break;
            }
        }

    }
    
    reportCount++;
    return reportCount;
}

int HID_::SetStringFeature(uint8_t id, const uint8_t* index, const char* data) {
    
    int res = SetFeature(id, index, 1);
    
    if(res == 0) return 0;
    
    res += SetFeature(0xFF00 | *index , data, strlen_P(data));
    
    return res;    
}

bool HID_::LockFeature(uint16_t id, bool lock) {
    if(rootReport) {
        HIDReport* current;
        for(current = rootReport;current; current=current->next) {
            if(current->id == id) {
                current->lock = lock;
                return true;
            }
        }
    }
    return false;
}


int HID_::SendReport(uint16_t id, const void* data, int len)
{
	auto ret = USB_Send(HID_TX, &id, 1);
	if (ret < 0) return ret;
	auto ret2 = USB_Send(HID_TX | TRANSFER_RELEASE, data, len);
	if (ret2 < 0) return ret2;
	return ret + ret2;
}

HIDReport* HID_::GetFeature(uint16_t id)
{
    HIDReport* current;
    int i=0;
    for(current=rootReport; current && i<reportCount; current=current->next, i++) {
        if(id == current->id) {
            return current;
        }
    }
    return (HIDReport*) NULL;
}

bool HID_::setup(USBSetup& setup)
{       
	if (pluggedInterface != setup.wIndex) {
		return false;
	}

	uint8_t request = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;
        
	if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
	{        
		if (request == HID_GET_REPORT) {

                        if(setup.wValueH == HID_REPORT_TYPE_FEATURE)
                        {

                            HIDReport* current = GetFeature(setup.wValueL);
                            if(current){ 
                                if(USB_SendControl(0, &(current->id), 1)>0 &&
                                   USB_SendControl(0, current->data, current->length)>0)
                                    return true;
                            }

                            return false;
                            
                        }    
			return true;
		}
		if (request == HID_GET_PROTOCOL) {
			// TODO: Send8(protocol);
			return true;
		}
		if (request == HID_GET_IDLE) {
			// TODO: Send8(idle);
		}
	}

	if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
	{       
		if (request == HID_SET_PROTOCOL) {
			// The USB Host tells us if we are in boot or report mode.
			// This only works with a real boot compatible device.
			protocol = setup.wValueL;
			return true;
		}
		if (request == HID_SET_IDLE) {
			idle = setup.wValueL;
			return true;
		}
		if (request == HID_SET_REPORT)
		{
                        if(setup.wValueH == HID_REPORT_TYPE_FEATURE)
                        {

                            HIDReport* current = GetFeature(setup.wValueL);
                            if(!current) return false;                              
                            if(setup.wLength != current->length + 1) return false;  
                            uint8_t* data = new uint8_t[setup.wLength];              
                            USB_RecvControl(data, setup.wLength);                   
                            if(*data != current->id) return false;                 
                            memcpy((uint8_t*)current->data, data+1, current->length); 
                            delete[] data;                                          
                            return true;
                            
                        }

		}
	}

	return false;
}

HID_::HID_(void) : PluggableUSBModule(2, 1, epType),
                   rootNode(NULL), descriptorSize(0),
                   protocol(HID_REPORT_PROTOCOL), idle(1)
{
	epType[0] = EP_TYPE_INTERRUPT_IN;
        epType[1] = EP_TYPE_INTERRUPT_OUT;
	PluggableUSB().plug(this);
}

int HID_::begin(void)
{
	return 0;
}

#endif /* if defined(USBCON) */
