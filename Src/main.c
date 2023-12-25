#include <main.h>

extern s_GPIO_Config pc13 ;
extern s_USART_Config usart2 ;
extern s_GPIO_Config pa8 ;
extern s_GPIO_Config pa2 ;
extern s_GPIO_Config pa3 ;

#define EP_CONTROL_NUM 0u
#define EP_CONTROL_RBSIZE 256u
#define EP_CONTROL_HWBSIZE 64u
#define EP_CONTROL_ISD 1u
#define EP_CONTROL_ADDR 0u
#define EP_CONTROL_POLLTMS 8u

#define EP_TEST_NUM 1u
#define EP_TEST_RBSIZE  256u
#define EP_TEST_HWBSIZE 64u
#define EP_TEST_ISD 0u
#define EP_TEST_ADDR 1u
#define EP_TEST_POLL_TMS 8u

#define ITF_NUM       0u
#define ITF_CLASS     1u
#define ITF_SUBCLASS  2u
#define ITF_PROTOCOL  3u
#define ITF_NUM_ALT   4u

#define CFG_NUM          0u
#define CFG_MAX_POWER    25u
#define CFG_SELF_POWER   0u
#define CFG_REMOTE_WKUP  0u


uint8_t dummy_0[12] = {0};
uint8_t dummy_1[12] = {0};
uint8_t dummy_2[12] = {0};
uint8_t dummy_3[12] = {0};

int main(void)
{

	commonPrintf("--------- STM32F103C8T6 Init-Done ---------\r\n");


	commonPrintf("--------- lwUSB - Inititialization ---------\r\n");
	lwUSB_Err err;
	Handle control_phy_h;
	Handle test_phy_h;
	Handle control_ep_h;
	Handle test_h;
	Handle itf_h;
	Handle cfg_h;

	err = lwUSB_Init();
	commonPrintf( "lwUSB_Init : %d ;\r\n",err);
	control_phy_h = lwUSB_CreatePhysicalEndPoint(EP_CONTROL_NUM, EP_CONTROL_RBSIZE, EP_CONTROL_ISD, &dummy_0[0], &dummy_1[1], EP_CONTROL_HWBSIZE);
	commonPrintf( "lwUSB_CreatePhysicalEndPoint(%d) : %d ;\r\n",EP_CONTROL_NUM,(uint32_t)control_phy_h.hc);
	test_phy_h = lwUSB_CreatePhysicalEndPoint(EP_TEST_NUM, EP_TEST_RBSIZE, EP_TEST_ISD, &dummy_2[0], &dummy_3[1], EP_TEST_HWBSIZE);

	commonPrintf( "lwUSB_CreatePhysicalEndPoint(%d) : %d ;\r\n",EP_TEST_NUM,(uint32_t)test_phy_h.hc);
	control_ep_h = lwUSB_CreateEndpoint(control_phy_h, EP_CONTROL_ADDR, e_lwUSB_EndPoint_Type_Control, e_lwUSB_EndPoint_Direction_INOUT, EP_CONTROL_HWBSIZE, EP_CONTROL_POLLTMS);
	commonPrintf( "lwUSB_CreateEndpoint(%d) : %d ;\r\n",EP_CONTROL_NUM,(uint32_t)control_ep_h.hc);
	test_h = lwUSB_CreateEndpoint(test_phy_h, EP_TEST_ADDR, e_lwUSB_EndPoint_Type_Interrupt, e_lwUSB_EndPoint_Direction_IN, EP_TEST_HWBSIZE, EP_TEST_POLL_TMS);
	commonPrintf( "lwUSB_CreateEndpoint(%d) : %d ;\r\n",EP_TEST_NUM,(uint32_t)test_h.hc);
	itf_h = lwUSB_CreateInterface(ITF_NUM, ITF_CLASS, ITF_SUBCLASS, ITF_PROTOCOL, ITF_NUM_ALT);
	commonPrintf( "lwUSB_CreateInterface(%d) : %d ;\r\n",ITF_NUM,(uint32_t)itf_h.hc);
	cfg_h = lwUSB_CreateConfiguration(CFG_NUM, CFG_MAX_POWER, CFG_SELF_POWER, CFG_REMOTE_WKUP);
	commonPrintf( "lwUSB_CreateConfiguration(%d) : %d ;\r\n",CFG_NUM,(uint32_t)cfg_h.hc);

	err = lwUSB_RegisterEndPoint(control_ep_h, itf_h);
	commonPrintf( "lwUSB_RegisterEndPoint : %d ;\r\n",err);
	err = lwUSB_RegisterEndPoint(test_h, itf_h);
	commonPrintf( "lwUSB_RegisterEndPoint : %d ;\r\n",err);
	err = lwUSB_RegisterInterface(itf_h, cfg_h);
	commonPrintf( "lwUSB_RegisterInterface: %d ;\r\n",err);
	err = lwUSB_RegisterConfiguration(cfg_h);
	commonPrintf( "lwUSB_RegisterConfiguration : %d ;\r\n",err);

	commonPrintf("--------- lwUSB - SuperLoop ---------\r\n");

	while(1){

		lwUSB_Main();

	}
	commonPrintf("---THIS SHOULD NEVER BE PRINTED---\r\n");
	return 1u;
}
