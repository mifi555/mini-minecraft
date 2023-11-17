#ifndef VBOWORKER_H
#define VBOWORKER_H

#include <QRunnable>
#include <QMutex>

#include <vector>

#include "chunk.h"

class VBOWorker : public QRunnable
{
private:
    Chunk* mp_chunk;
    std::vector<ChunkVBOData> *mp_chunkVBOsCompleted;
    QMutex *mp_chunkVBOsCompletedLock;
public:
    VBOWorker(Chunk* c, std::vector<ChunkVBOData> *data, QMutex *dataLock);
    void run() override;
};

#endif // VBOWORKER_H
