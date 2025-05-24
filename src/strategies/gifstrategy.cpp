#include "gifstrategy.h"
#include <QFile> // For QFile::exists etc. if needed, though QImage handles loading.
#include <QProcess> // Added for running external tool
#include <QStandardPaths> // Added for finding executable

QString GifStrategy::getOutputFileName(const QString &inputFileName) const {
    QFileInfo fileInfo(inputFileName);
    return fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".webp";
}

bool GifStrategy::isFileSupported(const QString &filePath) const {
    QString extension = QFileInfo(filePath).suffix().toLower();
    return extension == "gif";
}

QString GifStrategy::strategyName() const {
    return "GIF";
}

// Fallback method for static WebP conversion from GIF (first frame)
bool GifStrategy::convertToStaticWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    QImage image(inputFilePath);
    if (image.isNull()) {
        qWarning() << "GIFStrategy (Fallback): Failed to load GIF image (or first frame):" << inputFilePath;
        return false;
    }
    outputWebPFilePath = getOutputFileName(inputFilePath);
    if (image.hasAlphaChannel() && webpAlphaQualityHint >= 0) {
        qDebug() << "GIFStrategy (Fallback): Image has alpha. WebP quality set to:" << webpQuality << "(Alpha hint was:" << webpAlphaQualityHint << ")";
    }
    if (!image.save(outputWebPFilePath, "WEBP", webpQuality)) {
        qWarning() << "GIFStrategy (Fallback): Failed to save static WebP image from GIF:" << outputWebPFilePath << "with quality" << webpQuality;
        return false;
    }
    qDebug() << "GIFStrategy (Fallback): Successfully converted GIF" << inputFilePath << "to static WebP" << outputWebPFilePath;
    return true;
}

bool GifStrategy::processToWebP(const QString &inputFilePath, QString &outputWebPFilePath, int webpQuality, int webpAlphaQualityHint) {
    outputWebPFilePath = getOutputFileName(inputFilePath);
    QString gif2webpPath = QStandardPaths::findExecutable("gif2webp");

    if (!gif2webpPath.isEmpty()) {
        qDebug() << "GIFStrategy: Using gif2webp found at:" << gif2webpPath;
        QProcess process;
        QStringList arguments;
        arguments << "-q" << QString::number(webpQuality);
        // gif2webp has other options like -lossy, -mixed, -m, -d, -loop_count, etc.
        // For simplicity, we are only using quality for now.
        arguments << inputFilePath << "-o" << outputWebPFilePath;

        process.start(gif2webpPath, arguments);
        if (!process.waitForFinished(30000)) { // 30 second timeout
            qWarning() << "GIFStrategy: gif2webp timed out for" << inputFilePath;
            process.kill();
            return convertToStaticWebP(inputFilePath, outputWebPFilePath, webpQuality, webpAlphaQualityHint); // Fallback on timeout
        }

        if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
            QFileInfo webpFile(outputWebPFilePath);
            if (webpFile.exists() && webpFile.size() > 0) {
                qDebug() << "GIFStrategy: Successfully converted animated GIF" << inputFilePath << "to animated WebP" << outputWebPFilePath << "using gif2webp.";
                return true;
            } else {
                 qWarning() << "GIFStrategy: gif2webp reported success, but output file is missing or empty:" << outputWebPFilePath;
                 return convertToStaticWebP(inputFilePath, outputWebPFilePath, webpQuality, webpAlphaQualityHint); // Fallback
            }
        } else {
            qWarning() << "GIFStrategy: gif2webp failed for" << inputFilePath << "Exit code:" << process.exitCode() << "Error:" << process.readAllStandardError();
            return convertToStaticWebP(inputFilePath, outputWebPFilePath, webpQuality, webpAlphaQualityHint); // Fallback on error
        }
    } else {
        qWarning() << "GIFStrategy: gif2webp command not found in PATH. Falling back to static WebP conversion (first frame).";
        return convertToStaticWebP(inputFilePath, outputWebPFilePath, webpQuality, webpAlphaQualityHint);
    }
} 