#define __USB_FILE_H

#include <string.h>
#include "USB\usb_file.h"
#include "util\usart.h"
#include "util\flash.h"
#include "util\lcd.h"

// USB 파일 시스템
FATFS fatfs;
FIL file;
FRESULT res;

// 파일 버퍼
const uint16_t READ_BUFFER_SIZE =4096*4;  //16k
const uint16_t WRITE_BUFFER_SIZE = 1024;  //1k

uint8_t usb_read_buffer[READ_BUFFER_SIZE]  = {0};
uint8_t usb_write_buffer[WRITE_BUFFER_SIZE]  = {0};

// USB 핸들러
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern uint8_t USBHS_EnumDone;

void usb_store_pf(void);			// 그림 및  폰트 파일 저장
void usb_store_program(void);			// 실행 파일 저장

extern void store_bmp(u8 bar, u8 sel);
extern void store_font(u8 bar);
extern void delay_ms(u32 dly);

/**********************************************************************************************/
int FirmwareUSB(void)
{
	FILINFO fno;
  	DIR dir;
  	char *fn;
	char* path = "0:/DFI600";	// ==> 폴더변경,	char* path = "0:/"; ==> 루터
	char lfn[32];
	int filesize=0;				// 디렉토리명과 파일명을 담을 새로운 변수
	
	file_count=0;
	res = f_opendir(&dir,path);
	if (res == FR_OK) {
   		UsbPrint("\r\n>USB HS:Firmware: Directory scanned = %s", path);
		while(!file_count) {
//	    	UsbPrint("\r\n>USB HS:Explore: Directory opened = %s", path);
	    	res = f_readdir(&dir, &fno);
    		if (res != FR_OK || fno.fname[0] == 0) break;
    		if (fno.fname[0] == '.') continue;

	      	fn = fno.fname;
			sprintf(lfn,"%s%s%s",path,"/",fn);			// 디렉토리명과 파일명 조합, 중간에 / 넣기

    	  	if (fno.fattrib & AM_DIR){
        		continue;
	      	}else{	
	        	if(strstr(fn, "DF6") !=NULL){			// 특정파일명을 가졌을 경우
					if((fn[9]=='b')||(fn[9]=='B')){
						flag_upload = 0x04;
						file_count++;
						UsbPrint("\r>USB HS:Firmware: found filename = %s", fn);
						// 해당 파일을 읽고
						filesize= ReadFileFromUSB(lfn);
						printf0("\t file size:%d",filesize);
//						for(filesize=0;filesize<0x200;filesize++){ printf0(" %2x", *(__IO uint8_t*)(USB_BUFF+filesize)); delay_ms(50); }
					}
        		}
	      	}
    	} 
	}
	else
	{
		LCD_ErrLog("\r\n>USB HS:Explore: Cannot open root directory %d .\r",res);
	}
	return(filesize);
}
/**********************************************************************************************/
/**********************************************************************************************/

int USBFatReady(void)
{
	if( USBH_USR_ApplicationState == USH_USR_FS_FAT_INIT) return 1;
	return 0;
}

