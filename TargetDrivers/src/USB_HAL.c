/* Thie File Shall Contain HW Implementation of HAL Functions */


#include "USB_hwInterface.h"

void HAL_ResetHandler(void){
	hwRegisterReset();
	hwMemoryReset();
	hwSetAddress(0u);
	hwEnable();
}
