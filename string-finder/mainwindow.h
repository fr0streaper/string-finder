#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QTreeWidgetItem>
#include <QVector>
#include <QMutex>
#include <QFutureWatcher>
#include <QSet>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTreeWidgetItem* addGroup();
    QTreeWidgetItem* addGroupItem(QTreeWidgetItem *group);

    void updateProgressBarAndTreeWidget(QVector<QPair<QString, QVector<QPair<quint64, quint64> > > > &files, quint64 &offset);
    void search(QVector<QPair<QString, QVector<quint64> > > &files);
    QVector<QString> requestFilesRecursive(QString directoryPath);
    QVector<QVector<QString> > distributeFiles(QVector<QString> &filePaths);
    quint64 trigramCharactersToNumber(QString trigram);
    void calculateTrigrams(QVector<QString> &filePaths);

private slots:
    void selectAndIndexDirectory();
    void searchForString();
    void cancelTrigrams();
    void cancelSearch();

private:
    Ui::MainWindow *ui;
    QString currentDirectory;
    quint64 filesTotal, filesProcessed, filesUnreadable;
    QMutex mutex;

    bool isDirectoryIndexed, trigramsCancelled, searchCancelled;
    QFutureWatcher<void> watcher;
    QVector<QVector<QPair<QString, QVector<quint64> > > > distributedTrigrams;
};

#endif // MAINWINDOW_H
