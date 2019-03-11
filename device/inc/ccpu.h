#ifndef CCPU_CONF_H
#define CCPU_CONF_H
#include <stdint.h>
#include "device_conf.h"

#define     CCPU_DATA_T                 uint16_t
#define     CCPU_ID                     ((CCPU_DATA_T)0xff01U)

#define     CCPU_BASE_ADDRESS                   (0x60080000)

#define     CCPU_READ_REG(reg)      (*((CCPU_DATA_T *)(CCPU_BASE_ADDRESS + reg)))
#define     CCPU_WRITE_REG(reg, value)  (*((CCPU_DATA_T *)(CCPU_BASE_ADDRESS + reg)) = (CCPU_DATA_T)value)

#define     CCPU_WRITE_PSRAM_LAT                (0xe0U)

#define     CCPU_READ_CONTROL_REGISTER          (0x0U)
#define     CCPU_READ_ID_REGISTER               (0x2U)
#define     CCPU_READ_SPI_CONTROL_REGISTER      (0x4U)
#define     CCPU_READ_SPI_STATUS_REGISTER       (0x6U)
#define     CCPU_READ_SPI_REGISTER              (0x8U)
#define     CCPU_READ_RENDER_STAT			    (0x20U)
#define     CCPU_READ_GPU_STAT				    (0x24U)

#define     CCPU_READ_DBG					    (0x40U)
#define     CCPU_READ_DBG1					    (0x42U)

#define     CCPU_WRITE_CONTROL_REGISTER         (0x0U)
#define     CCPU_WRITE_SPI_CONTROL_REGISTER     (0x4U)
#define     CCPU_WRITE_SPI_DATA_REGISTER        (0x6U)
#define     CCPU_WRITE_TFT_RESET_REGISTER       (0x8U)
#define     CCPU_WRITE_RENDER_CTL_REGISTER      (0x20U)
#define     CCPU_WRITE_GPU_CTL_REGISTER  	    (0x22U)
#define     CCPU_WRITE_GPU_IRQ_REGISTER  	    (0x24U)






#define		CCPU_WRITE_GPU_COLOR	            (0x60U)
#define		CCPU_WRITE_GPU_DEST_X	            (0x62U)
#define		CCPU_WRITE_GPU_DEST_Y	            (0x64U)
#define		CCPU_WRITE_GPU_DEST_W	            (0x66U)
#define		CCPU_WRITE_GPU_DEST_H	            (0x68U)
#define		CCPU_WRITE_GPU_DEST_LEG	            (0x6aU)
#define		CCPU_WRITE_GPU_SRC_X	            (0x6cU)
#define		CCPU_WRITE_GPU_SRC_Y	            (0x6eU)
#define		CCPU_WRITE_GPU_SRC_W	            (0x70U)
#define		CCPU_WRITE_GPU_SRC_H	            (0x72U)
#define		CCPU_WRITE_GPU_SRC_LEG	            (0x74U)
#define		CCPU_WRITE_GPU_SCALE	            (0x76U)
#define		CCPU_WRITE_GPU_SCALEX	            (0x80U)
#define		CCPU_WRITE_GPU_SCALEY	            (0x82U)
#define		CCPU_WRITE_GPU_DPL		            (0x78U)
#define		CCPU_WRITE_GPU_DPH		            (0x7aU)
#define		CCPU_WRITE_GPU_SPL		            (0x7cU)
#define		CCPU_WRITE_GPU_SPH		            (0x7eU)

#define		CCPU_WRITE_COLOR_MODE		        (0x100U)

#define		CCPU_WRITE_FILL_CTL		            (0xa0U)
#define	    CCPU_READ_FILL_STAT		            (0xa0U)

#define		CCPU_WRITE_PFILL_CTL		        (0xaaU)

#define		CCPU_WRITE_COPY_CTL		            (0xc0U)
#define	    CCPU_READ_COPY_STAT		            (0xc0U)

#define 	CCPU_READ_BUSY					    (0x40U)
#define 	CCPU_READ_IRQS					    (0x42U)