u16 ExploreUSB(u8 cmd)
{
	FILINFO fno;
  	DIR dir;
  	char *fn;
	char* path = "0:/DFI600";	// ==> 폴더변경,	char* path = "0:/"; ==> 루터
	char lfn[32];
	int filesize=0;				// 디렉토리명과 파일명을 담을 새로운 변수
	u32	i;
	u16 cmd2; 
	
	if(cmd==BNF_UPDATE){
		cmd = FONT_UPDATE;
		cmd2 = BMP_UPDATE;
	}else cmd2 = 0xFFFF;
	file_count=0;
	res = f_opendir(&dir,path);
	if (res == FR_OK) {
   		UsbPrint("\r\n>USB HS:Explore: Directory scanned = %s", path);
		for (;;) {
//	    	UsbPrint("\r\n>USB HS:Explore: Directory opened = %s", path);
	    	res = f_readdir(&dir, &fno);
    		if (res != FR_OK || fno.fname[0] == 0) break;
//    		UsbPrint("\r\n>USB HS:Explore: Directory scanned = %s", path);
//			UsbPrint(" <f.f.%s> ",fno.fname);
    		if (fno.fname[0] == '.') continue;

	      	fn = fno.fname;
			sprintf(lfn,"%s%s%s",path,"/",fn);			// 디렉토리명과 파일명 조합, 중간에 / 넣기

    	  	if (fno.fattrib & AM_DIR){
        		continue;
	      	}else{	
	        	if(strstr(fn, "DFI") !=NULL){			// 특정파일명을 가졌을 경우
//	        	if(strext(fn, cmd) !=NG){				// 특정확장자를 가졌을 경우
					if((fn[9]==cmd)||(fn[9]==cmd2)){
						file_count++;
						UsbPrint("\r>USB HS:Explore: found filename = %2d:%s",file_count, fn);
						if(file_count>=64) break;
//	if(file_count>=5) break;
						if(fn[9]==BMP_UPDATE) flag_upload |= 0x01;
						if(fn[9]==FONT_UPDATE) flag_upload |= 0x02;
						i = file_count - 1;
						msg24(100+((i%3)*210),90+((i/3)*26),BLK_LCD,"%s", fn);
						// 해당 파일을 읽고
						filesize= ReadFileFromUSB(lfn);
						printf0("\n file size:%d",filesize);
						for(i=0;i<0x20;i++) rcv_buff[i] = *(__IO uint8_t*)(USB_BUFF+i);
						for(i=0x20;i<filesize;i++) *(u8*)(BMP_BUFF+i-0x20) = *(__IO uint8_t*)(USB_BUFF+i);
						store_bmp_usb();
						// 파일 저장
//						if((cmd=='P')||(cmd=='F')) usb_store_pf();		// 그림 및 폰트 파일 저장
//						else if(cmd=='B') usb_store_program();			// 실행 파일 저장
						// 파일 확장자를 BAK로 해서 저장 

//						strncpy(writefn,fn, strlen(fn)-4);
//						strncpy(&writefn[strlen(fn)-4],".BAK",4);
//						writefn[strlen(fn)]=0;
						// 파일 확장자를 BBB로 해서 저장 
//						strncpy(writefn,lfn, strlen(lfn)-4);
//						strncpy(&writefn[strlen(lfn)-4],".BCK",4);
//						strncpy(writefn,lfn, strlen(lfn));
//						writefn[strlen(lfn)-3] = 'X';
//						writefn[strlen(lfn)]=0;
						// 파일 저장
//						WriteFileToUSB(writefn,filesize);
					}
        		}
	      	}
    	} 
	}
	else
	{
		LCD_ErrLog("\r\n>USB HS:Explore: Cannot open root directory %d .\r",res);
	}
	//f_mount(NULL, "", 0);	
	return(file_count);
}
/**********************************************************************************************
void usb_store_pf(void){			// 그림 및 폰트 파일 저장
u8 b1;
	rcv_step=0;
	for(b1=0;b1<0x20;b1++)	rcv_buff[b1] = *(u8*)(USB_BUFF+b1);
	if((rcv_buff[1]=='D')&&(rcv_buff[2]=='F')&&(rcv_buff[3]=='4')&&(rcv_buff[6]=='.')){
		rcv_size = rcv_buff[14];
		rcv_size = (rcv_size*0x100) + rcv_buff[13];
		rcv_size = (rcv_size*0x100) + rcv_buff[12];
		printf0(" [Size:%d]", rcv_size); 
		if(rcv_size>MAX_BLOCK){
			rcv_size = MAX_BLOCK;
			printf0(" **** Size Over ****"); 
		}
		rcv_sum = rcv_buff[19];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[18];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[17];
		rcv_sum = (rcv_sum*0x100) + rcv_buff[16];
		get_sum=0;
		for(rcvwp=0;rcvwp<rcv_size;rcvwp++){
			b1 = *(u8*)(USB_BUFF+rcvwp+0x20);
			*(u8*)(BMP_BUFF+rcvwp) = b1;
			get_sum += b1;
		}
		printf0("\r RS:%x-GS:%x", rcv_sum, get_sum);
		if(rcv_sum==get_sum){
			if(rcv_buff[10]==2){
				store_bmp(0);
			}else if(rcv_buff[10]==4){
				printf0(" -%d-", rcv_buff[11]);
				store_font(0);
			}else printf0("  *** Known File ***");
		}else{
			printf0("\r *** Error SUM ***   RS:%x-GS:%x", rcv_sum, get_sum);
		}
		rcv_wait = rcv_step = 0;
	}
}
**********************************************************************************************/
void usb_store_program(void){			// 실행 파일 저장
}
/**********************************************************************************************/

//#define DEBUG_USB_READ
int ReadFileFromUSB(char* filename )
{
	
	u16 i, j=0;
	u32 numOfReadBytes=0;
	u32 numOfTotalRead=0;
#ifdef DEBUG_USB_READ	
	u32 numOfFilesize=0;
#endif

	// DRAM 포인터를 0 으로 초기화
	usbpnt=0;
	
	res = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)	
	{ 
		f_close(&file);
		//f_mount(NULL, "", 0);
		LCD_ErrLog("\r\n>USB HS:ReadFileFromUSB: %s not exist, res code=%d \r",filename,res);
		return -1;
	}
#ifdef DEBUG_USB_READ	
	else numOfFilesize = f_size(&file);
	UsbPrint("\r\n>USB HS:ReadFileFromUSB: filename=%s\t filesize=%d", filename,numOfFilesize);
