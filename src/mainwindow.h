#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QListWidget>
#include <QStringList>
#include <QSlider>
#include <QCheckBox>

// Custom class includes
#include "compressionservice.h"
#include "pngstrategy.h"
#include "jpegstrategy.h"

// Forward declarations for event handlers
class QDragEnterEvent;
class QDropEvent;

// class CompressionStrategy; // Provided by included headers

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onFileSelectionChanged();
    void processSelectedFiles();
    void onSelectFilesButtonClicked();

private:
    void setupUi();
    void addFileToList(const QString &filePath);
    void addFilesInDirectory(const QString &dirPath);
    QString formatFileSize(qint64 bytes);
    CompressionStrategy* getStrategyForFile(const QString &filePath) const;

    QPushButton *processButton;
    QProgressBar *progressBar;
    QListWidget *fileListWidget;
    CompressionService *compressionService;

    // New UI elements for WebP
    QLabel *webpQualityLabel;
    QSlider *webpQualitySlider;
    QLabel *webpAlphaQualityLabel;
    QSlider *webpAlphaQualitySlider;

    QCheckBox *deleteOriginalCheckBox;

    QPushButton *selectFilesButton;
};

#endif // MAINWINDOW_H 