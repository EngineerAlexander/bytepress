#include "avifstrategy.h"
#include <QImageReader> // To check if AVIF can be read

QString AvifStrategy::getOutputFileName(const QString &inputFileName) const {
    QFileInfo fileInfo(inputFileName);
    return fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".webp";
}

bool AvifStrategy::isFileSupported(const QString &filePath) const {
    QString extension = QFileInfo(filePath).suffix().toLower();
    if (extension == "avif") {
        // Check if Qt can actually read AVIF files with the current plugins
        QImageReader reader(filePath);
        if (reader.canRead()) {
            return true;
        }
        qWarning() << "AvifStrategy: AVIF file detected, but Qt cannot read it (missing AVIF image format plugin?):" << filePath;
        return false;
    }
    return false;
}

QString AvifStrategy::strategyName() const {
    return "AVIF";
}

bool AvifStrategy::processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    QImage image(inputFilePath);
    if (image.isNull()) {
        qWarning() << "AvifStrategy: Failed to load AVIF image:" << inputFilePath 
                   << "(Ensure Qt has AVIF image format plugin and the file is valid).";
        return false;
    }

    outputWebPFilePath = getOutputFileName(inputFilePath);
    int qualityForSave = webpQuality;

    if (image.hasAlphaChannel() && webpAlphaQualityHint >= 0) {
        qDebug() << "AvifStrategy: Image has alpha. WebP quality set to:" << webpQuality << "(Alpha hint was:" << webpAlphaQualityHint << ")";
    }

    if (!image.save(outputWebPFilePath, "WEBP", qualityForSave)) {
        qWarning() << "AvifStrategy: Failed to save WebP image from AVIF:" << outputWebPFilePath << "with quality" << qualityForSave;
        return false;
    }

    qDebug() << "AvifStrategy: Successfully converted AVIF" << inputFilePath << "to WebP" << outputWebPFilePath;
    return true;
} 