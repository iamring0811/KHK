/* Define to prevent recursive inclusion -------------------------------------*/

#include "USB\USB.h"

#define BMP_UPDATE			'P'					// P00
#define FONT_UPDATE			'F'					// F00
#define BNF_UPDATE			'A'					// P00+F00
#define PROG_UPDATE			'V'					// V00

#define NVIC_DATE			0x1C				// 4B
#define NVIC_NAME			0x20				// 8B
#define NVIC_VER			0x28				// 4B
#define NVIC_SIZE			0x34				// 4B


int USBFatReady(void);

int FirmwareUSB(void);
u16 ExploreUSB(u8 cmd);
int ReadFileFromUSB(char* filename);
int WriteFileToUSB(char* filename, u32 bytesToWrite );

#ifdef __USB_FILE_H
u8	file_count;

#else
extern u8	file_count;

#endif

