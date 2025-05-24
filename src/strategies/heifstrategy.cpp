#include "heifstrategy.h"
#include <QImageReader> // To check if HEIF/HEIC can be read

QString HeifStrategy::getOutputFileName(const QString &inputFileName) const {
    QFileInfo fileInfo(inputFileName);
    return fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".webp";
}

bool HeifStrategy::isFileSupported(const QString &filePath) const {
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    if (extension == "heic" || extension == "heif") {
        // Check if Qt can actually read HEIF/HEIC files with the current plugins
        QImageReader reader(filePath);
        if (reader.canRead()) {
            return true;
        }
        qWarning() << "HeifStrategy: HEIF/HEIC file detected, but Qt cannot read it (missing HEIF/HEIC image format plugin?):" << filePath;
        return false;
    }
    return false;
}

QString HeifStrategy::strategyName() const {
    return "HEIF/HEIC";
}

bool HeifStrategy::processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    QImage image(inputFilePath);
    if (image.isNull()) {
        qWarning() << "HeifStrategy: Failed to load HEIF/HEIC image:" << inputFilePath 
                   << "(Ensure Qt has HEIF/HEIC image format plugin and the file is valid).";
        return false;
    }

    outputWebPFilePath = getOutputFileName(inputFilePath);
    int qualityForSave = webpQuality;

    if (image.hasAlphaChannel() && webpAlphaQualityHint >= 0) {
        qDebug() << "HeifStrategy: Image has alpha. WebP quality set to:" << webpQuality << "(Alpha hint was:" << webpAlphaQualityHint << ")";
    }

    if (!image.save(outputWebPFilePath, "WEBP", qualityForSave)) {
        qWarning() << "HeifStrategy: Failed to save WebP image from HEIF/HEIC:" << outputWebPFilePath << "with quality" << qualityForSave;
        return false;
    }

    qDebug() << "HeifStrategy: Successfully converted HEIF/HEIC" << inputFilePath << "to WebP" << outputWebPFilePath;
    return true;
} 