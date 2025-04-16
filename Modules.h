/*
 * Modules.h
 *
 * Created: 29.01.2025 19:01:59
 *  Author: uwe
 */ 


#ifndef MODULES_H_
#define MODULES_H_

void MODULES_INIT (void);
void Modules_VDRANGE_SET (int8_t vdra);
void Modules_RG_SET (int8_t rgra, uint8_t chan);
void Modules_UGvRange_SET (int8_t range, int8_t chan);
void Modules_UGRange_SET (int8_t range, int8_t chan);
void Modules_RS_SET (int8_t rsr);

#define MODULES_VD_DSET 0
#define Modules_VGA1_DSET 1
#define Modules_VGA2_DSET 2
#define Modules_IS_DSET 3

#endif /* MODULES_H_ */