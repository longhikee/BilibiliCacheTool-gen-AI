#pragma once
#include <QString>

// 元数据文件名（含 title 字段），如需修改可在此直接改，
// 或通过编译选项 -DVIDEO_INFO_FILENAME="\"xxx.json\"" 覆盖
#ifndef VIDEO_INFO_FILENAME
#define VIDEO_INFO_FILENAME "videoInfo.json"
#endif

// 读取目录下的 .video.json，返回其中的 title 字段（空则失败）
QString readTitleFromJson(const QString &dir);

// 清洗标题，移除 Windows 文件名非法字符： \ / : * ? " < > |
QString sanitizeTitle(const QString &raw);
