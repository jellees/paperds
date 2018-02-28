#include "common.h"
#include "exception.h"
#include "Test.h"

#define	DEFAULT_DMA_NUMBER		1

void NitroStartUp()
{
	OS_Init();
	Core_PreInit();
}

static void VBlankIntr()
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}

#include <nitro/code16.h>
static void Init()
{
	TPCalibrateParam calibrate;
	OS_SetUserExceptionHandler(ExceptionHandler, NULL);
	OS_InitTick();
	OS_InitAlarm();//for frame interrupt
	OS_InitVAlarm();
	OS_InitThread();
	TP_Init();
	TP_GetUserInfo(&calibrate);
	TP_SetCalibrateParam(&calibrate);
	GX_Init();
	GX_SetDefaultDMA(2);

	GX_DispOff();
	GXS_DispOff();

	OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
	OS_EnableIrqMask(OS_IE_V_BLANK);
	OS_EnableIrq();

	FS_Init(DEFAULT_DMA_NUMBER);

	GX_VBlankIntr(true);         // to generate V-Blank interrupt request

	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	GX_DisableBankForLCDC();

	MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
	MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

	MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
	MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette

	SND_Init();

	Core_Init();
	NNS_G3dInit();
}
#include <nitro/codereset.h>

void NitroMain()
{
	Init();

	OS_WaitVBlankIntr();

	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);
	GX_DispOn();
	GXS_DispOn();

	NOCASH_Printf("Hello World!");

	Test* test = new Test();

	while (true)
	{
		test->Update();
		test->Render();

		OS_WaitVBlankIntr();
	}
}