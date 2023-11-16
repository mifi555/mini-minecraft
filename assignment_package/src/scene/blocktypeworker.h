#ifndef BLOCKTYPEWORKER_H
#define BLOCKTYPEWORKER_H

#include <QRunnable>
#include <QMutex>

#include <unordered_set>

#include "chunk.h"

class BlockTypeWorker : public QRunnable
{
private:
    int m_minX, m_minZ;
    std::vector<Chunk*> m_chunksToFill;
    std::unordered_set<Chunk*> *mp_completedChunks;
    QMutex *mp_completedChunksLock;

public:
    BlockTypeWorker(
        int x,
        int z,
        std::vector<Chunk*> chunksToFill,
        std::unordered_set<Chunk*>* completedChunks,
        QMutex* completedChunksLock
    );

    void run() override;
};

#endif // BLOCKTYPEWORKER_H
