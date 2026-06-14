#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QRect>
#include <QElapsedTimer>
#include <QAtomicInt>
#include <QMutex>
#include <QFuture>
#include <QFutureWatcher>
#include "../filters/filter.h"

struct RegionResult {
    int regionIndex;
    QRect region;
    QImage processed;
    qint64 elapsedMs;
    qint64 threadId;
};

struct ProcessingStats {
    int gridRows;
    int gridCols;
    qint64 totalParallelMs;
    qint64 estimatedSequentialMs;
    QList<RegionResult> regionResults;
    bool wasCancelled;

    double speedup() const {
        return totalParallelMs > 0 ? static_cast<double>(estimatedSequentialMs) / totalParallelMs : 0.0;
    }
};

class TaskManager : public QObject
{
    Q_OBJECT

public:
    explicit TaskManager(QObject* parent = nullptr);
    ~TaskManager();

    void setSourceImage(const QImage& image);
    void setGridSize(int rows, int cols);
    void setFilterChain(const QList<FilterParams>& filters);

    void startProcessing();
    void cancelProcessing();

    bool isProcessing() const;
    ProcessingStats lastStats() const;

    static int getKernelRadiusForFilterChain(const QList<FilterParams>& filters);

signals:
    void progressChanged(int percent, qint64 elapsedMs, qint64 remainingMs);
    void regionCompleted(int regionIndex, qint64 elapsedMs);
    void regionThreadIdReady(int regionIndex, qint64 threadId);
    void processingFinished(const QImage& result, const ProcessingStats& stats);
    void processingCancelled();

private slots:
    void onAllFinished();

private:
    RegionResult processRegion(int regionIndex, const QRect& region, const QList<FilterParams>& filters);
    QList<QRect> computeGridRegions() const;
    qint64 estimateSequentialTime() const;

    QImage m_source;
    int m_gridRows;
    int m_gridCols;
    QList<FilterParams> m_filters;

    QAtomicInt m_completedCount;
    QAtomicInt m_cancelled;
    int m_totalRegions;

    QList<RegionResult> m_regionResults;
    QElapsedTimer m_timer;
    mutable QMutex m_mutex;

    QFutureWatcher<RegionResult>* m_watcher;
    QFuture<RegionResult> m_future;

    ProcessingStats m_lastStats;
};

#endif
