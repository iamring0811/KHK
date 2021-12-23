
void out_dac(u8 dat);
short set_balance(u16 yoff);
void set_default_RGB(void);
void read_balance(void);

void run_calibration(void);
void out_check(void);

void set_serial_data(void);
void run_fw_update(void);
void run_image_update(void);
void fnb_update(u8 cmd);
int prog_update(void);

void send_chk_ref(void);
short set_chk_ref(void);
void run_system_check(void);
void init_system_check(void);
#ifdef	_SYSADJ_C_
u16 blkRED, blkGRN, blkBLU;
u16 whtRED, whtGRN, whtBLU;
u16 dac_r, dac_g, dac_b;
u32 avrR, avrG, avrB;
short flag_balance;


#else
extern u16 blkRED, blkGRN, blkBLU;
extern u16 whtRED, whtGRN, whtBLU;
extern u16 dac_r, dac_g, dac_b;
extern short flag_balance;
extern u32 avrR, avrG, avrB;
#endif


