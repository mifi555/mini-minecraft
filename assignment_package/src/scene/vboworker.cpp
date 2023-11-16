#include "vboworker.h"

VBOWorker::VBOWorker(Chunk *c, std::vector<ChunkVBOData> *data, QMutex *dataLock)
    : mp_chunk{c},
    mp_chunkVBOsCompleted{data},
    mp_chunkVBOsCompletedLock{dataLock}
{}

void VBOWorker::run()
{
    // TODO: VBOWorker::run()
}
