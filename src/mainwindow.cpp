#include "mainwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>
#include <QStyle>
#include <QLabel>
#include <QProgressBar>
#include <QtConcurrent>
#include <utility>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_hasUnsavedChanges(false)
    , m_compareViewer(nullptr)
    , m_filterDock(nullptr)
    , m_filterPanel(nullptr)
    , m_progressDock(nullptr)
    , m_progressPanel(nullptr)
    , m_taskManager(new TaskManager(this))
    , m_previewTimer(new QTimer(this))
    , m_previewWatcher(new QFutureWatcher<QPair<QImage, qint64>>(this))
    , m_previewDirty(false)
    , m_previewSerial(0)
{
    setWindowTitle(QStringLiteral("多线程图像滤镜处理器"));
    resize(1280, 800);
    setAcceptDrops(true);
    setDockNestingEnabled(true);

    m_previewTimer->setSingleShot(true);
    m_previewTimer->setInterval(250);
    connect(m_previewTimer, &QTimer::timeout, this, &MainWindow::onDebouncePreview);
    connect(m_previewWatcher, &QFutureWatcher<QImage>::finished, this, &MainWindow::onPreviewFinished);

    setupUi();
    setupMenuBar();
    setupToolBar();
    setupDockWidgets();
    setupConnections();
    updateActions();

    statusBar()->showMessage(QStringLiteral("就绪 - 请打开一张图片开始处理 (支持 JPG / PNG / BMP)"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    m_compareViewer = new CompareViewer(this);
    setCentralWidget(m_compareViewer);
}

void MainWindow::setupMenuBar()
{
    QStyle* s = style();

    QMenu* fileMenu = menuBar()->addMenu(QStringLiteral("文件(&F)"));
    m_actionOpen = new QAction(s->standardIcon(QStyle::SP_DirOpenIcon), QStringLiteral("打开图片..."), this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onOpenImage);
    fileMenu->addAction(m_actionOpen);

    m_actionSave = new QAction(s->standardIcon(QStyle::SP_DialogSaveButton), QStringLiteral("保存"), this);
    m_actionSave->setShortcut(QKeySequence::Save);
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onSaveImage);
    fileMenu->addAction(m_actionSave);

    m_actionSaveAs = new QAction(QStringLiteral("另存为..."), this);
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsImage);
    fileMenu->addAction(m_actionSaveAs);

    m_actionReplace = new QAction(s->standardIcon(QStyle::SP_DialogResetButton), QStringLiteral("替换原图继续编辑"), this);
    connect(m_actionReplace, &QAction::triggered, this, &MainWindow::onReplaceOriginal);
    fileMenu->addAction(m_actionReplace);

    fileMenu->addSeparator();
    m_actionExit = new QAction(QStringLiteral("退出"), this);
    m_actionExit->setShortcut(QKeySequence::Quit);
    connect(m_actionExit, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_actionExit);

    QMenu* editMenu = menuBar()->addMenu(QStringLiteral("处理(&E)"));
    m_actionApply = new QAction(s->standardIcon(QStyle::SP_CommandLink), QStringLiteral("应用滤镜(多线程)"), this);
    m_actionApply->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return));
    connect(m_actionApply, &QAction::triggered, this, &MainWindow::onApplyFilter);
    editMenu->addAction(m_actionApply);

    m_actionPreview = new QAction(s->standardIcon(QStyle::SP_FileDialogDetailedView), QStringLiteral("快速预览(单线程)"), this);
    m_actionPreview->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    connect(m_actionPreview, &QAction::triggered, this, &MainWindow::onPreviewFilter);
    editMenu->addAction(m_actionPreview);

    editMenu->addSeparator();
    m_actionCancel = new QAction(s->standardIcon(QStyle::SP_DialogCancelButton), QStringLiteral("取消处理"), this);
    m_actionCancel->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(m_actionCancel, &QAction::triggered, this, &MainWindow::onCancelProcessing);
    editMenu->addAction(m_actionCancel);

    QMenu* viewMenu = menuBar()->addMenu(QStringLiteral("视图(&V)"));
    m_actionToggleGrid = new QAction(QStringLiteral("显示网格分区"), this);
    m_actionToggleGrid->setCheckable(true);
    m_actionToggleGrid->setChecked(true);
    connect(m_actionToggleGrid, &QAction::toggled, this, &MainWindow::onToggleGrid);
    viewMenu->addAction(m_actionToggleGrid);

    viewMenu->addSeparator();
    m_actionZoomIn = new QAction(s->standardIcon(QStyle::SP_ArrowUp), QStringLiteral("放大"), this);
    m_actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(m_actionZoomIn, &QAction::triggered, this, &MainWindow::onZoomIn);
    viewMenu->addAction(m_actionZoomIn);

    m_actionZoomOut = new QAction(s->standardIcon(QStyle::SP_ArrowDown), QStringLiteral("缩小"), this);
    m_actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(m_actionZoomOut, &QAction::triggered, this, &MainWindow::onZoomOut);
    viewMenu->addAction(m_actionZoomOut);

    m_actionFit = new QAction(QStringLiteral("适应窗口"), this);
    m_actionFit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(m_actionFit, &QAction::triggered, this, &MainWindow::onFitWindow);
    viewMenu->addAction(m_actionFit);

    m_actionResetZoom = new QAction(QStringLiteral("原始大小"), this);
    connect(m_actionResetZoom, &QAction::triggered, this, &MainWindow::onZoomReset);
    viewMenu->addAction(m_actionResetZoom);

    QMenu* helpMenu = menuBar()->addMenu(QStringLiteral("帮助(&H)"));
    m_actionAbout = new QAction(QStringLiteral("关于程序"), this);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(m_actionAbout);

    m_actionAboutQt = new QAction(QStringLiteral("关于 Qt"), this);
    connect(m_actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    helpMenu->addAction(m_actionAboutQt);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar(QStringLiteral("主工具栏"));
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_mainToolBar->setMovable(false);
    m_mainToolBar->addAction(m_actionOpen);
    m_mainToolBar->addAction(m_actionSave);
    m_mainToolBar->addAction(m_actionSaveAs);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actionApply);
    m_mainToolBar->addAction(m_actionPreview);
    m_mainToolBar->addAction(m_actionCancel);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actionToggleGrid);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_actionReplace);
}

