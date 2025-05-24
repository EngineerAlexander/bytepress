#include "compressionservice.h"

CompressionService::CompressionService()
{
}

CompressionService::~CompressionService()
{
    qDeleteAll(strategies);
    strategies.clear();
}

void CompressionService::addStrategy(CompressionStrategy* strategy)
{
    if (strategy && !strategies.contains(strategy)) {
        strategies.append(strategy);
    }
}

CompressionStrategy* CompressionService::getStrategyForFile(const QString &filePath) const
{
    for (CompressionStrategy* strategy : strategies) {
        if (strategy->isFileSupported(filePath)) {
            qInfo() << "Strategy found:" << strategy->strategyName() << "for file:" << filePath;
            return strategy;
        }
    }
    qWarning() << "No suitable strategy found for file:" << filePath;
    return nullptr;
}