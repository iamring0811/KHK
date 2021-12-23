
void init_RAM(void);

#define item_ram_size		21

#ifdef _PARA_H_

u8 ar_item_ram[item_ram_size][10][11];
u8 co_item_ram[item_ram_size][10][11];
u8 si_item_ram[item_ram_size][10][11];
const u8 *ar_printform[20][10];
const u8 *co_printform[20][10];
const u8 *si_printform[20][10];
const u8 ini_strip[]; 					//264=24*11
const u8 ini_position[];
const u8 ini_weight[20];
const u8 ini_para[20*41];

#else

extern u8 ar_item_ram[item_ram_size][10][11];
extern u8 co_item_ram[item_ram_size][10][11];
extern u8 si_item_ram[item_ram_size][10][11];
extern const u8 *ar_printform[20][10];
extern const u8 *co_printform[20][10];
extern const u8 *si_printform[20][10];
extern const u8 ini_strip[]; 			//264=24*11
extern const u8 ini_position[];
extern const u8 ini_weight[20];
extern const u8 ini_para[20*41];

#endif

