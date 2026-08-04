#include "decryptor.h"
#include <QFile>
#include <QDir>

bool decryptM4s(const QString &in, const QString &out)
{
    QFile fin(in), fout(out);
    if (!fin.open(QIODevice::ReadOnly) || !fout.open(QIODevice::WriteOnly))
        return false;
    if (!fin.seek(9)) // 跳过开头的 9 个字节
        return false;

    constexpr qint64 BUF = 65536;
    char buf[BUF];
    qint64 n;
    while ((n = fin.read(buf, BUF)) > 0) {
        if (fout.write(buf, n) != n)
            return false;
    }
    return true;
}

QString findM4s(const QString &dir, const QString &suffix)
{
    QDir d(dir);
    QStringList files = d.entryList(QStringList() << ("*" + suffix + ".m4s"), QDir::Files);
    if (files.isEmpty())
        return QString();
    return d.filePath(files.first());
}
