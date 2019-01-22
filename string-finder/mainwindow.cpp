#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QDebug>
#include <QTextStream>
#include <QProgressDialog>
#include <QtConcurrent>

//total number of characters in the character set
const quint64 ASIZE = (1 << 16);

//[DEBUG] trigram to characters: (char)(trig / (ASIZE * ASIZE)) << (char)((trig % (ASIZE * ASIZE)) / ASIZE) << (char)(trig % ASIZE)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    filesTotal(0),
    filesProcessed(0),
    filesUnreadable(0),
    searchCancelled(false),
    trigramsCancelled(false),
    isDirectoryIndexed(false)
{
    ui->setupUi(this);

    setWindowTitle("String finder");
    ui->treeWidget->setHeaderLabel("Matches found in current directory");
    ui->statusBar->showMessage("Choose a directory to find your string");

    connect(ui->actionSearch, &QAction::triggered, this, &MainWindow::searchForString);
    connect(ui->actionSelect_and_index_directory, &QAction::triggered, this, &MainWindow::selectAndIndexDirectory);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTreeWidgetItem* MainWindow::addGroup()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
    ui->treeWidget->addTopLevelItem(item);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    return item;
}

QTreeWidgetItem* MainWindow::addGroupItem(QTreeWidgetItem *group)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    group->addChild(item);
    return item;
}

void MainWindow::updateProgressBarAndTreeWidget(QVector<QPair<QString, QVector<QPair<quint64, quint64> > > > &files, quint64 &offset)
{
    for (quint64 i = offset; i < files.size(); ++i)
    {
        QTreeWidgetItem* group = addGroup();
        group->setText(0, "[" + QString::number(files[i].second.size()) + " match(es)] " + files[i].first);
        //group->setFlags(group->flags() & ~Qt::);

        for (auto match : files[i].second)
        {
            QTreeWidgetItem* item = addGroupItem(group);
            item->setText(0, "Line " + QString::number(match.first) + ", symbol " + QString::number(match.second));
        }
    }
    filesProcessed += files.size() - offset;
    offset = files.size();
    emit watcher.progressValueChanged(filesProcessed);
}

void MainWindow::search(QVector<QPair<QString, QVector<quint64> > > &files)
{
    QString str = ui->searchLineEdit->text();

    //calculate str's trigrams
    QSet<quint64> strTrigramsSet;
    QString currentTrigram = "";
    for (quint8 i = 0; i < str.length(); ++i)
    {
        currentTrigram += str[i];
        if (currentTrigram.size() < 3)
        {
            continue;
        }
        if (currentTrigram.size() > 3)
        {
            currentTrigram = currentTrigram.right(3);
        }

        strTrigramsSet.insert(trigramCharactersToNumber(currentTrigram));

        if (searchCancelled)
        {
            return;
        }
    }
    QVector<quint64> strTrigrams;
    for (quint64 trigram : strTrigramsSet)
    {
        strTrigrams.push_back(trigram);

      if (searchCancelled)
        {
            return;
        }
    }

    qSort(strTrigrams);

    //cycle through files
    QVector<QPair<QString, QVector<QPair<quint64, quint64> > > > processedFiles;
    quint64 offset = 0;
    for (QPair<QString, QVector<quint64> > file : files)
    {
        //check corresponding trigrams
        int strId = 0, fileId = 0;
        while (strId != strTrigrams.size() && fileId != file.second.size())
        {
            if (strTrigrams[strId] == file.second[fileId])
            {
                ++strId;
            }
            ++fileId;
        }

        if (strId != strTrigrams.size())
        {
            continue;
        }

        //search for string if all trigrams are there
        QFile currentFile(currentDirectory + "/" + file.first);
        currentFile.open(QFile::ReadOnly);
        QTextStream reader(&currentFile);

        QVector<QPair<quint64, quint64> > foundMatches;
        QString fileSegment = "";
        quint64 line = 1, symbol = 1;
        while (!reader.atEnd())
        {
            QChar c;
            reader >> c;

            if (c == '\n')
            {
                ++line;
                symbol = 1;
                fileSegment = "";
                continue;
            }

            fileSegment += c;
            if (fileSegment.size() >= str.size() && fileSegment.right(1) == str.right(1))
            {
                if (fileSegment.right(str.size()) == str)
                {
                    foundMatches.push_back({ line, symbol - str.size() + 1 });
                }
            }

            if (fileSegment.size() > 2 * str.size())
            {
                fileSegment = fileSegment.right(str.size());
            }

            ++symbol;
        }
        currentFile.close();

        if (foundMatches.empty())
        {
            continue;
        }

        processedFiles.push_back({ file.first, foundMatches });
        if (mutex.try_lock())
        {
            updateProgressBarAndTreeWidget(processedFiles, offset);
            mutex.unlock();
        }

        if (searchCancelled)
        {
            return;
        }
    }

    mutex.lock();
    updateProgressBarAndTreeWidget(processedFiles, offset);
    mutex.unlock();
}

