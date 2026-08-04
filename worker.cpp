#include "worker.h"
#include "decryptor.h"
#include "mp4builder.h"
#include <QDir>
#include <QFile>

QString audioSuffix = "-30280";
QString videoSuffix = "-30016";

Worker::Worker(QObject *parent) : QObject(parent) {}

void Worker::requestStop() { stopping = true; }

void Worker::runJobs(const QVector<Job> &jobs)
{
    stopping = false;

    for (const Job &job : jobs) {
        if (stopping) {
            emit taskStatus(job.row, "已停止");
            break;
        }

        // 计算本次操作标签
        QString op;
        if (job.makeMp4) op += "MP4";
        if (job.makeMp3) op += (op.isEmpty() ? "" : ",") + QString("MP3");

        // 1) 解密
        emit taskStatus(job.row, "解密中");
        QString videoSrc = findM4s(job.dir, videoSuffix);
        QString audioSrc = findM4s(job.dir, audioSuffix);
        if (videoSrc.isEmpty() || audioSrc.isEmpty()) {
            emit taskStatus(job.row, "失败");
            emit taskDone(job.row, op, false);
            continue;
        }

        QString tempDir = QDir::temp().absoluteFilePath(QString("m4s_tool_%1").arg(job.row));
        QDir().mkpath(tempDir);
        QString videoDec = QDir(tempDir).filePath(job.title + "_v.m4s");
        QString audioDec = QDir(tempDir).filePath(job.title + "_a.m4s");
        if (!decryptM4s(videoSrc, videoDec) || !decryptM4s(audioSrc, audioDec)) {
            emit taskStatus(job.row, "失败");
            emit taskDone(job.row, op, false);
            QDir(tempDir).removeRecursively();
            continue;
        }

        bool ok = true;

        // 2) 生成 MP4（耗时，同步阻塞在后台线程）
        if (job.makeMp4) {
            emit taskStatus(job.row, "合成MP4中");
            int rc = buildMp4(videoDec, audioDec, job.title, job.outRoot);
            if (rc != 0) {
                emit taskStatus(job.row, "失败");
                ok = false;
            }
        }

        // 3) 生成 MP3（解密后的音频直接复制/重命名为 title.mp3）
        if (ok && job.makeMp3) {
            emit taskStatus(job.row, "生成MP3中");
            QString mp3 = QDir(job.outRoot).filePath(job.title + ".mp3");
            QFile::remove(mp3);
            if (!QFile::copy(audioDec, mp3)) {
                emit taskStatus(job.row, "失败");
                ok = false;
            }
        }

        if (ok) emit taskStatus(job.row, "完成");
        emit taskDone(job.row, op, ok);

        // 清理临时解密文件
        QDir(tempDir).removeRecursively();
    }

    emit allFinished();
}
