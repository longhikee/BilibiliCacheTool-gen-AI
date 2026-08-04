QT       += widgets
CONFIG   += c++17
TARGET    = bilibili-tool
TEMPLATE  = app

# ============================================================
# 部署：生成可在未安装 Qt 的 Windows PC 上运行的程序
# 1. 用 Release 模式编译（不要用 Debug，否则需要 Qt 调试版 DLL 和
#    MSVC/MinGW 调试运行库，无法简单分发）。
# 2. 编译后 exe 会输出到 DESTDIR 指定的目录（见下）。
# 3. 打开 Qt 自带的 “Qt 5.15.2 (MinGW 64-bit)” 命令行，进入该目录执行：
#       windeployqt bilibili-tool.exe
#    它会自动拷贝 Qt 运行所需的 DLL（及 MinGW 运行库 libgcc/libstdc++ 等）。
# 4. 把 ffmpeg.exe 也复制到同一目录（FFMPEG_EXECUTABLE 默认即同目录下的
#    ffmpeg.exe）。最终整个目录拷贝到目标 PC 即可直接双击运行。
# ============================================================

# 编译产物（exe）输出目录，与源码分离，方便整体打包分发
DESTDIR = $$OUT_PWD/build

SOURCES += main.cpp \
           mainwindow.cpp \
           worker.cpp \
           jsonutils.cpp \
           decryptor.cpp \
           mp4builder.cpp

HEADERS += mainwindow.h \
           worker.h \
           jsonutils.h \
           decryptor.h \
           mp4builder.h