void MainWindow::setupDockWidgets()
{
    m_filterDock = new QDockWidget(QStringLiteral("滤镜与网格设置"), this);
    m_filterDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_filterPanel = new FilterControlsPanel(m_filterDock);
    m_filterDock->setWidget(m_filterPanel);
    addDockWidget(Qt::LeftDockWidgetArea, m_filterDock);

    m_progressDock = new QDockWidget(QStringLiteral("处理进度与性能统计"), this);
    m_progressDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_progressPanel = new ProgressPanel(m_progressDock);
    m_progressDock->setWidget(m_progressPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_progressDock);
}

void MainWindow::setupConnections()
{
    connect(m_taskManager, &TaskManager::progressChanged, this, &MainWindow::onProgressChanged);
    connect(m_taskManager, &TaskManager::regionCompleted, this, &MainWindow::onRegionCompleted);
    connect(m_taskManager, &TaskManager::regionThreadIdReady, m_progressPanel, &ProgressPanel::setRegionThreadId);
    connect(m_taskManager, &TaskManager::processingFinished, this, &MainWindow::onProcessingFinished);
    connect(m_taskManager, &TaskManager::processingCancelled, this, &MainWindow::onProcessingCancelled);
    connect(m_progressPanel, &ProgressPanel::cancelRequested, this, &MainWindow::onCancelProcessing);
    connect(m_filterPanel, &FilterControlsPanel::filterChainChanged, this, &MainWindow::onFilterChainChanged);
    connect(m_filterPanel, &FilterControlsPanel::parametersChanged, this, &MainWindow::onParametersChanged);
}

