/* Thie File Shall Contain HW Implementation of HAL Functions */


#include "USB_hwInterface.h"

void HAL_ResetHandler(void){
	hwRegisterReset();
	hwMemoryReset();
	hwEnable();
	return;
}
void HAL_SetAddress( uint8_t devAddr ){
	hwSetAddress(devAddr);
	return;
}
