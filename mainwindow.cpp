#include "mainwindow.h"
#include "decryptor.h"
#include "jsonutils.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMetaObject>
#include <QVector>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Bilibili缓存工具");
    resize(950, 520);
    setAcceptDrops(true);

    // 整体样式表（QSS）
    setStyleSheet(R"(
        MainWindow, QWidget {
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
            font-size: 13px;
        }
        QLabel { color: #2f3640; }
        QLineEdit {
            border: 1px solid #d2d6de;
            border-radius: 6px;
            padding: 6px 9px;
            background: #ffffff;
            selection-background-color: #4a90e2;
        }
        QLineEdit:focus { border: 1px solid #4a90e2; }
        QPushButton {
            border: 1px solid #c7ccd8;
            border-radius: 6px;
            padding: 7px 16px;
            background: #eef0f4;
            color: #2f3640;
        }
        QPushButton:hover { background: #e2e6ee; border-color: #b5bcc8; }
        QPushButton:pressed { background: #d4d9e2; border-color: #aab2c0; }
        QPushButton:disabled { background: #f3f4f7; color: #b2bec3; border-color: #e1e4ea; }
        QTableWidget {
            border: 1px solid #e1e4ea;
            border-radius: 8px;
            gridline-color: #eef1f5;
            background: #ffffff;
            alternate-background-color: #f7f9fc;
            selection-background-color: #d6e4f0;
        }
        QHeaderView::section {
            background: #f0f2f7;
            color: #2f3640;
            border: none;
            border-bottom: 1px solid #e1e4ea;
            padding: 8px;
            font-weight: bold;
        }
        QTableWidget::item { padding: 5px; }
        QTableWidget::item:selected { background: #d6e4f0; color: #2f3640; }
        QCheckBox { spacing: 6px; color: #2f3640; }
        QCheckBox::indicator { width: 16px; height: 16px; }
    )");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    central->setStyleSheet("background: #f5f6fa;");
    QVBoxLayout *root = new QVBoxLayout(central);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(14);

    // 输出目录行
    QHBoxLayout *outLayout = new QHBoxLayout();
    outLayout->setSpacing(8);
    outLayout->addWidget(new QLabel("输出目录："));
    outputEdit = new QLineEdit(this);
    outLayout->addWidget(outputEdit, 1);
    browseBtn = new QPushButton("浏览", this);
    outLayout->addWidget(browseBtn);
    root->addLayout(outLayout);

    // 任务表格
    table = new QTableWidget(0, 5, this);
    table->setHorizontalHeaderLabels(QStringList()
        << "选择" << "目录" << "标题" << "状态" << "操作");
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 列宽优化：选择/状态/操作固定窄宽，目录给较大初始宽，标题弹性占满剩余
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);   // 选择
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive); // 目录
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // 标题
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);   // 状态
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);   // 操作
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 300);
    table->setColumnWidth(3, 100);
    table->setColumnWidth(4, 80);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    root->addWidget(table, 1);

    // 按钮行
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);
    selectAllChk = new QCheckBox("全选", this);
    const QString primaryBtnStyle =
        "QPushButton {"
        "  border: 1px solid #2f6fb0; border-radius: 6px; padding: 7px 16px;"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #3f86c7,stop:1 #2f6fb0);"
        "  color: #ffffff; font-weight: bold;"
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #4a92cf,stop:1 #3a7abd); }"
        "QPushButton:disabled { background: #c6dcef; color: #6f93b8; }";
    addBtn = new QPushButton("添加目录", this);
    addBtn->setStyleSheet(primaryBtnStyle);
    genMp4Btn = new QPushButton("生成 MP4", this);
    genMp4Btn->setStyleSheet(primaryBtnStyle);
    genMp3Btn = new QPushButton("生成 MP3", this);
    genMp3Btn->setStyleSheet(primaryBtnStyle);
    stopBtn = new QPushButton("停止", this);
    stopBtn->setStyleSheet(
        "QPushButton {"
        "  border: 1px solid #c0392b; border-radius: 6px; padding: 7px 16px;"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #e74c3c,stop:1 #c0392b);"
        "  color: #ffffff; font-weight: bold;"
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #ec5e4f,stop:1 #cd4436); }"
        "QPushButton:disabled { background: #f0b4ae; color: #ffffff; }");
    stopBtn->setEnabled(false);
    btnLayout->addWidget(selectAllChk);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(genMp4Btn);
    btnLayout->addWidget(genMp3Btn);
    btnLayout->addWidget(stopBtn);
    btnLayout->addStretch(1);
    root->addLayout(btnLayout);

    // 后台工作线程
    thread = new QThread(this);
    worker = new Worker();
    worker->moveToThread(thread);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Worker::taskStatus, this, &MainWindow::onTaskStatus);
    connect(worker, &Worker::taskDone, this, &MainWindow::onTaskDone);
    connect(worker, &Worker::allFinished, this, &MainWindow::onAllFinished);
    thread->start();

    // 信号连接
    connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddDirs);
    connect(genMp4Btn, &QPushButton::clicked, this, &MainWindow::onGenerateMp4);
    connect(genMp3Btn, &QPushButton::clicked, this, &MainWindow::onGenerateMp3);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(selectAllChk, &QCheckBox::toggled, this, &MainWindow::onSelectAll);
}

