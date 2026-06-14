#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QDockWidget>
#include <QTimer>
#include <QFutureWatcher>
#include "threading/taskmanager.h"
#include "widgets/filtercontrolspanel.h"
#include "widgets/progresspanel.h"
#include "widgets/compareviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenImage();
    void onSaveImage();
    void onSaveAsImage();
    void onReplaceOriginal();
    void onApplyFilter();
    void onPreviewFilter();
    void onProcessingFinished(const QImage& result, const ProcessingStats& stats);
    void onProcessingCancelled();
    void onCancelProcessing();
    void onProgressChanged(int percent, qint64 elapsed, qint64 remaining);
    void onRegionCompleted(int idx, qint64 ms);
    void onFilterChainChanged();
    void onParametersChanged();
    void onToggleGrid(bool show);
    void onZoomIn();
    void onZoomOut();
    void onFitWindow();
    void onZoomReset();
    void onAbout();
    void onDebouncePreview();
    void onPreviewFinished();

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupDockWidgets();
    void setupConnections();
    void updateActions();
    bool loadImageFromFile(const QString& path);
    bool saveImageToFile(const QString& path, const QImage& image);
    QString getFileFilters() const;
    bool confirmDiscard();

    QImage m_originalImage;
    QImage m_resultImage;
    QString m_currentFilePath;
    bool m_hasUnsavedChanges;

    CompareViewer* m_compareViewer;

    QDockWidget* m_filterDock;
    FilterControlsPanel* m_filterPanel;

    QDockWidget* m_progressDock;
    ProgressPanel* m_progressPanel;

    QToolBar* m_mainToolBar;

    QAction* m_actionOpen;
    QAction* m_actionSave;
    QAction* m_actionSaveAs;
    QAction* m_actionReplace;
    QAction* m_actionExit;

    QAction* m_actionApply;
    QAction* m_actionPreview;
    QAction* m_actionCancel;
    QAction* m_actionToggleGrid;

    QAction* m_actionZoomIn;
    QAction* m_actionZoomOut;
    QAction* m_actionFit;
    QAction* m_actionResetZoom;

    QAction* m_actionAbout;
    QAction* m_actionAboutQt;

    TaskManager* m_taskManager;
    QImage m_previewImage;
    QTimer* m_previewTimer;
    QFutureWatcher<QPair<QImage, qint64>>* m_previewWatcher;
    bool m_previewDirty;
    qint64 m_previewSerial;
};

#endif
