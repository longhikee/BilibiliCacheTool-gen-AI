#include "mp4builder.h"
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

int buildMp4(const QString &videoM4s, const QString &audioM4s,
             const QString &title, const QString &outDir)
{
    // 解析 ffmpeg 可执行文件路径：相对路径基于本程序 exe 所在目录
    QString cmd = FFMPEG_EXECUTABLE;
    QFileInfo fi(cmd);
    if (fi.isRelative())
        cmd = QDir(QCoreApplication::applicationDirPath()).filePath(cmd);

    QString dst = QDir(outDir).filePath(title + ".mp4");
    QStringList args{ "-y", "-i", videoM4s, "-i", audioM4s, "-c", "copy", dst };
    return QProcess::execute(cmd, args);
}