QVector<QVector<QString> > MainWindow::distributeFiles(QVector<QString> &filePaths)
{
    quint64 threadCount = qMin(filePaths.size(), QThread::idealThreadCount());
    QVector<QVector<QString> > distribution(threadCount);
    QVector<quint64> distribution_sizes(threadCount, 0);

    quint64 id = 0, total_size = 0;
    for (QString filePath : filePaths)
    {
        QFile currentFile(currentDirectory + "/" + filePath);
        total_size += currentFile.size();
    }
    quint64 average = total_size / threadCount;
    for (quint64 i = 0; id != filePaths.size(); i = (i + 1) % threadCount)
    {
        if (distribution_sizes[i] < average)
        {
            QString filePath = filePaths[id];
            QFile currentFile(currentDirectory + "/" + filePath);
            distribution[i].push_back(filePath);
            distribution_sizes[i] += currentFile.size();
            ++id;
        }
    }

    return distribution;
}

QVector<QString> MainWindow::requestFilesRecursive(QString directoryPath)
{
    QDir directory(directoryPath);
    QFileInfoList fileList = directory.entryInfoList(QDir::Files | QDir::NoSymLinks);
    QFileInfoList dirList = directory.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QVector<QString> recursiveList;
    for (QFileInfo file : fileList)
    {
        recursiveList.push_back(file.fileName());
    }
    for (QFileInfo dir : dirList)
    {
        if (!dir.isReadable() || !dir.exists())
        {
            continue;
        }

        QVector<QString> subdirFileList = requestFilesRecursive(dir.absoluteFilePath());
        for (QString file : subdirFileList)
        {
            recursiveList.push_back(dir.fileName() + "/" + file);
        }
    }

    return recursiveList;
}

void MainWindow::selectAndIndexDirectory()
{
    //=== selecting directory
    currentDirectory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                        QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!QDir(currentDirectory).exists())
    {
        QMessageBox::critical(this, "String finder", "Chosen directory cannot be opened");
        return;
    }
    ui->directoryLineEdit->setText(currentDirectory);
    isDirectoryIndexed = false;
    trigramsCancelled = false;

    //=== retrieving list of files
    ui->statusBar->showMessage("Retrieving the list of files in the directory...");
    QVector<QString> filePaths = requestFilesRecursive(currentDirectory);
    filesTotal = filePaths.size();
    filesProcessed = 0;

    //=== trigrams calculation
    distributedTrigrams.clear();

    ui->statusBar->showMessage("Indexing directory...");
    QVector<QVector<QString> > distribution = distributeFiles(filePaths);

    QProgressDialog progressBar;
    progressBar.setWindowTitle("String finder");
    progressBar.setLabelText("Indexing in progress...");
    progressBar.setRange(0, filesTotal);

    connect(&watcher, SIGNAL(progressValueChanged(int)), &progressBar, SLOT(setValue(int)));
    connect(&watcher, SIGNAL(finished()), &progressBar, SLOT(reset()));
    connect(&progressBar, SIGNAL(canceled()), &watcher, SLOT(cancel()));
    connect(&progressBar, SIGNAL(canceled()), this, SLOT(cancelTrigrams()));

    watcher.setFuture(QtConcurrent::map(distribution, [this](QVector<QString> &paths) { calculateTrigrams(paths); }));
    progressBar.exec();
    watcher.waitForFinished();

    //=== post-trigrams stuff

    if (watcher.isCanceled())
    {
        QMessageBox::critical(this, "String finder", "Indexing cancelled; to search for string, please choose a directory and wait until indexing is finished");
        return;
    }

    isDirectoryIndexed = true;
    ui->statusBar->showMessage("Search for matches or choose another directory");
}

