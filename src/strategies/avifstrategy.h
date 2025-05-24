#ifndef AVIFSTRATEGY_H
#define AVIFSTRATEGY_H

#include "compressionstrategy.h"
#include <QImage>       
#include <QFileInfo>    
#include <QDebug>       

class AvifStrategy : public CompressionStrategy {
public:
    AvifStrategy() = default;
    ~AvifStrategy() override = default;

    bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) override;
    QString getOutputFileName(const QString &inputFileName) const override;
    bool isFileSupported(const QString &filePath) const override;
    QString strategyName() const override;

    // Implementation of other pure virtuals from base
    QString getCompressionType() const override { return "AVIF_to_WebP"; }
    bool supportsQualitySlider() const override { return false; } // No pre-optimization quality for AVIF
    bool isLossless() const override { return false; } // AVIF can be lossy or lossless, no pre-optimization step here
    bool supportsTransparency() const override { return true; } // AVIF supports transparency
};

#endif // AVIFSTRATEGY_H 