#define		CCPU_WRITE_IRQ_CLEAR			    (0x42U)

#define     CCPU_READ_AMPL_CY                   (0x200U)




#define     CCPU_PSRAM_ADLAT_GP         (0U)
#define     CCPU_PSRAM_ADLAT_GM         (0xfU << CCPU_PSRAM_ADLAT_GP)

#define     CCPU_PSRAM_DLAT_GP          (4U)
#define     CCPU_PSRAM_DLAT_GM          (0xfU << CCPU_PSRAM_DLAT_GP)

#define     CCPU_PSRAM_HOLD_GP          (8U)
#define     CCPU_PSRAM_HOLD_GM          (0xfU << CCPU_PSRAM_HOLD_GP)

/*pll*/
/*write bits*/
#define     CCPU_PLL_RESET_BP           (0)
#define     CCPU_PLL_RESET_BM           (1 << CCPU_PLL_RESET_BP)

#define     CCPU_PLL_ENABLE_BP          (1)
#define     CCPU_PLL_ENABLE_BM          (1 << CCPU_PLL_ENABLE_BP)
/*write bits*/

/*read bits*/
#define     CCPU_PLL_LOCKED_BP          (2)
#define     CCPU_PLL_LOCKED_BM          (1 << CCPU_PLL_LOCKED_BP)
/*read bits*/
/*pll*/

/*spi definitions*/
/*write bits*/

#define     CCPU_SPI_CPOL_BP     (0x0U)/*edge polarity to latch the data by spi controller, "0" = rising, "1" - falling*/
#define     CCPU_SPI_CPOL_BM     (1 << CCPU_SPI_CPOL_BP)

#define     CCPU_SPI_CPHA_BP     (0x01U)/*start phase, "0" - first phase, "1" - second phase*/
#define     CCPU_SPI_CPHA_BM     (1 << CCPU_SPI_CPHA_BP) 

#define     CCPU_SPI_DIR_BP      (0x2U)/*"0" - MSB, "1" = LSB*/
#define     CCPU_SPI_DIR_BM      (1 << CCPU_SPI_DIR_BP)

#define     CCPU_SPI_PRESC_GP    (0x3U)/*clock prescaler for spi controller domain*/
#define     CCPU_SPI_PRESC_BM    (0x1fU << CCPU_SPI_PRESC_GP)

#define     CCPU_SPI_COUNT_GP    (0x8U)/*bit count that spi controller will send, valid values are - 2 to 16*/
#define     CCPU_SPI_COUNT_BM    (0xfU << CCPU_SPI_COUNT_GP)

#define     CCPU_SPI_RESET_BP    (0xcU)
#define     CCPU_SPI_RESET_BM    (1 << CCPU_SPI_RESET_BP)

/*write bits*/

/*read bits*/

#define     CCPU_SPI_BUSY_BP     (0x0U)/*busy flag of spi controller - "1" - busy, "0" - ready*/
#define     CCPU_SPI_BUSY_BM     (1 << CCPU_SPI_BUSY_BP)

#define     CCPU_SPI_ERROR_GP    (0x1U)
#define     CCPU_SPI_ERROR_GM    (7U << CCPU_SPI_ERROR_GP)

/*read bits*/
/*spi definitions*/

/*tft*/
/*write bits*/

#define     CCPU_TFT_RESET_BP    (0U)
#define     CCPU_TFT_RESET_BM    (1 << CCPU_TFT_RESET_BP)

/*write bits*/

/*read bits*/

/*read bits*/
/*tft*/

/*GPU & RENDER*/
/*write bits*/

#define     CCPU_GPU_RESET_BP    	    (0U)
#define     CCPU_GPU_RESET_BM    	    (1 << CCPU_GPU_RESET_BP)

#define     CCPU_RND_RESET_BP    	    (0U)
#define     CCPU_RND_RESET_BM    	    (1 << CCPU_RND_RESET_BP)

#define     CCPU_RND_UPDATE_BP    	    (1U)
#define     CCPU_RND_UPDATE_BM    	    (1 << CCPU_RND_UPDATE_BP)

