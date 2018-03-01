#include "common.h"
#include "player/Player.h"
#include "Test.h"



Test::Test()
{
	GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
	MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
	(void)GX_DisableBankForLCDC();

	G3X_InitMtxStack();

	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);

	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
	GX_SetVisiblePlane(GX_PLANEMASK_BG0);
	GXS_SetGraphicsMode(GX_BGMODE_0);

	G3X_SetShading(GX_SHADING_TOON);
	G3X_AntiAlias(TRUE);
	G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);

	G3X_AlphaTest(FALSE, 0);                   // AlphaTest OFF
	G3X_AlphaBlend(TRUE);                      // AlphaTest ON

	G3X_SetClearColor(GX_RGB(119 >> 3, 199 >> 3, 244 >> 3), 0, 0x7fff, 63, FALSE);
	G3_ViewPort(0, 0, 255, 191);
	GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

	NNS_GfdResetLnkTexVramState();
	NNS_GfdResetLnkPlttVramState();

	// Open model.
	_modelResource = (NNSG3dResFileHeader*)OpenFile("/data/summersky.nsbmd");
	NNS_G3dResDefaultSetup(_modelResource);
	NNSG3dResFileHeader* resourceTextures = (NNSG3dResFileHeader*)OpenFile("/data/summersky.nsbtx");
	NNS_G3dResDefaultSetup(resourceTextures);
	NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(_modelResource), NNS_G3dGetTex(resourceTextures));
	NNS_G3dRenderObjInit(&_modelRender, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(_modelResource), 0));
	NNS_FndFreeToExpHeap(gHeapHandle, resourceTextures);

	// Set camera matrix.
	VEC_Set(&_up, 0, FX32_ONE, 0);
	VEC_Set(&_position, FX32_CONST(36), FX32_CONST(36), 0);
	VEC_Set(&_at, 0, 0, 0);

	// Create the player object as mario.
	_mario = new Player();
}


void* Test::OpenFile(char* path)
{
	FSFile file;
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	uint32_t mSize = FS_GetLength(&file);
	void* buffer = NNS_FndAllocFromExpHeapEx(gHeapHandle, mSize, 32);
	if (buffer != NULL)
		FS_ReadFile(&file, buffer, (int)mSize);
	FS_CloseFile(&file);
	DC_FlushRange(buffer, mSize);
	return buffer;
}


Test::~Test()
{
	NNS_FndFreeToExpHeap(gHeapHandle, _modelResource);
}


void Test::Update()
{
	uint16_t gKeys = PAD_Read();
	if (gKeys & PAD_BUTTON_A)
		_position.x += FX32_CONST(4);
	else if (gKeys & PAD_BUTTON_B)
		_position.x -= FX32_CONST(4);

	_mario->Update();
}


void Test::Render()
{
	MtxFx44 mtx;
	MTX_PerspectiveW(FX32_SIN30, FX32_COS30, 256 * 4096 / 192, FX32_CONST(1.5), FX32_CONST(2000), 1024, &mtx);
	NNS_G3dGlbSetProjectionMtx(&mtx);


	NNS_G3dGlbLightVector(GX_LIGHTID_0, 0, GX_FX32_FX10_MAX, 0);
	NNS_G3dGlbLightColor(GX_LIGHTID_0, GX_RGB(0xFF, 0xFF, 0xFF));
	NNS_G3dGlbLightVector(GX_LIGHTID_3, GX_FX32_FX10_MAX, 0, GX_FX32_FX10_MAX);
	NNS_G3dGlbLightColor(GX_LIGHTID_3, GX_RGB(0xFF, 0xFF, 0xFF));
	NNS_G3dGlbLightVector(GX_LIGHTID_2, GX_FX32_FX10_MAX, 0, GX_FX32_FX10_MAX);
	NNS_G3dGlbLightColor(GX_LIGHTID_2, GX_RGB(0xFF, 0xFF, 0xFF));
	NNS_G3dGlbFlushP();
	NNS_G3dGeFlushBuffer();


	VecFx32 up = _up;
	VecFx32 pos = _position;
	pos.x = (pos.x + 8) >> 4;
	pos.y = (pos.y + 8) >> 4;
	pos.z = (pos.z + 8) >> 4;
	VecFx32 target = _at;
	target.x = (target.x + 8) >> 4;
	target.y = (target.y + 8) >> 4;
	target.z = (target.z + 8) >> 4;
	NNS_G3dGlbLookAt(&pos, &up, &target);
	NNS_G3dGlbFlushP();

	NNS_G3dGeFlushBuffer();

	NNS_G3dGePushMtx();
	{
		VecFx32 position = { 0, 0, 0 };
		NNS_G3dGeTranslateVec(&position);
		NNS_G3dDraw(&_modelRender);
	}
	NNS_G3dGePopMtx(1);
	//NNS_G3dGlbFlushP();

	_mario->Render();

	NNS_G3dGeFlushBuffer();
	G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
}