quint64 MainWindow::trigramCharactersToNumber(QString trigram)
{
    return ((ASIZE * ASIZE) * (quint64)trigram[0].unicode() + ASIZE * (quint64)trigram[1].unicode() + (quint64)trigram[2].unicode());
}

void MainWindow::calculateTrigrams(QVector<QString> &filePaths)
{
    QVector<QPair<QString, QVector<quint64> > > currentPack;
    quint64 calculated = 0, unreadable = 0;
    for (QString filePath : filePaths)
    {
        ++calculated;
        QFile currentFile(currentDirectory + "/" + filePath);
        if (!currentFile.open(QFile::ReadOnly))
        {
            ++unreadable;
            continue;
        }

        QSet<quint64> trigramsSet;

        QTextStream reader(&currentFile);
        QString currentTrigram = "";
        while (!reader.atEnd())
        {
            QChar c;
            reader >> c;
            currentTrigram += c;
            if (currentTrigram.size() < 3)
            {
                continue;
            }
            if (currentTrigram.size() > 3)
            {
                currentTrigram = currentTrigram.right(3);
            }

            if (!currentTrigram.contains('\n'))
            {
                trigramsSet.insert(trigramCharactersToNumber(currentTrigram));
            }

            if (trigramsCancelled)
            {
                return;
            }
        }

        QVector<quint64> currentTrigrams;
        for (quint64 x : trigramsSet)
        {
            currentTrigrams.push_back(x);
        }

        qSort(currentTrigrams);
        currentPack.push_back(QPair<QString, QVector<quint64> >(filePath, currentTrigrams));
        currentFile.close();

        if (mutex.try_lock())
        {
            filesProcessed += calculated;
            filesUnreadable += unreadable;
            unreadable = 0;
            calculated = 0;
            emit watcher.progressValueChanged(filesProcessed);
            mutex.unlock();
        }
    }
    mutex.lock();
    filesProcessed += calculated;
    filesUnreadable += unreadable;
    emit watcher.progressValueChanged(filesProcessed);

    distributedTrigrams.push_back(currentPack);
    mutex.unlock();
}

void MainWindow::cancelTrigrams()
{
    trigramsCancelled = true;
}

void MainWindow::cancelSearch()
{
    searchCancelled = true;
}

void MainWindow::searchForString()
{
    if (ui->directoryLineEdit->text().isEmpty() || !isDirectoryIndexed)
    {
        QMessageBox::critical(this, "String finder", "Please select a directory and wait until it is indexed");
        return;
    }
    if (ui->searchLineEdit->text().size() < 3 || ui->searchLineEdit->text().size() > 63)
    {
        QMessageBox::critical(this, "String finder", "The string should be 3 to 63 characters long; your string's length is " +
                              QString::number(ui->searchLineEdit->text().size()) + " characters");
        return;
    }

    ui->treeWidget->clear();

    ui->statusBar->showMessage("Searching for matches...");
    ui->searchLineEdit->setDisabled(true);
    ui->searchButton->setDisabled(true);

    filesProcessed = filesUnreadable;
    searchCancelled = false;

    QProgressDialog progressBar;
    progressBar.setWindowTitle("String finder");
    progressBar.setLabelText("Searching in progress...");
    progressBar.setRange(filesUnreadable, filesTotal);

    connect(&watcher, SIGNAL(progressValueChanged(int)), &progressBar, SLOT(setValue(int)));
    connect(&watcher, SIGNAL(finished()), &progressBar, SLOT(reset()));
    connect(&progressBar, SIGNAL(canceled()), &watcher, SLOT(cancel()));
    connect(&progressBar, SIGNAL(canceled()), this, SLOT(cancelSearch()));

    watcher.setFuture(QtConcurrent::map(distributedTrigrams, [this](QVector<QPair<QString, QVector<quint64> > > &files) { search(files); }));
    progressBar.exec();
    watcher.waitForFinished();

    //search(distributedTrigrams[0]);

    ui->statusBar->showMessage("Search finished");
    ui->searchLineEdit->setDisabled(false);
    ui->searchButton->setDisabled(false);
}