#define     CCPU_FILL_RESET_BP    	    (0U)
#define     CCPU_FILL_RESET_BM    	    (1 << CCPU_FILL_RESET_BP)

#define     CCPU_FILL_UPDATE_BP    	    (1U)
#define     CCPU_FILL_UPDATE_BM    	    (1 << CCPU_FILL_UPDATE_BP)

#define     CCPU_COPY_RESET_BP    	    (0U)
#define     CCPU_COPY_RESET_BM    	    (1 << CCPU_COPY_RESET_BP)

#define     CCPU_COPY_UPDATE_BP    	    (1U)
#define     CCPU_COPY_UPDATE_BM    	    (1 << CCPU_COPY_UPDATE_BP)

#define     CCPU_COPY_SRC_BP    	    (2U)
#define     CCPU_COPY_SRC_BM    	    (1 << CCPU_COPY_SRC_BP)

#define     CCPU_COPY_PPU_RESET_BP    	(3U)
#define     CCPU_COPY_PPU_RESET_BM    	(1 << CCPU_COPY_PPU_RESET_BP)

#define     CCPU_RND_IRQ_CLEAR_BP    	(0U)
#define     CCPU_RND_IRQ_CLEAR_BM    	(1 << CCPU_RND_IRQ_CLEAR_BP)

#define     CCPU_FILL_IRQ_CLEAR_BP    	(1U)
#define     FILL_IRQ_CLEAR_BM    	    (1 << CCPU_FILL_IRQ_CLEAR_BP)

#define     CCPU_COPY_IRQ_CLEAR_BP    	(0x2U)
#define     CCPU_COPY_IRQ_CLEAR_BM    	(1 << CCPU_COPY_IRQ_CLEAR_BP)

#define     CCPU_COPY_X_MIRROR_BP       (14U)
#define     CCPU_COPY_X_MIRROR_BM       (1 << CCPU_COPY_X_MIRROR_BP)
#define     CCPU_COPY_Y_MIRROR_BP       (15U)
#define     CCPU_COPY_Y_MIRROR_BM       (1 << CCPU_COPY_Y_MIRROR_BP)
#define     CCPU_COPY_SWAP_BP    		(13U)
#define     CCPU_COPY_SWAP_BM    		(1 << CCPU_COPY_SWAP_BP)
#define     CCPU_COPY_INTERLACE_X_BP    (12U)
#define     CCPU_COPY_INTERLACE_X_BM    (1 << CCPU_COPY_INTERLACE_X_BP)
#define     CCPU_COPY_INTERLACE_Y_BP    (11U)
#define     CCPU_COPY_INTERLACE_Y_BM    (1 << CCPU_COPY_INTERLACE_Y_BP)

/*write bits*/
						
#define     CCPU_RND_BUSY_BP    	(1U)
#define     CCPU_RND_BUSY_BM    	(1 << CCPU_RND_BUSY_BP)

#define     CCPU_FILL_BUSY_BP    	(1U)
#define     CCPU_FILL_BUSY_BM    	(1 << CCPU_FILL_BUSY_BP)

#define     CCPU_COPY_BUSY_BP    	(1U)
#define     CCPU_COPY_BUSY_BM    	(1 << CCPU_COPY_BUSY_BP)

#define     CCPU_RND_IRQ_BP    	    (0U)
#define     CCPU_RND_IRQ_BM    	    (1 << CCPU_RND_IRQ_BP)

#define     CCPU_FILL_IRQ_BP    	(1U)
#define     CCPU_FILL_IRQ_BM    	(1 << CCPU_FILL_IRQ_BP)

#define     CCPU_COPY_IRQ_BP    	(2U)
#define     CCPU_COPY_IRQ_BM    	(1 << CCPU_COPY_IRQ_BP)


						
/*read bits*/

#define     CCPU_QVGA_BUSY_BP    (0U)
#define     CCPU_QVGA_BUSY_BM    (1 << CCPU_QVGA_BUSY_BP)

/*read bits*/
/*GPU & RENDER*/

#endif /*CCPU_CONF_H*/

