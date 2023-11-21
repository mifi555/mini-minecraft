#include "blocktypeworker.h"
#include "terrain.h"

#define DEBUG 0

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
#if DEBUG
    for (Chunk* &chunk : m_chunksToFill) {

        // flat terrain
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                chunk->setBlockAt(x, 128, z, GRASS);
            }
        }

        // draw the terrain boundaries
        if (chunk->getMinX() == m_minX) {
            for (int z = 0; z < 16; z++) {
                chunk->setBlockAt(0, 128, z, STONE);
            }
        }

        if (chunk->getMinZ() == m_minZ) {
            for (int x = 0; x < 16; x++) {
                chunk->setBlockAt(x, 128, 0, STONE);
            }
        }

        mp_completedChunksLock->lock();
        mp_completedChunks->insert(chunk);
        mp_completedChunksLock->unlock();
    }
#else
    for (Chunk* &chunk : m_chunksToFill) {
        Terrain::generateChunkTerrain(chunk);
        mp_completedChunksLock->lock();
        mp_completedChunks->insert(chunk);
        mp_completedChunksLock->unlock();
    }
#endif
}
