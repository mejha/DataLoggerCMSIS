/* 
 * SDcard_commands.h - SD card commands header file 
 * 
 * -----------------------------------------------------------------
 *
 * Brief description:	Contains a list of SD commands using SPI
 *                      communication and appropriate #defines
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
 * Revisions:	- (name: rev. date in a form dd.mm.yyyy
 *				  short description of the rev)
 *				  
 */

// *********************   CMD   ************************

#define CMD0    ((uint32_t)0)         // go idle state
#define CMD1    ((uint32_t)1)         // Sends host capacity support information and activates the card's initialization process.
#define ACMD41  ((uint32_t)41)        // ACMD<n> pomeni sekvenca CMD55-CMD<n> 
#define CMD8    ((uint32_t)8)         // Check Voltage range (SD card v2)
#define CMD9    ((uint32_t)9)         // Send CSD (Card Specific Data
#define CMD10   ((uint32_t)10)        // Send CID (Card ID)
#define CMD12   ((uint32_t)12)        // Stop to read data
#define CMD16   ((uint32_t)16)        // Change R/W block size
#define CMD17   ((uint32_t)17)        // Read a block
#define CMD18   ((uint32_t)18)        // Read multiple blocks
#define CMD23   ((uint32_t)23)        // MMC only: define no. of block to transfer in the next R/W command
#define ACMD23  ((uint32_t)23)        // SD card only: define no. of block to pre-erase in the next multi-block write command
#define CMD24   ((uint32_t)24)        // Wrirte a block
#define CMD25   ((uint32_t)25)        // Write multiple blocks
#define CMD55   ((uint32_t)55)        // Leading command for ACMD<n> 
#define CMD58   ((uint32_t)58)        // Read OCR (Operation conditions register)


#define GO_IDLE_STATE       CMD0
#define SEND_OP_COND        CMD1
#define APP_SEND_OP_COND    ACMD41
#define SEND_IF_COND        CMD8
#define SEND_CSD            CMD9
#define SEND_CID            CMD10
#define STOP_TRANSMISSION   CMD12
#define SET_BLOCKLEN        CMD16
#define READ_SINGLE_BLOCK   CMD17
#define READ_MULTIPLE_BLOCK CMD18
#define SET_BLOCK_COUNT     CMD23
#define SET_WR_BLOCK_ERASE_COUNT \
                            ACMD23
#define WRITE_BLOCK         CMD24
#define WRITE_MULTIPLE_BLOCK \
                            CMD25
#define APP_CMD             CMD55
#define READ_OCR            CMD58


// *********************   Response   ************************

#define R1_IDLE_STATE       ((uint8_t)0x01)
#define R1_ILLEGAL_CMD      ((uint8_t)0x04)
#define R1_CRC_ERR          ((uint8_t)0x08)
#define R1_ERASE_SEQ_ERR    ((uint8_t)0x10)
#define R1_ADDRESS_ERR      ((uint8_t)0x20)
#define R1_PARAM_ERR        ((uint8_t)0x40)


// *********************   Tokens   ************************

#define DATA_START_BLOCK     ((uint8_t)0xFE)
#define STOP_TRAN_TOKEN      ((uint8_t)0xFD)
#define WRITE_MULTIPLE_TOKEN ((uint8_t)0xFC)


// *********************   Maske   ************************

#define DATA_RES_MASK       ((uint8_t)0x1F)
#define DATA_RES_ACCEPTED   ((uint8_t)0x05)
