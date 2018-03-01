#pragma once

void* Util_LoadFileToBuffer(const char* path, uint32_t* size, bool tempoarly);
void* Util_LoadLZ77FileToBuffer(const char* path, uint32_t* size, bool tempoarly);
void Util_LoadTextureWithKey(NNSGfdTexKey key, void* data);
void Util_LoadTexture4x4WithKey(NNSGfdTexKey key, void* data, void* indexData);
void Util_LoadTextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadTexture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadLZ77TextureFromCard(const char* texPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadLZ77Texture4x4FromCard(const char* texPath, const char* texIndexPath, const char* plttPath, NNSGfdTexKey &texKey, NNSGfdPlttKey &plttKey);
void Util_LoadPaletteWithKey(NNSGfdPlttKey key, void* data);

void Util_FreeAllToExpHeapByGroupId(NNSFndHeapHandle heap, int groupId);

void Util_DrawSprite(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height);
void Util_DrawSpriteScaled(fx32 x, fx32 y, fx32 z, fx32 width, fx32 height, fx32 scale);

void VEC_Lerp(VecFx32* a, VecFx32* b, fx32 t, VecFx32* result);

void Util_SetupSubOAMForDouble3D();

void Util_SetupBillboardMatrix();
void Util_SetupBillboardYMatrix();

static inline void VEC_Mul(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab)
{
	ab->x = FX_Mul(a->x, b->x);
	ab->y = FX_Mul(a->y, b->y);
	ab->z = FX_Mul(a->z, b->z);
}

static inline void VEC_MulByFx32(VecFx32 *a, fx32 b, VecFx32 *ab)
{
	ab->x = FX_Mul(a->x, b);
	ab->y = FX_Mul(a->y, b);
	ab->z = FX_Mul(a->z, b);
}

void VEC_ProjectOnVector(VecFx32* vec, VecFx32* vec2, VecFx32* dest);
void VEC_ProjectOnPlane(VecFx32* vec, VecFx32* norm, VecFx32* dest);