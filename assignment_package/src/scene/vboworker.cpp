#include "vboworker.h"

VBOWorker::VBOWorker(Chunk *c, std::vector<ChunkVBOData> *data, QMutex *dataLock)
    : mp_chunk{c},
    mp_chunkVBOsCompleted{data},
    mp_chunkVBOsCompletedLock{dataLock}
{}

void VBOWorker::run()
{
    ChunkVBOData vboData(mp_chunk);
    mp_chunk->createMultithreaded(vboData);
    mp_chunkVBOsCompletedLock->lock();
    mp_chunkVBOsCompleted->push_back(vboData);
    mp_chunkVBOsCompletedLock->unlock();
}
