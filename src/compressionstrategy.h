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

    // Main processing function: takes input, produces outputWebPFilePath, uses quality settings.
    virtual bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) = 0;

    // Checks if this strategy can handle the file based on its path/extension.
    virtual bool isFileSupported(const QString &filePath) const = 0;

    // Generates an appropriate output WebP file name from an input file name.
    virtual QString getOutputFileName(const QString &inputFileName) const = 0;

    // Returns the name of the strategy (e.g., "PNG", "JPEG").
    virtual QString strategyName() const = 0;

    virtual QString getCompressionType() const = 0; 
    virtual bool supportsQualitySlider() const { return false; }
    virtual bool isLossless() const = 0; 
    virtual bool supportsTransparency() const { return false; }

protected:
    // generateDefaultOutputFileName is removed as output is inplace
};

#endif // COMPRESSIONSTRATEGY_H 