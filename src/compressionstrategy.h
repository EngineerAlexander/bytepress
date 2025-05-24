#ifndef COMPRESSIONSTRATEGY_H
#define COMPRESSIONSTRATEGY_H

#include <QString>
#include <QImage>
#include <QFileInfo>
#include <QTemporaryFile> // For safer temporary file handling

class CompressionStrategy
{
public:
    virtual ~CompressionStrategy() = default;

    virtual bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) = 0;

    virtual bool isFileSupported(const QString &filePath) const = 0;

    virtual QString getOutputFileName(const QString &inputFileName) const = 0;

    virtual QString strategyName() const = 0;

    virtual QString getCompressionType() const = 0; 
    virtual bool supportsQualitySlider() const { return false; }
    virtual bool isLossless() const = 0; 
    virtual bool supportsTransparency() const { return false; }
};

#endif // COMPRESSIONSTRATEGY_H 