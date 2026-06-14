#include "taskmanager.h"
#include <QtConcurrent>
#include <QThreadPool>
#include <QPainter>
#include <QThread>
#include <QVector>
#include <functional>
#include <cmath>

TaskManager::TaskManager(QObject* parent)
    : QObject(parent)
    , m_gridRows(2)
    , m_gridCols(2)
    , m_completedCount(0)
    , m_cancelled(0)
    , m_totalRegions(0)
    , m_watcher(new QFutureWatcher<RegionResult>(this))
{
    connect(m_watcher, &QFutureWatcher<RegionResult>::finished, this, &TaskManager::onAllFinished);
    connect(m_watcher, &QFutureWatcher<RegionResult>::progressRangeChanged, this,
        [this](int min, int max) {
            Q_UNUSED(min);
            Q_UNUSED(max);
        });
    connect(m_watcher, &QFutureWatcher<RegionResult>::progressValueChanged, this,
        [this](int value) {
            if (m_totalRegions <= 0) return;
            int percent = value * 100 / m_totalRegions;
            qint64 elapsed = m_timer.elapsed();
            qint64 remaining = 0;
            if (value > 0 && value < m_totalRegions) {
                double perRegion = static_cast<double>(elapsed) / value;
                remaining = static_cast<qint64>(perRegion * (m_totalRegions - value));
            }
            emit progressChanged(percent, elapsed, remaining);
        });
    connect(m_watcher, &QFutureWatcher<RegionResult>::resultReadyAt, this,
        [this](int index) {
            RegionResult r = m_watcher->resultAt(index);
            {
                QMutexLocker locker(&m_mutex);
                m_regionResults.append(r);
                std::sort(m_regionResults.begin(), m_regionResults.end(),
                    [](const RegionResult& a, const RegionResult& b) {
                        return a.regionIndex < b.regionIndex;
                    });
            }
            emit regionCompleted(r.regionIndex, r.elapsedMs);
            emit regionThreadIdReady(r.regionIndex, r.threadId);
        });
}

TaskManager::~TaskManager()
{
    cancelProcessing();
}

void TaskManager::setSourceImage(const QImage& image)
{
    m_source = image;
}

void TaskManager::setGridSize(int rows, int cols)
{
    m_gridRows = qMax(1, rows);
    m_gridCols = qMax(1, cols);
}

void TaskManager::setFilterChain(const QList<FilterParams>& filters)
{
    m_filters = filters;
}

bool TaskManager::isProcessing() const
{
    return m_watcher->isRunning();
}

ProcessingStats TaskManager::lastStats() const
{
    QMutexLocker locker(&m_mutex);
    return m_lastStats;
}

int TaskManager::getKernelRadiusForFilterChain(const QList<FilterParams>& filters)
{
    int radius = 0;
    for (const auto& f : filters) {
        int r = 0;
        switch (f.type) {
        case FilterTypes::GaussianBlur:
            r = f.params.value("radius", 3).toInt();
            break;
        case FilterTypes::SobelEdge:
        case FilterTypes::Emboss:
        case FilterTypes::Sharpen:
            r = 1;
            break;
        case FilterTypes::OilPaint:
            r = f.params.value("radius", 3).toInt();
            break;
        default:
            r = 0;
        }
        radius = qMax(radius, r);
    }
    return radius;
}

QList<QRect> TaskManager::computeGridRegions() const
{
    QList<QRect> regions;
    if (m_source.isNull()) return regions;

    int w = m_source.width();
    int h = m_source.height();
    int cellW = w / m_gridCols;
    int cellH = h / m_gridRows;

    for (int r = 0; r < m_gridRows; ++r) {
        for (int c = 0; c < m_gridCols; ++c) {
            int x = c * cellW;
            int y = r * cellH;
            int cw = (c == m_gridCols - 1) ? (w - x) : cellW;
            int ch = (r == m_gridRows - 1) ? (h - y) : cellH;
            regions.append(QRect(x, y, cw, ch));
        }
    }
    return regions;
}

