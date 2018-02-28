#include "common.h"

NNSFndHeapHandle gHeapHandle;

#define SYSTEM_HEAP_SIZE        64*1024

#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define ROUND_DOWN(value, alignment) \
    ((u32)(value) & ~(alignment-1))

void Core_PreInit()
{
	void* sysHeapMemory = OS_AllocFromMainArenaLo(SYSTEM_HEAP_SIZE, 16);
	u32 arenaLow = ROUND_UP(OS_GetMainArenaLo(), 16);
	u32 arenaHigh = ROUND_DOWN(OS_GetMainArenaHi(), 16);
	u32 appHeapSize = arenaHigh - arenaLow;
	void* appHeapMemory = OS_AllocFromMainArenaLo(appHeapSize, 16);
	gHeapHandle = NNS_FndCreateExpHeap(appHeapMemory, appHeapSize);
}

void* operator new(std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 32);
	MI_CpuClear32(block, blocksize);
	return block;
}

void* operator new[](std::size_t blocksize)
{
    void* block = NNS_FndAllocFromExpHeapEx(gHeapHandle, blocksize, 32);
	MI_CpuClear32(block, blocksize);
	return block;
}

void operator delete(void* block) throw()
{
    NNS_FndFreeToExpHeap(gHeapHandle, block);
}

void operator delete[](void* block) throw()
{
    NNS_FndFreeToExpHeap(gHeapHandle, block);
}

#define SOUND_HEAP_SIZE 0x80000

NNSSndArc gSndArc;

static u8 mSndHeap[SOUND_HEAP_SIZE];
NNSSndHeapHandle gSndHeapHandle;

#define STREAM_THREAD_PRIO 10

static uint32_t mSzWork;
static void* mPMgrWork;

static TPData mTPData[4];

void Core_Init()
{

}