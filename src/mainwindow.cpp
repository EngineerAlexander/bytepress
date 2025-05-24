#include "mainwindow.h"
#include "gifstrategy.h"
#include "avifstrategy.h"
#include "heifstrategy.h"
#include <QDir>
#include <QApplication>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDirIterator>
#include <QListWidget>
#include <QAbstractItemView>
#include <QFile>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    compressionService = new CompressionService();
    compressionService->addStrategy(new PngStrategy());
    compressionService->addStrategy(new JpegStrategy());
    compressionService->addStrategy(new GifStrategy());
    compressionService->addStrategy(new AvifStrategy());
    compressionService->addStrategy(new HeifStrategy());

    setupUi();
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete compressionService;
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Layout for file list label and add files button
    QHBoxLayout *fileSelectionLayout = new QHBoxLayout();
    QLabel *fileListLabel = new QLabel("Files to compress (drag & drop or add):");
    fileSelectionLayout->addWidget(fileListLabel, 1);

    selectFilesButton = new QPushButton("Add Files...", this);
    connect(selectFilesButton, &QPushButton::clicked, this, &MainWindow::onSelectFilesButtonClicked);
    fileSelectionLayout->addWidget(selectFilesButton);
    mainLayout->addLayout(fileSelectionLayout);

    fileListWidget = new QListWidget(this);
    fileListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    mainLayout->addWidget(fileListWidget);

    processButton = new QPushButton("Compress Selected Files", this);
    connect(processButton, &QPushButton::clicked, this, &MainWindow::processSelectedFiles);
    mainLayout->addWidget(processButton);

    // WebP Quality Slider
    webpQualityLabel = new QLabel(QString("WebP Quality: %1").arg(80)); // Default to 80
    mainLayout->addWidget(webpQualityLabel);
    webpQualitySlider = new QSlider(Qt::Horizontal, this);
    webpQualitySlider->setRange(0, 100);
    webpQualitySlider->setValue(80);
    connect(webpQualitySlider, &QSlider::valueChanged, this, [this](int value) {
        webpQualityLabel->setText(QString("WebP Quality: %1").arg(value));
    });
    mainLayout->addWidget(webpQualitySlider);

    // WebP Alpha Quality Slider
    webpAlphaQualityLabel = new QLabel(QString("WebP Alpha Quality: %1").arg(100)); // Default to 100
    mainLayout->addWidget(webpAlphaQualityLabel);
    webpAlphaQualitySlider = new QSlider(Qt::Horizontal, this);
    webpAlphaQualitySlider->setRange(0, 100);
    webpAlphaQualitySlider->setValue(100);
    connect(webpAlphaQualitySlider, &QSlider::valueChanged, this, [this](int value) {
        webpAlphaQualityLabel->setText(QString("WebP Alpha Quality: %1").arg(value));
    });
    mainLayout->addWidget(webpAlphaQualitySlider);

    // Delete Original Checkbox
    deleteOriginalCheckBox = new QCheckBox("Delete original files after successful WebP conversion", this);
    deleteOriginalCheckBox->setChecked(false); // Default to not deleting
    mainLayout->addWidget(deleteOriginalCheckBox);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    mainLayout->addWidget(progressBar);

    setCentralWidget(centralWidget);
    setWindowTitle("BytePress - WebP Batch Compressor");
    resize(500, 450);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (const QUrl &url : mimeData->urls()) {
            QString filePath = url.toLocalFile();
            QFileInfo fileInfo(filePath);
            if (fileInfo.isFile()) addFileToList(filePath);
            else if (fileInfo.isDir()) addFilesInDirectory(filePath);
        }
    }
}

QString MainWindow::formatFileSize(qint64 bytes)
{
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    if (bytes >= gb) return QString::number(static_cast<double>(bytes) / gb, 'f', 2) + " GB";
    if (bytes >= mb) return QString::number(static_cast<double>(bytes) / mb, 'f', 2) + " MB";
    if (bytes >= kb) return QString::number(static_cast<double>(bytes) / kb, 'f', 2) + " KB";
    return QString::number(bytes) + " Bytes";
}

void MainWindow::addFileToList(const QString &filePath)
{
    for (int i = 0; i < fileListWidget->count(); ++i) {
        if (fileListWidget->item(i)->data(Qt::UserRole).toString() == filePath) return;
    }
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) return;
    QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(fileInfo.fileName()).arg(formatFileSize(fileInfo.size())));
    item->setData(Qt::UserRole, filePath); 
    fileListWidget->addItem(item);
}

void MainWindow::addFilesInDirectory(const QString &dirPath)
{
    QDirIterator it(dirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) addFileToList(it.next());
}

// Uses the simplified CompressionService method
CompressionStrategy* MainWindow::getStrategyForFile(const QString &filePath) const
{
    return compressionService->getStrategyForFile(filePath);
}

void MainWindow::onFileSelectionChanged()
{
    // This slot can be used for other UI updates if needed, e.g., enabling/disabling processButton
    // For now, its primary role (slider visibility) is gone.
    // processButton->setEnabled(fileListWidget->selectedItems().count() > 0); // Example: enable button if items selected
}

