#ifndef HEIFSTRATEGY_H
#define HEIFSTRATEGY_H

#include "compressionstrategy.h"
#include <QImage>       
#include <QFileInfo>    
#include <QDebug>       

class HeifStrategy : public CompressionStrategy {
public:
    HeifStrategy() = default;
    ~HeifStrategy() override = default;

    bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) override;
    QString getOutputFileName(const QString &inputFileName) const override;
    bool isFileSupported(const QString &filePath) const override;
    QString strategyName() const override;

    // Implementation of other pure virtuals from base
    QString getCompressionType() const override { return "HEIF_to_WebP"; }
    bool supportsQualitySlider() const override { return false; } // No pre-optimization quality for HEIF/HEIC
    bool isLossless() const override { return false; } // HEIF/HEIC can be lossy or lossless, no pre-optimization step here
    bool supportsTransparency() const override { return true; } // HEIF/HEIC supports transparency
};

#endif // HEIFSTRATEGY_H 