#endif
	// USB가 연결되어 있는 상태에서 읽어들인다.
	while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
	{
		res = f_read(&file, usb_read_buffer, READ_BUFFER_SIZE, &numOfReadBytes);
		if((numOfReadBytes == 0) || (res != FR_OK)) /*EOF or Error*/			
	    {
    	  break; 
	    }
		else
		{
			//////////////////////////////////////////////////////////////////////////////////////////
			//========== 여기 아래에서 버퍼에 쌓인 내용 처리
			numOfTotalRead = numOfTotalRead + numOfReadBytes;
			if(numOfReadBytes<16384){
#ifdef DEBUG_USB_READ	
				UsbPrint("\r\n>\tReaded=%d\tTotal=%d", numOfReadBytes,numOfTotalRead);
#endif
			}else if((++j)>=4){
#ifdef DEBUG_USB_READ	
				UsbPrint("\r\n>\tReaded=%d\tTotal=%d", numOfReadBytes*j,numOfTotalRead);
#endif
				j=0;
			}
			USB_OTG_BSP_mDelay(1);
			
			// 읽은 양 만큼 DRAM 으로 옮긴다.
			for(i=0;i<numOfReadBytes;i++){
				*(__IO uint8_t*)(USB_BUFF+usbpnt) = usb_read_buffer[i];	
				usbpnt++;
			}
#ifdef DEBUG_USB_READ	
			if(!j) UsbPrint("\t usbpnt=%d",usbpnt);
#endif
			///////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	f_close(&file);
	//f_mount(NULL, "", 0);
	
	//UsbPrint("\r\n>USB HS:ReadFileFromUSB: name=%s\tTotal=%d\t", filename,numOfTotalRead);
	
	for(i=0;i<50;i++)
	{
//		USB_Process();
		USB_HS_Process();
		USB_OTG_BSP_mDelay (1); 
	}
	
	// 읽은 바이트 수를 반환한다. 이 값은 usbpnt 값보다 1 크다
	return numOfTotalRead;
}

int WriteFileToUSB(char* filename, u32 bytesToWrite )
{
	u32 bytesWritten=0;
	u32 bytesWrittens=0;
	u32 remainBytesToWrite;

	int ret=0;
	u16 i, j=0;
		
	if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
	{
		LCD_ErrLog ("\r\n>USB HS:WriteFileToUSB: Disk is write protected \r");
		return -1;
	}


	if(f_open(&file, filename,FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{ 
		UsbPrint("\r\n>USB HS:WriteFileToUSB: filename=%s\t filesize=%d", filename,bytesToWrite);
		
		// USB가 연결되어 있는 상태에서 쓴다.
		while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
		{
			// 남은 양 계산
			remainBytesToWrite = bytesToWrite - bytesWrittens;
			if (remainBytesToWrite ==0 ) break;
			
			///////////////////////////////////////////////////////////
			// DRAM 버퍼에서 쓰기로 버퍼로 읽어와서 저장 
			if (remainBytesToWrite<=WRITE_BUFFER_SIZE)
			{
				for(i=0;i< remainBytesToWrite;i++)
				{
					usb_write_buffer[i] = *(__IO uint8_t*)(USB_BUFF+bytesWrittens+i);	
				}
			}
			else
			{
				for(i=0;i<WRITE_BUFFER_SIZE;i++)
				{
					usb_write_buffer[i] = *(__IO uint8_t*)(USB_BUFF+bytesWrittens+i);	
				}
			}
			
			////////////////////////////////////////////////////////////////
			/* Write buffer to file */
			res= f_write (&file, usb_write_buffer, i, (void *)&bytesWritten);   
			
			
			if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
			{
				ret =-1;
				LCD_ErrLog("\r\n>USB HS:WriteFileToUSB:  filename %s CANNOT be writen. (write error)\r",filename );
				break;
			}
			else 
			{
				bytesWrittens+=bytesWritten;
				if(bytesWritten<1024){
					UsbPrint("\r\n>\tWritten=%d\tTotal=%d",(j*1024)+bytesWritten,bytesWrittens);
				}else if((++j)>=64){
					UsbPrint("\r\n>\tWritten=%d\tTotal=%d",bytesWritten*j,bytesWrittens);
					j=0;
				}
			}
		}
	}
	else 
	{
		ret =-1;
		LCD_ErrLog("\r\n>USB HS:WriteFileToUSB:  filename  %s CANNOT be writen. (open error)\r",filename );
	}

	f_close(&file);
	//f_mount(NULL, "", 0);
	
	for(i=0;i<50;i++)
	{
//		USB_Process();
		USB_HS_Process();
		USB_OTG_BSP_mDelay (1); 
	}
	
	return ret;
}