void MainWindow::updateActions()
{
    bool hasImage = !m_originalImage.isNull();
    bool processing = m_taskManager->isProcessing();
    bool hasFilters = !m_filterPanel->currentFilterChain().isEmpty();
    bool hasResult = !m_resultImage.isNull();

    m_actionSave->setEnabled(hasResult && !processing);
    m_actionSaveAs->setEnabled(hasResult && !processing);
    m_actionReplace->setEnabled(hasResult && !processing);
    m_actionApply->setEnabled(hasImage && hasFilters && !processing);
    m_actionPreview->setEnabled(hasImage && hasFilters && !processing);
    m_actionCancel->setEnabled(processing);
    m_actionToggleGrid->setEnabled(hasImage);
}

QString MainWindow::getFileFilters() const
{
    return QStringLiteral("图像文件 (*.jpg *.jpeg *.png *.bmp);;JPEG 图像 (*.jpg *.jpeg);;PNG 图像 (*.png);;BMP 图像 (*.bmp)");
}

bool MainWindow::loadImageFromFile(const QString& path)
{
    QImage img;
    if (!img.load(path)) {
        QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("无法加载图片:\n%1").arg(path));
        return false;
    }
    m_originalImage = img.convertToFormat(QImage::Format_RGB32);
    m_currentFilePath = path;
    m_resultImage = QImage();
    m_hasUnsavedChanges = false;
    m_compareViewer->setOriginalImage(m_originalImage);
    m_compareViewer->setResultImage(QImage());
    m_compareViewer->setGridOverlay(m_filterPanel->gridRows(), m_filterPanel->gridCols(), m_actionToggleGrid->isChecked());
    m_progressPanel->reset();
    statusBar()->showMessage(QStringLiteral("已加载: %1 (%2 x %3)")
        .arg(QFileInfo(path).fileName())
        .arg(m_originalImage.width())
        .arg(m_originalImage.height()));
    updateActions();
    return true;
}

bool MainWindow::saveImageToFile(const QString& path, const QImage& image)
{
    if (image.isNull()) return false;
    if (!image.save(path)) {
        QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("保存图片失败:\n%1").arg(path));
        return false;
    }
    m_hasUnsavedChanges = false;
    statusBar()->showMessage(QStringLiteral("已保存: %1").arg(QFileInfo(path).fileName()));
    return true;
}

void MainWindow::onOpenImage()
{
    if (!confirmDiscard()) return;
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片"), QString(), getFileFilters());
    if (!path.isEmpty()) {
        loadImageFromFile(path);
    }
}

void MainWindow::onSaveImage()
{
    if (m_resultImage.isNull()) return;
    if (m_currentFilePath.isEmpty()) {
        onSaveAsImage();
        return;
    }
    saveImageToFile(m_currentFilePath, m_resultImage);
}

void MainWindow::onSaveAsImage()
{
    if (m_resultImage.isNull()) return;
    QString defaultName;
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fi(m_currentFilePath);
        defaultName = fi.absolutePath() + "/" + fi.completeBaseName() + "_filtered." + fi.suffix();
    }
    QString path = QFileDialog::getSaveFileName(this, QStringLiteral("另存为图片"), defaultName, getFileFilters());
    if (!path.isEmpty()) {
        saveImageToFile(path, m_resultImage);
    }
}

