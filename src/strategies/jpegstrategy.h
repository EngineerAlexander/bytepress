#ifndef JPEGSTRATEGY_H
#define JPEGSTRATEGY_H

#include "compressionstrategy.h"
#include <QImage>       // For QImage operations
#include <QFileInfo>    // For file information
#include <QTemporaryFile> // For temporary files
#include <QDebug>       // For logging

class JpegStrategy : public CompressionStrategy
{
public:
    JpegStrategy() = default;
    ~JpegStrategy() override = default;

    // Methods from the updated CompressionStrategy base class
    bool processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) override;
    QString getOutputFileName(const QString &inputFileName) const override;
    bool isFileSupported(const QString &filePath) const override;
    QString strategyName() const override;

    // Implementation of other pure virtuals from base
    QString getCompressionType() const override { return "JPEG_to_WebP"; } // Describes the overall process
    bool supportsQualitySlider() const override { return true; } // For the initial JPEG step (quality 100)
    bool isLossless() const override { return false; } // Initial JPEG optimization is lossy (re-save at 100%)
    bool supportsTransparency() const override { return false; } // JPEGs don't support transparency
};

#endif // JPEGSTRATEGY_H 