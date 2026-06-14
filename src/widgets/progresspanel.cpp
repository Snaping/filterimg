#include "progresspanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QBrush>
#include <QColor>

ProgressPanel::ProgressPanel(QWidget* parent)
    : QWidget(parent)
    , m_totalRegions(0)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    QGroupBox* progressGroup = new QGroupBox(QStringLiteral("处理进度"), this);
    QVBoxLayout* progLayout = new QVBoxLayout(progressGroup);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat(QStringLiteral("%p%"));
    progLayout->addWidget(m_progressBar);

    QHBoxLayout* timeLayout = new QHBoxLayout();
    m_elapsedLabel = new QLabel(QStringLiteral("已耗时: 00:00"), this);
    m_remainingLabel = new QLabel(QStringLiteral("剩余: --:--"), this);
    timeLayout->addWidget(m_elapsedLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(m_remainingLabel);
    progLayout->addLayout(timeLayout);

    m_cancelBtn = new QPushButton(QStringLiteral("取消处理"), this);
    m_cancelBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    connect(m_cancelBtn, &QPushButton::clicked, this, &ProgressPanel::cancelRequested);
    progLayout->addWidget(m_cancelBtn);

    mainLayout->addWidget(progressGroup);

    QGroupBox* statsGroup = new QGroupBox(QStringLiteral("性能统计"), this);
    QVBoxLayout* statsLayout = new QVBoxLayout(statsGroup);

    QHBoxLayout* summaryLayout = new QHBoxLayout();
    m_speedupLabel = new QLabel(QStringLiteral("加速比: --"), this);
    QFont font = m_speedupLabel->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 1);
    m_speedupLabel->setFont(font);
    m_speedupLabel->setStyleSheet(QStringLiteral("color: #2d7ff9;"));
    m_totalTimeLabel = new QLabel(QStringLiteral("总耗时: --"), this);
    summaryLayout->addWidget(m_speedupLabel);
    summaryLayout->addStretch();
    summaryLayout->addWidget(m_totalTimeLabel);
    statsLayout->addLayout(summaryLayout);

    m_regionTable = new QTableWidget(this);
    m_regionTable->setColumnCount(4);
    m_regionTable->setHorizontalHeaderLabels(
        QStringList() << QStringLiteral("区域") << QStringLiteral("耗时(ms)")
                     << QStringLiteral("线程ID") << QStringLiteral("状态"));
    m_regionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_regionTable->verticalHeader()->setVisible(false);
    m_regionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_regionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    statsLayout->addWidget(m_regionTable);

    mainLayout->addWidget(statsGroup);
    mainLayout->addStretch();
}

QString ProgressPanel::formatTime(qint64 ms)
{
    if (ms < 0) ms = 0;
    qint64 totalSec = ms / 1000;
    qint64 h = totalSec / 3600;
    qint64 m = (totalSec % 3600) / 60;
    qint64 s = totalSec % 60;
    qint64 msRemain = ms % 1000;
    if (h > 0) {
        return QStringLiteral("%1:%2:%3.%4")
            .arg(h, 2, 10, QLatin1Char('0'))
            .arg(m, 2, 10, QLatin1Char('0'))
            .arg(s, 2, 10, QLatin1Char('0'))
            .arg(msRemain / 100, 1, 10);
    }
    return QStringLiteral("%1:%2.%3")
        .arg(m, 2, 10, QLatin1Char('0'))
        .arg(s, 2, 10, QLatin1Char('0'))
        .arg(msRemain / 100, 1, 10);
}

void ProgressPanel::setProgress(int percent, qint64 elapsedMs, qint64 remainingMs)
{
    m_progressBar->setValue(percent);
    m_elapsedLabel->setText(QStringLiteral("已耗时: %1").arg(formatTime(elapsedMs)));
    if (remainingMs > 0) {
        m_remainingLabel->setText(QStringLiteral("剩余: ~%1").arg(formatTime(remainingMs)));
    } else if (percent >= 100) {
        m_remainingLabel->setText(QStringLiteral("已完成"));
    } else {
        m_remainingLabel->setText(QStringLiteral("剩余: 估算中..."));
    }
}

