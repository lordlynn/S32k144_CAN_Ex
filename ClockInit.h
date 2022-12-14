#include "S32k144.h"

/***********************************************************************
 * This function initializes the external 16MHz oscillator.
 *
 * @param void
 * @return void
 **********************************************************************/
void SOSC_init_8MHz(void);


/***********************************************************************
 * This function initializes the SPLL to 160MHz
 *
 * @param void
 * @return void
 **********************************************************************/
void SPLL_init_160MHz(void);


/***********************************************************************
 * This function initializes the core clk to 80MHz
 *
 * @param void
 * @return void
 **********************************************************************/
void NormalRUNmode_80MHz(void);
