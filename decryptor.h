#pragma once
#include <QString>

// 流式解密：跳过 m4s 文件开头的 6 个字节（大文件分块读写，不整文件入内存）
bool decryptM4s(const QString &in, const QString &out);

// 在目录中按后缀查找 m4s 文件（suffix 形如 "_0" / "_1"）
QString findM4s(const QString &dir, const QString &suffix);