void MainWindow::onReplaceOriginal()
{
    if (m_resultImage.isNull()) return;
    int ret = QMessageBox::question(this, QStringLiteral("替换原图"),
        QStringLiteral("当前结果图将作为新的原图，未保存的修改将丢失。\n是否继续?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    m_originalImage = m_resultImage;
    m_resultImage = QImage();
    m_compareViewer->setOriginalImage(m_originalImage);
    m_compareViewer->setResultImage(QImage());
    m_compareViewer->setGridOverlay(m_filterPanel->gridRows(), m_filterPanel->gridCols(), m_actionToggleGrid->isChecked());
    m_progressPanel->reset();
    m_hasUnsavedChanges = true;
    statusBar()->showMessage(QStringLiteral("已用结果图替换原图 - 可以继续叠加滤镜"));
    updateActions();
}

void MainWindow::onApplyFilter()
{
    auto chain = m_filterPanel->currentFilterChain();
    if (chain.isEmpty() || m_originalImage.isNull()) return;

    m_progressPanel->reset();
    int totalRegions = m_filterPanel->gridRows() * m_filterPanel->gridCols();
    m_progressPanel->setTotalRegions(totalRegions);

    m_taskManager->setSourceImage(m_originalImage);
    m_taskManager->setGridSize(m_filterPanel->gridRows(), m_filterPanel->gridCols());
    m_taskManager->setFilterChain(chain);
    m_taskManager->startProcessing();
    updateActions();
    statusBar()->showMessage(QStringLiteral("开始多线程处理 - %1 个区域, %2 个滤镜").arg(totalRegions).arg(chain.size()));
}

void MainWindow::onPreviewFilter()
{
    auto chain = m_filterPanel->currentFilterChain();
    if (chain.isEmpty() || m_originalImage.isNull()) return;

    QElapsedTimer t;
    t.start();
    statusBar()->showMessage(QStringLiteral("正在快速预览 (单线程)..."));
    QApplication::processEvents();

    QImage result = m_originalImage;
    for (const auto& f : chain) {
        result = Filter::apply(result, f);
    }

    m_previewImage = result;
    m_resultImage = result;
    m_compareViewer->setResultImage(result);
    m_hasUnsavedChanges = true;

    qint64 elapsed = t.elapsed();
    statusBar()->showMessage(QStringLiteral("预览完成 - 耗时 %1 ms (单线程, 参考值)").arg(elapsed));
    updateActions();
}

void MainWindow::onProcessingFinished(const QImage& result, const ProcessingStats& stats)
{
    m_resultImage = result;
    m_compareViewer->setResultImage(result);
    m_progressPanel->showStats(stats);
    m_hasUnsavedChanges = true;
    statusBar()->showMessage(QStringLiteral("完成! 并行耗时 %1 ms, 估计串行 %2 ms, 加速比 %3x")
        .arg(stats.totalParallelMs)
        .arg(stats.estimatedSequentialMs)
        .arg(stats.speedup(), 0, 'f', 2));
    updateActions();
}

void MainWindow::onProcessingCancelled()
{
    m_progressPanel->reset();
    statusBar()->showMessage(QStringLiteral("处理已取消"));
    updateActions();
}

void MainWindow::onCancelProcessing()
{
    if (!m_taskManager->isProcessing()) return;
    statusBar()->showMessage(QStringLiteral("正在取消处理..."));
    m_taskManager->cancelProcessing();
}

void MainWindow::onProgressChanged(int percent, qint64 elapsed, qint64 remaining)
{
    m_progressPanel->setProgress(percent, elapsed, remaining);
    if (remaining > 0) {
        statusBar()->showMessage(QStringLiteral("处理中... %1%  已用时 %2  剩余约 %3")
            .arg(percent).arg(elapsed).arg(remaining));
    }
}

void MainWindow::onRegionCompleted(int idx, qint64 ms)
{
    m_progressPanel->setRegionCompleted(idx, ms);
}

void MainWindow::onFilterChainChanged()
{
    m_compareViewer->setGridOverlay(m_filterPanel->gridRows(), m_filterPanel->gridCols(), m_actionToggleGrid->isChecked());
    updateActions();
    auto chain = m_filterPanel->currentFilterChain();
    if (!chain.isEmpty() && !m_originalImage.isNull() && !m_taskManager->isProcessing()) {
        m_previewDirty = true;
        m_previewTimer->start();
    }
}

void MainWindow::onParametersChanged()
{
    auto chain = m_filterPanel->currentFilterChain();
    if (!chain.isEmpty() && !m_originalImage.isNull() && !m_taskManager->isProcessing()) {
        m_previewDirty = true;
        m_previewTimer->start();
    }
}

void MainWindow::onDebouncePreview()
{
    if (!m_previewDirty) return;
    auto chain = m_filterPanel->currentFilterChain();
    if (chain.isEmpty() || m_originalImage.isNull() || m_taskManager->isProcessing()) return;

    if (m_previewWatcher->isRunning()) return;

    m_previewDirty = false;
    m_previewSerial++;
    qint64 serial = m_previewSerial;
    QImage source = m_originalImage;
    QList<FilterParams> filters = chain;

    statusBar()->showMessage(QStringLiteral("正在异步预览..."));

    auto worker = [source, filters, serial]() -> QPair<QImage, qint64> {
        QImage result = source;
        for (const auto& f : filters) {
            result = Filter::apply(result, f);
        }
        return qMakePair(result, serial);
    };

    m_previewWatcher->setFuture(QtConcurrent::run(worker));
}

void MainWindow::onPreviewFinished()
{
    QPair<QImage, qint64> res = m_previewWatcher->result();
    QImage result = res.first;
    qint64 serial = res.second;
    if (serial != m_previewSerial) {
        return;
    }
    m_resultImage = result;
    m_compareViewer->setResultImage(result);
    m_hasUnsavedChanges = true;
    statusBar()->showMessage(QStringLiteral("预览已实时更新 (异步)"));
    updateActions();
    if (m_previewDirty) {
        m_previewTimer->start();
    }
}

void MainWindow::onToggleGrid(bool show)
{
    m_compareViewer->setGridOverlay(m_filterPanel->gridRows(), m_filterPanel->gridCols(), show);
}

void MainWindow::onZoomIn()
{
    if (m_compareViewer) m_compareViewer->zoomIn();
}

void MainWindow::onZoomOut()
{
    if (m_compareViewer) m_compareViewer->zoomOut();
}

void MainWindow::onFitWindow()
{
    if (m_compareViewer) m_compareViewer->fitToWindow();
}

void MainWindow::onZoomReset()
{
    if (m_compareViewer) m_compareViewer->resetZoom();
}

void MainWindow::onAbout()
{
    QString aboutText = QStringLiteral(
        "<h2>多线程图像滤镜处理器</h2>"
        "<p><b>版本:</b> 1.0</p>"
        "<p>一个功能强大的 C++ / Qt5 多线程图像滤镜处理程序。</p>"
        "<h3>主要特性:</h3>"
        "<ul>"
        "<li>8 种内置滤镜: 灰度、反色、高斯模糊、Sobel 边缘、浮雕、油画、锐化、复古棕</li>"
        "<li>滤镜链支持 - 可任意组合多个滤镜</li>"
        "<li>自定义行列网格分区 (1x1 ~ 16x16)</li>"
        "<li>基于 QtConcurrent 的多线程并行处理</li>"
        "<li>实时进度条与剩余时间估算</li>"
        "<li>随时取消正在进行的处理</li>"
        "<li>区域耗时统计和加速比展示</li>"
        "<li>左右分屏 / 切换 对比原图与结果</li>"
        "<li>可调节各滤镜参数 (模糊半径、阈值等)</li>"
        "<li>支持替换原图叠加多轮滤镜</li>"
        "<li>支持 JPG / PNG / BMP 格式读写</li>"
        "</ul>"
        "<p><b>快捷键:</b><br>"
        "Ctrl+O: 打开 | Ctrl+S: 保存 | Ctrl+Enter: 应用滤镜 | Ctrl+P: 预览 | Esc: 取消</p>");
    QMessageBox::about(this, QStringLiteral("关于 多线程图像滤镜处理器"), aboutText);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (!confirmDiscard()) {
        event->ignore();
        return;
    }
    if (m_taskManager->isProcessing()) {
        m_taskManager->cancelProcessing();
    }
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    auto urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString path = urls.first().toLocalFile();
    if (!path.isEmpty()) {
        if (!confirmDiscard()) return;
        loadImageFromFile(path);
    }
}

bool MainWindow::confirmDiscard()
{
    if (!m_hasUnsavedChanges) return true;
    if (m_resultImage.isNull()) return true;
    int ret = QMessageBox::warning(this, QStringLiteral("未保存的修改"),
        QStringLiteral("当前有未保存的修改，是否继续?\n\n"
                       "按 [是] 继续 (会丢失当前结果)\n"
                       "按 [否] 先保存结果"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        QMessageBox::No);
    if (ret == QMessageBox::Cancel) return false;
    if (ret == QMessageBox::No) {
        onSaveAsImage();
    }
    return true;
}
