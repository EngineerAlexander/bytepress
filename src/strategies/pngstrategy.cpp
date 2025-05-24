#include "pngstrategy.h"
#include <QDir> // Required for QDir::tempPath()

QString PngStrategy::getOutputFileName(const QString &inputFileName) const {
    QFileInfo fileInfo(inputFileName);
    return fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".webp";
}

bool PngStrategy::isFileSupported(const QString &filePath) const {
    QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "png";
}

QString PngStrategy::strategyName() const {
    return "PNG";
}

bool PngStrategy::processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    QImage originalImage(inputFilePath);
    if (originalImage.isNull()) {
        qWarning() << "PNGStrategy: Failed to load original image:" << inputFilePath;
        return false;
    }
    
    QString tempOptimizedPath = QDir::temp().filePath("optimized_" + QFileInfo(inputFilePath).fileName() + ".png");
    QFile::remove(tempOptimizedPath); // Clean up if it exists from a previous run for some reason

    // 1. Attempt lossless PNG optimization
    bool savedOptimized = originalImage.save(tempOptimizedPath, "PNG", 0); // Quality 0 for PNG is max lossless compression
    
    QFileInfo originalInfo(inputFilePath);
    QFileInfo optimizedInfo(tempOptimizedPath);
    bool useOptimized = false;

    if (savedOptimized && optimizedInfo.exists() && optimizedInfo.size() > 0) {
        if (optimizedInfo.size() < originalInfo.size()) {
            useOptimized = true;
            qDebug() << "PNGStrategy: Using optimized PNG (saved" << (originalInfo.size() - optimizedInfo.size()) << "bytes) for WebP conversion.";
        } else {
            qDebug() << "PNGStrategy: Optimized PNG (" << optimizedInfo.size() << "bytes) was not smaller than original (" << originalInfo.size() << "bytes). Using original.";
            // If not smaller, or same size, remove the temp file as it's not needed.
             if (!QFile::remove(tempOptimizedPath)) {
                qWarning() << "PNGStrategy: Could not remove temporary optimized file:" << tempOptimizedPath;
            }
        }
    } else {
        qWarning() << "PNGStrategy: Failed to save or find optimized PNG:" << tempOptimizedPath << "Using original.";
         // Ensure temp file is removed if saving failed or it doesn't exist properly
        if (optimizedInfo.exists() && !QFile::remove(tempOptimizedPath)) {
             qWarning() << "PNGStrategy: Could not remove temporary optimized file after failed save:" << tempOptimizedPath;
        }
    }

    const QString *imagePathForWebP = useOptimized ? &tempOptimizedPath : &inputFilePath;

    QImage imageToConvert(*imagePathForWebP);
    if (imageToConvert.isNull()) {
        qWarning() << "PNGStrategy: Failed to load image for WebP conversion:" << *imagePathForWebP;
        if (useOptimized && optimizedInfo.exists()) QFile::remove(tempOptimizedPath); // Clean up temp file if it was used
        return false;
    }

    // 2. Convert to WebP
    outputWebPFilePath = getOutputFileName(inputFilePath);
    int qualityForSave = webpQuality;
    if (imageToConvert.hasAlphaChannel() && webpAlphaQualityHint >= 0) {
        qDebug() << "PNGStrategy: Image has alpha. WebP quality set to:" << webpQuality << "(Alpha hint was:" << webpAlphaQualityHint << ")";
    }

    if (!imageToConvert.save(outputWebPFilePath, "WEBP", qualityForSave)) {
        qWarning() << "PNGStrategy: Failed to save WebP image:" << outputWebPFilePath << "with quality" << qualityForSave;
        if (useOptimized && optimizedInfo.exists()) QFile::remove(tempOptimizedPath); // Clean up temp file
        return false;
    }

    if (useOptimized && optimizedInfo.exists()) {
        if (!QFile::remove(tempOptimizedPath)) { // Clean up the temporary optimized PNG
            qWarning() << "PNGStrategy: Failed to remove temporary optimized file:" << tempOptimizedPath;
        }
    }

    qDebug() << "PNGStrategy: Successfully converted" << inputFilePath << "to" << outputWebPFilePath;
    return true;
} 