qint64 TaskManager::estimateSequentialTime() const
{
    QMutexLocker locker(&m_mutex);
    qint64 total = 0;
    for (const auto& r : m_regionResults) {
        total += r.elapsedMs;
    }
    return total;
}

RegionResult TaskManager::processRegion(int regionIndex, const QRect& region, const QList<FilterParams>& filters)
{
    QElapsedTimer t;
    t.start();
    RegionResult result;
    result.regionIndex = regionIndex;
    result.region = region;
    result.threadId = reinterpret_cast<qint64>(QThread::currentThreadId());

    if (m_cancelled.loadAcquire()) {
        result.elapsedMs = 0;
        return result;
    }

    int kernelRadius = getKernelRadiusForFilterChain(filters);
    QRect expanded = Filter::expandRectForKernel(region, kernelRadius, m_source.size());
    QImage subImg = m_source.copy(expanded);

    QImage current = subImg;
    for (const auto& f : filters) {
        if (m_cancelled.loadAcquire()) break;
        current = Filter::apply(current, f);
    }

    if (m_cancelled.loadAcquire()) {
        result.elapsedMs = t.elapsed();
        return result;
    }

    QRect localRegion(region.topLeft() - expanded.topLeft(), region.size());
    result.processed = current.copy(localRegion);
    result.elapsedMs = t.elapsed();
    return result;
}

void TaskManager::startProcessing()
{
    if (m_source.isNull() || m_filters.isEmpty()) return;
    if (m_watcher->isRunning()) return;

    m_completedCount.storeRelease(0);
    m_cancelled.storeRelease(0);
    m_regionResults.clear();

    QList<QRect> regions = computeGridRegions();
    m_totalRegions = regions.size();
    if (m_totalRegions == 0) return;

    m_timer.start();

    QVector<int> indices;
    indices.reserve(regions.size());
    for (int i = 0; i < regions.size(); ++i) indices.push_back(i);

    std::function<RegionResult(const int&)> mapFunc = [this, regions](const int& idx) -> RegionResult {
        if (idx < 0 || idx >= regions.size()) {
            RegionResult dummy;
            dummy.regionIndex = idx;
            dummy.elapsedMs = 0;
            dummy.threadId = 0;
            return dummy;
        }
        const QRect& r = regions[idx];
        if (m_cancelled.loadAcquire()) {
            RegionResult res;
            res.regionIndex = idx;
            res.region = r;
            res.elapsedMs = 0;
            res.threadId = 0;
            return res;
        }
        RegionResult res = processRegion(idx, r, m_filters);
        m_completedCount.fetchAndAddRelaxed(1);
        return res;
    };

    m_future = QtConcurrent::mapped(indices, mapFunc);
    m_watcher->setFuture(m_future);
}

void TaskManager::cancelProcessing()
{
    if (!m_watcher->isRunning()) return;
    m_cancelled.storeRelease(1);
    m_future.cancel();
    m_future.waitForFinished();
}

void TaskManager::onAllFinished()
{
    qint64 totalMs = m_timer.elapsed();

    if (m_cancelled.loadAcquire()) {
        emit processingCancelled();
        return;
    }

    QMutexLocker locker(&m_mutex);

    ProcessingStats stats;
    stats.gridRows = m_gridRows;
    stats.gridCols = m_gridCols;
    stats.totalParallelMs = totalMs;
    stats.regionResults = m_regionResults;
    stats.wasCancelled = false;
    stats.estimatedSequentialMs = 0;
    for (const auto& r : m_regionResults) {
        stats.estimatedSequentialMs += r.elapsedMs;
    }

    QImage result(m_source.size(), QImage::Format_RGB32);
    result.fill(Qt::black);
    QPainter p(&result);
    for (const auto& r : m_regionResults) {
        if (!r.processed.isNull()) {
            p.drawImage(r.region.topLeft(), r.processed);
        }
    }
    p.end();

    m_lastStats = stats;
    locker.unlock();

    emit progressChanged(100, totalMs, 0);
    emit processingFinished(result, stats);
}
