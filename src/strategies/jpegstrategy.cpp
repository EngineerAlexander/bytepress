#include "jpegstrategy.h"
#include <QDir> // Required for QDir::tempPath()
#include <QFile> // Required for QFile::remove
#include <QDebug> // Required for qDebug/qWarning

// Implementation of JpegStrategy methods

QString JpegStrategy::getOutputFileName(const QString &inputFileName) const {
    QFileInfo fileInfo(inputFileName);
    return fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".webp";
}

bool JpegStrategy::isFileSupported(const QString &filePath) const {
    QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "jpg" || extension == "jpeg";
}

QString JpegStrategy::strategyName() const {
    return "JPEG";
}

bool JpegStrategy::processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    Q_UNUSED(webpAlphaQualityHint); // JPEG has no alpha
    QImage originalImage(inputFilePath);
    if (originalImage.isNull()) {
        qWarning() << "JPEGStrategy: Failed to load original image:" << inputFilePath;
        return false;
    }

    QString tempOptimizedPath = QDir::temp().filePath("optimized_" + QFileInfo(inputFilePath).fileName() + ".jpg");
    if (QFile::exists(tempOptimizedPath)) {
        QFile::remove(tempOptimizedPath); // Clean up if it exists
    }

    // 1. Attempt 100% quality JPEG save
    bool savedOptimized = originalImage.save(tempOptimizedPath, "JPG", 100); // Quality 100 for JPG

    QFileInfo originalInfo(inputFilePath);
    QFileInfo optimizedInfo(tempOptimizedPath);
    bool useOptimized = false;

    if (savedOptimized && optimizedInfo.exists() && optimizedInfo.size() > 0) {
        if (optimizedInfo.size() < originalInfo.size()) {
            useOptimized = true;
            qDebug() << "JPEGStrategy: Using 100% quality JPEG (saved" << (originalInfo.size() - optimizedInfo.size()) << "bytes) for WebP conversion.";
        } else {
             qDebug() << "JPEGStrategy: 100% quality JPEG (" << optimizedInfo.size() << "bytes) was not smaller than original (" << originalInfo.size() << "bytes). Using original.";
             if (optimizedInfo.exists() && !QFile::remove(tempOptimizedPath)) {
                qWarning() << "JPEGStrategy: Could not remove temporary 100% quality JPEG file:" << tempOptimizedPath;
            }
        }
    } else {
        qWarning() << "JPEGStrategy: Failed to save or find 100% quality JPEG:" << tempOptimizedPath << "Using original.";
        if (optimizedInfo.exists() && !QFile::remove(tempOptimizedPath)) {
             qWarning() << "JPEGStrategy: Could not remove temporary 100% quality JPEG file after failed save:" << tempOptimizedPath;
        }
    }

    // Determine which image path to use for WebP conversion
    QString imagePathToLoadForWebP = useOptimized ? tempOptimizedPath : inputFilePath;

    QImage imageToConvert(imagePathToLoadForWebP);
    if (imageToConvert.isNull()) {
        qWarning() << "JPEGStrategy: Failed to load image for WebP conversion:" << imagePathToLoadForWebP;
        if (useOptimized && QFile::exists(tempOptimizedPath)) { // Check existence before removing
            QFile::remove(tempOptimizedPath);
        }
        return false;
    }

    // 2. Convert to WebP
    outputWebPFilePath = getOutputFileName(inputFilePath);
    if (!imageToConvert.save(outputWebPFilePath, "WEBP", webpQuality)) {
        qWarning() << "JPEGStrategy: Failed to save WebP image:" << outputWebPFilePath << "with quality" << webpQuality;
        if (useOptimized && QFile::exists(tempOptimizedPath)) { // Check existence before removing
            QFile::remove(tempOptimizedPath);
        }
        return false;
    }

    // Clean up the temporary optimized JPEG if it was used and exists
    if (useOptimized && QFile::exists(tempOptimizedPath)) {
        if (!QFile::remove(tempOptimizedPath)) {
            qWarning() << "JPEGStrategy: Failed to remove temporary 100% quality JPEG file:" << tempOptimizedPath;
        }
    }

    qDebug() << "JPEGStrategy: Successfully converted" << inputFilePath << "to" << outputWebPFilePath;
    return true;
}