void ProgressPanel::setRegionCompleted(int regionIndex, qint64 elapsedMs)
{
    if (regionIndex < 0 || regionIndex >= m_regionTable->rowCount()) return;
    QTableWidgetItem* timeItem = m_regionTable->item(regionIndex, 1);
    if (timeItem) {
        timeItem->setText(QString::number(elapsedMs));
    }
    QTableWidgetItem* statusItem = m_regionTable->item(regionIndex, 3);
    if (statusItem) {
        statusItem->setText(QStringLiteral("✓ 完成"));
        statusItem->setForeground(QBrush(QColor(0, 160, 0)));
    }
    m_regionTable->item(regionIndex, 1)->setBackground(QBrush(QColor(230, 255, 230)));
}

void ProgressPanel::setRegionThreadId(int regionIndex, qint64 threadId)
{
    if (regionIndex < 0 || regionIndex >= m_regionTable->rowCount()) return;
    QTableWidgetItem* threadItem = m_regionTable->item(regionIndex, 2);
    if (threadItem) {
        threadItem->setText(QString::number(threadId));
    }
}

void ProgressPanel::showStats(const ProcessingStats& stats)
{
    setTotalRegions(stats.regionResults.size());
    for (int i = 0; i < stats.regionResults.size(); ++i) {
        const auto& r = stats.regionResults[i];
        if (i < m_regionTable->rowCount()) {
            m_regionTable->item(i, 1)->setText(QString::number(r.elapsedMs));
            m_regionTable->item(i, 2)->setText(QString::number(r.threadId));
            m_regionTable->item(i, 3)->setText(QStringLiteral("✓ 完成"));
            m_regionTable->item(i, 3)->setForeground(QBrush(QColor(0, 160, 0)));
        }
    }

    if (stats.totalParallelMs > 0) {
        double sp = stats.speedup();
        m_speedupLabel->setText(QStringLiteral("加速比: %1x").arg(sp, 0, 'f', 2));
        m_totalTimeLabel->setText(QStringLiteral("并行: %1 / 串行估计: %2")
            .arg(formatTime(stats.totalParallelMs))
            .arg(formatTime(stats.estimatedSequentialMs)));
    }

    setProgress(100, stats.totalParallelMs, 0);
}

void ProgressPanel::reset()
{
    m_progressBar->setValue(0);
    m_elapsedLabel->setText(QStringLiteral("已耗时: 00:00"));
    m_remainingLabel->setText(QStringLiteral("剩余: --:--"));
    m_speedupLabel->setText(QStringLiteral("加速比: --"));
    m_totalTimeLabel->setText(QStringLiteral("总耗时: --"));
    m_regionTable->setRowCount(0);
    m_totalRegions = 0;
}

void ProgressPanel::setTotalRegions(int count)
{
    m_totalRegions = count;
    m_regionTable->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        QTableWidgetItem* idxItem = new QTableWidgetItem(QStringLiteral("#%1").arg(i + 1));
        idxItem->setTextAlignment(Qt::AlignCenter);
        m_regionTable->setItem(i, 0, idxItem);

        QTableWidgetItem* timeItem = new QTableWidgetItem(QStringLiteral("--"));
        timeItem->setTextAlignment(Qt::AlignCenter);
        m_regionTable->setItem(i, 1, timeItem);

        QTableWidgetItem* threadItem = new QTableWidgetItem(QStringLiteral("--"));
        threadItem->setTextAlignment(Qt::AlignCenter);
        m_regionTable->setItem(i, 2, threadItem);

        QTableWidgetItem* statusItem = new QTableWidgetItem(QStringLiteral("等待中"));
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setForeground(QBrush(QColor(128, 128, 128)));
        m_regionTable->setItem(i, 3, statusItem);
    }
}