MainWindow::~MainWindow()
{
    if (worker) worker->requestStop();
    if (thread) {
        thread->quit();
        thread->wait();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl &u : urls) {
        QString p = u.toLocalFile();
        QFileInfo fi(p);
        if (fi.isDir())
            addDirectory(p);
    }
    event->acceptProposedAction();
}

void MainWindow::onSelectAll(bool checked)
{
    for (int r = 0; r < table->rowCount(); ++r) {
        QTableWidgetItem *it = table->item(r, 0);
        if (it) it->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
}

void MainWindow::addDirectory(const QString &dir)
{
    QDir d(dir);
    if (!d.exists())
        return;

    // if (!QFile::exists(d.filePath(VIDEO_INFO_FILENAME)) ||
    //     findM4s(dir, "_0").isEmpty() ||
    //     findM4s(dir, "_1").isEmpty()) {
    if (!QFile::exists(d.filePath(VIDEO_INFO_FILENAME))) {
        QMessageBox::warning(this,
                             "错误",
                             QString("目录缺少必要文件 %1：\n%2").arg(VIDEO_INFO_FILENAME).arg(dir));
        return;
    }

    // 去重
    for (int r = 0; r < table->rowCount(); ++r) {
        if (table->item(r, 1)->text() == dir)
            return;
    }

    QString raw = readTitleFromJson(dir);
    QString title = sanitizeTitle(raw.isEmpty() ? d.dirName() : raw);

    int r = table->rowCount();
    table->insertRow(r);

    QTableWidgetItem *chk = new QTableWidgetItem();
    chk->setFlags(chk->flags() | Qt::ItemIsUserCheckable);
    chk->setCheckState(Qt::Unchecked);
    chk->setTextAlignment(Qt::AlignCenter);
    table->setItem(r, 0, chk);

    QStringList cells = {dir, title, "待处理", ""};
    for (int c = 1; c <= 4; ++c) {
        QTableWidgetItem *it = new QTableWidgetItem(cells[c - 1]);
        it->setTextAlignment(Qt::AlignCenter);
        it->setFlags(it->flags() & ~Qt::ItemIsEditable);
        table->setItem(r, c, it);
    }
}

void MainWindow::onAddDirs()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setWindowTitle("选择目录（可多选）");
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    if (dialog.exec() == QDialog::Accepted) {
        for (const QString &d : dialog.selectedFiles())
            addDirectory(d);
    }
}

void MainWindow::onBrowseOutput()
{
    QString d = QFileDialog::getExistingDirectory(this, "选择输出根目录");
    if (!d.isEmpty()) {
        outputRoot = d;
        outputEdit->setText(d);
    }
}

void MainWindow::onGenerateMp4()
{
    startGeneration(true, false);
}

void MainWindow::onGenerateMp3()
{
    startGeneration(false, true);
}

void MainWindow::onStop()
{
    worker->requestStop();
}

bool MainWindow::isRowSelected(int row) const
{
    QTableWidgetItem *it = table->item(row, 0);
    return it && it->checkState() == Qt::Checked;
}

void MainWindow::startGeneration(bool mp4, bool mp3)
{
    if (outputRoot.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择输出目录");
        return;
    }

    QVector<Job> jobs;
    for (int r = 0; r < table->rowCount(); ++r) {
        if (!isRowSelected(r))
            continue;

        Job job;
        job.row = r;
        job.dir = table->item(r, 1)->text();
        job.title = table->item(r, 2)->text();
        job.outRoot = outputRoot;
        job.makeMp4 = mp4;
        job.makeMp3 = mp3;
        jobs.append(job);

        QString op;
        if (mp4) op = "MP4";
        if (mp3) {
            if (!op.isEmpty()) op += ",";
            op += "MP3";
        }
        table->item(r, 4)->setText(op);
        table->item(r, 3)->setText("排队中");
    }

    if (jobs.isEmpty()) {
        QMessageBox::information(this, "提示", "请先勾选要处理的任务");
        return;
    }

    setUiEnabled(false);
    QMetaObject::invokeMethod(worker, "runJobs",
                              Qt::QueuedConnection,
                              Q_ARG(QVector<Job>, jobs));
}

void MainWindow::setUiEnabled(bool enabled)
{
    addBtn->setEnabled(enabled);
    browseBtn->setEnabled(enabled);
    genMp4Btn->setEnabled(enabled);
    genMp3Btn->setEnabled(enabled);
    outputEdit->setEnabled(enabled);
    selectAllChk->setEnabled(enabled);
    stopBtn->setEnabled(!enabled);
}

void MainWindow::onTaskStatus(int row, const QString &status)
{
    if (row >= 0 && row < table->rowCount())
        table->item(row, 3)->setText(status);
}

void MainWindow::onTaskDone(int row, const QString &opTag, bool ok)
{
    Q_UNUSED(row);
    Q_UNUSED(opTag);
    Q_UNUSED(ok);
}

void MainWindow::onAllFinished()
{
    setUiEnabled(true);
    QMessageBox::information(this, "完成", "所有任务处理完毕");
}
