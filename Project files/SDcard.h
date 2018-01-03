/* 
 * SDcard.h - SD card header file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains SD card functions
 *
 * -----------------------------------------------------------------
 *
 * Author: 				Aljaz Prislan
 * 
 * Date of creation:	10.4.2017
 * 
 * Date of 1st release: 
 * 
 * -----------------------------------------------------------------
 *
 * Revisions:	- 
 *				  
 *				  
 */
 
 #include "stm32f10x.h"
 #include "SDcard_commands.h"
 
 #ifndef _SDCARD_H
 #define _SDCARD_H
 
 typedef struct CID {
	// byte 0
	uint8_t mid;  // Manufacturer ID
	// byte 1-2
	char oid[2];  // OEM/Application ID
	// byte 3-7
	char pnm[5];  // Product name
	// byte 8
	unsigned prv_m :4;  // Product revision n.m
	unsigned prv_n :4;
	// byte 9-12
	uint32_t psn;  // Product serial number
	// byte 13
	unsigned mdt_year_high :4;  // Manufacturing date
	unsigned reserved :4;
	// byte 14
	unsigned mdt_month :4;
	unsigned mdt_year_low :4;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} cid_t;
 
// CSD v karticah v1
typedef struct CSDV1 {
	// byte 0
	unsigned reserved1 :6;
	unsigned csd_ver :2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len :4;
	unsigned ccc_low :4;
	// byte 6
	unsigned c_size_high :2;
	unsigned reserved2 :2;
	unsigned dsr_imp :1;
	unsigned read_blk_misalign :1;
	unsigned write_blk_misalign :1;
	unsigned read_bl_partial :1;
	// byte 7
	uint8_t c_size_mid;
	// byte 8
	unsigned vdd_r_curr_max :3;
	unsigned vdd_r_curr_min :3;
	unsigned c_size_low :2;
	// byte 9
	unsigned c_size_mult_high :2;
	unsigned vdd_w_cur_max :3;
	unsigned vdd_w_curr_min :3;
	// byte 10
	unsigned sector_size_high :6;
	unsigned erase_blk_en :1;
	unsigned c_size_mult_low :1;
	// byte 11
	unsigned wp_grp_size :7;
	unsigned sector_size_low :1;
	// byte 12
	unsigned write_bl_len_high :2;
	unsigned r2w_factor :3;
	unsigned reserved3 :2;
	unsigned wp_grp_enable :1;
	// byte 13
	unsigned reserved4 :5;
	unsigned write_partial :1;
	unsigned write_bl_len_low :2;
	// byte 14
	unsigned reserved5 :2;
	unsigned file_format :2;
	unsigned tmp_write_protect :1;
	unsigned perm_write_protect :1;
	unsigned copy :1;
	unsigned file_format_grp :1;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} csd1_t;

// CSD v karticah v2
typedef struct CSDV2 {
	// byte 0
	unsigned reserved1 :6;
	unsigned csd_ver :2;
	// byte 1
	uint8_t taac;
	// byte 2
	uint8_t nsac;
	// byte 3
	uint8_t tran_speed;
	// byte 4
	uint8_t ccc_high;
	// byte 5
	unsigned read_bl_len :4;
	unsigned ccc_low :4;
	// byte 6
	unsigned reserved2 :4;
	unsigned dsr_imp :1;
	unsigned read_blk_misalign :1;
	unsigned write_blk_misalign :1;
	unsigned read_bl_partial :1;
	// byte 7
	unsigned reserved3 :2;
	unsigned c_size_high :6;
	// byte 8
	uint8_t c_size_mid;
	// byte 9
	uint8_t c_size_low;
	// byte 10
	unsigned sector_size_high :6;
	unsigned erase_blk_en :1;
	unsigned reserved4 :1;
	// byte 11
	unsigned wp_grp_size :7;
	unsigned sector_size_low :1;
	// byte 12
	unsigned write_bl_len_high :2;
	unsigned r2w_factor :3;
	unsigned reserved5 :2;
	unsigned wp_grp_enable :1;
	// byte 13
	unsigned reserved6 :5;
	unsigned write_partial :1;
	unsigned write_bl_len_low :2;
	// byte 14
	unsigned reserved7 :2;
	unsigned file_format :2;
	unsigned tmp_write_protect :1;
	unsigned perm_write_protect :1;
	unsigned copy :1;
	unsigned file_format_grp :1;
	// byte 15
	unsigned always1 :1;
	unsigned crc :7;
} csd2_t;

// unija CSD registra za staro in novo verzijo kartic
union csd_t {
	csd1_t v1;
	csd2_t v2;
};
 
 uint32_t SD_getSize(void);
 uint8_t  SD_readReg(uint8_t cmd, void *buf);
 uint8_t  SD_readBlock(uint32_t address, uint8_t *buffer);
 uint8_t  SD_writeBlock(uint32_t address, uint8_t *buffer);
 void     SD_waitUntilReady(void);
 uint8_t  SD_init(void);
 uint8_t  SD_cardCommand(uint8_t cmd, uint32_t arg);
 void     SD_selectCard(void);
 void     SD_deselectCard(void);
 void     SD_waitUntilReady(void);

#endif
 