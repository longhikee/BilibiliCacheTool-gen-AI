#pragma once
#include <QString>

// 外部合成工具（ffmpeg）的可执行文件路径。
// 修改方式二选一：
//   1) 直接改下面的 #define；
//   2) 在 bilibili-tool.pro 加一行：DEFINES += FFMPEG_EXECUTABLE=\\\"C:/tools/ffmpeg.exe\\\"
// 支持相对路径：相对路径会基于本程序 exe 所在目录解析，
// 因此把 ffmpeg.exe 与 bilibili-tool.exe 放在同一目录即可随程序一起分发。
#ifndef FFMPEG_EXECUTABLE
#define FFMPEG_EXECUTABLE "ffmpeg/ffmpeg.exe"
#endif

// 调用外部 command 合成 MP4。
// 参数：解密后的视频 m4s、解密后的音频 m4s、标题、输出目录。
// 返回进程退出码（0 表示成功）。
int buildMp4(const QString &videoM4s, const QString &audioM4s,
             const QString &title, const QString &outDir);
