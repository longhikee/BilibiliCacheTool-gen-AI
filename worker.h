#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <atomic>
#include <QMetaType>

// 单个处理任务
struct Job {
    int row = -1;          // 在表格中的行号
    QString dir;           // 源目录（含 *_0.m4s / *_1.m4s / .video.json）
    QString title;         // 清洗后的标题
    QString outRoot;       // 用户选择的输出根目录
    bool makeMp4 = false;  // 是否生成 MP4
    bool makeMp3 = false;  // 是否生成 MP3
};
Q_DECLARE_METATYPE(Job)
Q_DECLARE_METATYPE(QVector<Job>)

// 后台工作对象：在独立线程中串行消费任务队列
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    void requestStop();

signals:
    void taskStatus(int row, const QString &status);   // 更新状态文字列
    void taskDone(int row, const QString &opTag, bool ok);
    void allFinished();

public slots:
    void runJobs(const QVector<Job> &jobs);

private:
    std::atomic<bool> stopping{false};
};
