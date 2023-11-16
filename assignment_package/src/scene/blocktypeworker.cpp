#include "blocktypeworker.h"

BlockTypeWorker::BlockTypeWorker(
    int x,
    int z,
    std::vector<Chunk*> chunksToFill,
    std::unordered_set<Chunk*>* completedChunks,
    QMutex* completedChunksLock
    ) : m_minX{x},
        m_minZ{z},
        m_chunksToFill{chunksToFill},
        mp_completedChunks{completedChunks},
        mp_completedChunksLock{completedChunksLock} {
}

void BlockTypeWorker::run() {
    qDebug() << "running...";
}
