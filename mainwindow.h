#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QThread>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include "worker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onAddDirs();
    void onBrowseOutput();
    void onGenerateMp4();
    void onGenerateMp3();
    void onStop();
    void onSelectAll(bool checked);
    void onTaskStatus(int row, const QString &status);
    void onTaskDone(int row, const QString &opTag, bool ok);
    void onAllFinished();

private:
    QTableWidget *table = nullptr;
    QLineEdit *outputEdit = nullptr;
    QPushButton *addBtn = nullptr;
    QPushButton *browseBtn = nullptr;
    QPushButton *genMp4Btn = nullptr;
    QPushButton *genMp3Btn = nullptr;
    QPushButton *stopBtn = nullptr;
    QCheckBox *selectAllChk = nullptr;

    QThread *thread = nullptr;
    Worker *worker = nullptr;
    QString outputRoot;

    void addDirectory(const QString &dir);
    void startGeneration(bool mp4, bool mp3);
    void setUiEnabled(bool enabled);
    bool isRowSelected(int row) const;
};
