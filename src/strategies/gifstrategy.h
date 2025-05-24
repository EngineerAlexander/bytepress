#ifndef GIFSTRATEGY_H
#define GIFSTRATEGY_H

#include "compressionstrategy.h"
#include <QImage>       
#include <QFileInfo>    
#include <QDebug>       

class GifStrategy : public CompressionStrategy {
public:
    GifStrategy() = default;
    ~GifStrategy() override = default;

    bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) override;
    QString getOutputFileName(const QString &inputFileName) const override;
    bool isFileSupported(const QString &filePath) const override;
    QString strategyName() const override;

    // Implementation of other pure virtuals from base
    QString getCompressionType() const override { return "GIF_to_WebP"; }
    bool supportsQualitySlider() const override { return false; } // No pre-optimization quality for GIF
    bool isLossless() const override { return true; } // GIF itself is lossless, no pre-optimization step here
    bool supportsTransparency() const override { return true; } // GIFs support transparency

private:
    // Helper for fallback static WebP conversion
    bool convertToStaticWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint);
};

#endif // GIFSTRATEGY_H 