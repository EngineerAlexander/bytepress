#ifndef COMPRESSIONSERVICE_H
#define COMPRESSIONSERVICE_H

#include "compressionstrategy.h"
#include <QList>
#include <QString>
#include <QDebug>

class CompressionService
{
public:
    CompressionService();
    ~CompressionService();

    void addStrategy(CompressionStrategy* strategy);
    // Finds the first suitable strategy for the given file type (for compression)
    CompressionStrategy* getStrategyForFile(const QString &filePath) const;

private:
    QList<CompressionStrategy*> strategies;
};

#endif // COMPRESSIONSERVICE_H 