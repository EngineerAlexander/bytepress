#ifndef PNGSTRATEGY_H
#define PNGSTRATEGY_H

#include "compressionstrategy.h"
#include <QImage>       // For QImage operations
#include <QFileInfo>    // For file information
#include <QTemporaryFile> // For temporary files
#include <QDebug>       // For logging

class PngStrategy : public CompressionStrategy {
public:
    PngStrategy() = default;
    ~PngStrategy() override = default;

    // Methods from the updated CompressionStrategy base class
    bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) override;
    QString getOutputFileName(const QString &inputFileName) const override;
    bool isFileSupported(const QString &filePath) const override;
    QString strategyName() const override;

    // Implementation of other pure virtuals from base
    QString getCompressionType() const override { return "PNG_to_WebP"; } // Describes the overall process
    bool supportsQualitySlider() const override { return false; } // For the initial PNG step
    bool isLossless() const override { return true; } // Initial PNG optimization is lossless
    bool supportsTransparency() const override { return true; } // PNGs support transparency, relevant for WebP
};

#endif // PNGSTRATEGY_H 