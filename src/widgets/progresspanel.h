#ifndef PROGRESSPANEL_H
#define PROGRESSPANEL_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include "../threading/taskmanager.h"

class ProgressPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressPanel(QWidget* parent = nullptr);

public slots:
    void setProgress(int percent, qint64 elapsedMs, qint64 remainingMs);
    void setRegionCompleted(int regionIndex, qint64 elapsedMs);
    void setRegionThreadId(int regionIndex, qint64 threadId);
    void showStats(const ProcessingStats& stats);
    void reset();
    void setTotalRegions(int count);

signals:
    void cancelRequested();

private:
    static QString formatTime(qint64 ms);

    QProgressBar* m_progressBar;
    QLabel* m_elapsedLabel;
    QLabel* m_remainingLabel;
    QLabel* m_speedupLabel;
    QLabel* m_totalTimeLabel;
    QPushButton* m_cancelBtn;
    QTableWidget* m_regionTable;
    int m_totalRegions;
};

#endif
