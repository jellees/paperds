#include "common.h"
#include "player/Player.h"
#include "camera/Camera.h"
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
	_modelResource = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/coltest.nsbmd", NULL, FALSE);
	NNS_G3dResDefaultSetup(_modelResource);
	NNSG3dResFileHeader* resourceTextures = (NNSG3dResFileHeader*)Util_LoadFileToBuffer("/data/coltest.nsbtx", NULL, TRUE);
	NNS_G3dResDefaultSetup(resourceTextures);
	BOOL binded = NNS_G3dBindMdlSet(NNS_G3dGetMdlSet(_modelResource), NNS_G3dGetTex(resourceTextures));
	NNS_G3dRenderObjInit(&_modelRender, NNS_G3dGetMdlByIdx(NNS_G3dGetMdlSet(_modelResource), 0));
	NNS_FndFreeToExpHeap(gHeapHandle, resourceTextures);

	// Create the player object as mario.
	_mario = new Player();

	// Create a new camera object and set its type.
	_camera = new Camera();
	_camera->SetType(Camera::CAMERA_STANDARD);
	VecFx32 target;
	_mario->GetPosition(&target);
	_camera->SetTarget(&target);
}


Test::~Test()
{
	NNS_FndFreeToExpHeap(gHeapHandle, _modelResource);
}


void Test::Update()
{
	_mario->Update();
	
	VecFx32 target;
	_mario->GetPosition(&target);
	_camera->SetTarget(&target);
	_camera->Update();
}


void Test::Render()
{
	MtxFx44 mtx;
	MTX_PerspectiveW(FX32_SIN30, FX32_COS30, 256 * 4096 / 192, FX32_CONST(1.5), FX32_CONST(2000), 1024, &mtx);
	NNS_G3dGlbSetProjectionMtx(&mtx);
	NNS_G3dGlbFlushP();
	NNS_G3dGeFlushBuffer();

	_camera->Apply();
	NNS_G3dGlbFlushP();
	NNS_G3dGeFlushBuffer();

	_mario->Render();
	NNS_G3dGlbFlushP();

	NNS_G3dGePushMtx();
	{
		VecFx32 position = { 0, 0, 0 };
		NNS_G3dGeTranslateVec(&position);
		NNS_G3dDraw(&_modelRender);
	}
	NNS_G3dGePopMtx(1);
	NNS_G3dGlbFlushP();

	NNS_G3dGeFlushBuffer();
	G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
}