void MainWindow::onSelectFilesButtonClicked()
{
    QStringList selectedFilePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Select Images to Compress"),
        QDir::homePath(), // Default directory to open
        tr("Images (*.png *.jpg *.jpeg *.gif *.avif *.heic *.heif)")
    );

    if (!selectedFilePaths.isEmpty()) {
        for (const QString &filePath : selectedFilePaths) {
            addFileToList(filePath);
        }
    }
}

void MainWindow::processSelectedFiles()
{
    // Collect all unprocessed items from the list
    QList<QListWidgetItem*> itemsToProcess;
    for (int i = 0; i < fileListWidget->count(); ++i) {
        QListWidgetItem *currentItem = fileListWidget->item(i);
        QString currentText = currentItem->text();
        if (!currentText.contains("Saved as WebP") &&
            !currentText.contains("WebP Error") && 
            !currentText.contains("Conversion Error") &&
            !currentText.contains("No Method for WebP")) {
            itemsToProcess.append(currentItem);
        }
    }

    if (itemsToProcess.isEmpty()) {
        QMessageBox::information(this, "Information", "No unprocessed files found in the queue.");
        return;
    }

    // Get WebP quality settings from sliders
    int webpQuality = webpQualitySlider->value();
    int webpAlphaQuality = webpAlphaQualitySlider->value();

    progressBar->setValue(0);
    progressBar->setMaximum(itemsToProcess.count());
    progressBar->setFormat("%v/%m files");
    int successCount = 0;

    for (int i = 0; i < itemsToProcess.count(); ++i) {
        QListWidgetItem *currentItem = itemsToProcess.at(i);
        QString inputFilePath = currentItem->data(Qt::UserRole).toString();
        QFileInfo originalFileInfo(inputFilePath);
        QString originalFileName = originalFileInfo.fileName();
        QString initialDisplayText = QString("%1 (%2)").arg(originalFileName).arg(formatFileSize(originalFileInfo.size()));
        
        currentItem->setText(initialDisplayText + " - Processing to WebP...");
        QApplication::processEvents(); 

        CompressionStrategy* strategy = getStrategyForFile(inputFilePath);
        QString outputWebPFilePath;

        if (strategy) {
            bool processedOk = strategy->processToWebP(inputFilePath, outputWebPFilePath, webpQuality, webpAlphaQuality);
            
            if (processedOk) {
                QFileInfo finalWebPInfo(outputWebPFilePath);
                if (finalWebPInfo.exists() && finalWebPInfo.size() > 0) {
                    successCount++;
                    QString newDisplayText = QString("%1 (%2) - Saved as WebP").arg(finalWebPInfo.fileName()).arg(formatFileSize(finalWebPInfo.size()));
                    
                    // Calculate and add savings percentage
                    qint64 originalSize = originalFileInfo.size();
                    qint64 finalSize = finalWebPInfo.size();
                    QString savingsDetailText = "";

                    if (originalSize > 0) {
                        if (finalSize < originalSize) {
                            double savingsPercent = (static_cast<double>(originalSize - finalSize) / originalSize) * 100.0;
                            savingsDetailText = QString(" (Original: %1, Saved: %2%)")
                                                .arg(formatFileSize(originalSize))
                                                .arg(QString::number(savingsPercent, 'f', 1));
                        } else if (finalSize == originalSize) {
                            savingsDetailText = QString(" (Original: %1, No size change)")
                                                .arg(formatFileSize(originalSize));
                        } else { // finalSize > originalSize
                            double increasePercent = (static_cast<double>(finalSize - originalSize) / originalSize) * 100.0;
                            savingsDetailText = QString(" (Original: %1, Larger by: %2%)")
                                                .arg(formatFileSize(originalSize))
                                                .arg(QString::number(increasePercent, 'f', 1));
                        }
                    } else {
                        savingsDetailText = " (Original size unknown or zero)";
                    }
                    newDisplayText += savingsDetailText;

                    currentItem->setText(newDisplayText);

                    // Check if original should be deleted
                    if (deleteOriginalCheckBox->isChecked()) {
                        if (QFile::remove(inputFilePath)) {
                            qDebug() << "Successfully deleted original file:" << inputFilePath;
                        } else {
                            qWarning() << "Failed to delete original file:" << inputFilePath;
                            QMessageBox::warning(this, "Deletion Error", QString("Could not delete original file: %1").arg(originalFileName));
                        }
                    }

                } else {
                    currentItem->setText(initialDisplayText + " - WebP Error (File not created or empty)");
                    QMessageBox::warning(this, "Processing Error", QString("Error creating WebP for: %1. File not found or empty.").arg(originalFileName));
                }
            } else {
                currentItem->setText(initialDisplayText + " - WebP Conversion Error");
                QMessageBox::warning(this, "Processing Error", QString("Failed to convert to WebP: %1").arg(originalFileName));
            }
        } else {
            currentItem->setText(initialDisplayText + " - No Method for WebP");
            QMessageBox::warning(this, "Strategy Error", QString("No compression method found for WebP conversion of: %1").arg(originalFileName));
        }
        progressBar->setValue(i + 1);
        QApplication::processEvents(); 
    }
    
    QString summary = QString("Converted %1 of %2 file(s) to WebP.").arg(successCount).arg(itemsToProcess.count());
    progressBar->setFormat(summary);
    QMessageBox::information(this, "Processing Complete", summary);
} 