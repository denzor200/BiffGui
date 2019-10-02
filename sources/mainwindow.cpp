#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolButton>
#include <QSettings>
#include <QTemporaryFile>

#include <QFileDialog>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <QDebug>

#include "Settings.h"
#include "generating.h"

#include "DirectoriesRegistry.h"
#include "maintabledelegates.h"
#include "utils.h"
#include "convertersyncapi.h"
#include "converterwaiting.h"

static const char* SUBBTITLES_EXTENSIONS = "Все поддерживаемые форматы(*.ass *.srt);;"
                                           "Advanced SubStation Alpha (*.ass);;"
                                           "SubRip (*.srt)";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_ModelsMgr = new MainTableModelsManager(this);

    ui->tableView->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
    ui->tableView->setItemDelegateForColumn(0,new CEnterNameDelegate(this) );
    ui->tableView->setItemDelegateForColumn(1,new CChoiceLinksDelegate(this) );
    ui->tableView->setItemDelegateForColumn(2,new CChoiceDenyStatusDelegate(this) );
    ui->tableView->setSortingEnabled(true);

    ui->tableView_Reversed->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);
    ui->tableView_Reversed->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
    ui->tableView_Reversed->setItemDelegateForColumn(0,new CEnterNameDelegate(this) );
    ui->tableView_Reversed->setItemDelegateForColumn(1,new CChoiceLinksDelegate(this) );
    ui->tableView_Reversed->setItemDelegateForColumn(2,new CChoiceDenyStatusDelegate(this) );
    ui->tableView_Reversed->setSortingEnabled(true);

    m_ProxyModel.setSourceModel( m_ModelsMgr->GetModel() );
    ui->tableView->setModel( &m_ProxyModel );

    m_ProxyModelReversed.setSourceModel( m_ModelsMgr->GetModelReversed() );
    ui->tableView_Reversed->setModel( &m_ProxyModelReversed );


    // под конец решили, что перевернутая таблица по умолчанию все же лучше..
    ReverseTable();
}

MainWindow::~MainWindow()
{
    delete m_OpenedSubbtitle;
    delete ui;
}

void MainWindow::makeDoc()
{
    const int actorsCount = m_ModelsMgr->constRegistryAPI()->getRealActorsCount();
    const int personsCount = m_ModelsMgr->constRegistryAPI()->getRealPersonsCount();

    if (!m_OpenedSubbtitle)
    {
        QMessageBox::critical(this, "Ошибка", "Не открыт файл с субтитрами");
        return;
    }
    if (actorsCount <= 0 && personsCount <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Таблица пуста");
        return;
    }
    if (actorsCount <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Список актеров пуст");
        return;
    }
    if (personsCount <= 0)
    {
        QMessageBox::critical(this, "Ошибка", "Список персонажей пуст");
        return;
    }

    QString InFile, OutDir;

    InFile = m_OpenedSubbtitle->FileName;
    QSettings Settings;
    OutDir = QFileDialog::getExistingDirectory(this, "Создать монтажные листы", Settings.value(DirectoriesRegistry::DOC_OUTDIR).toString());

    if (OutDir!="")
    {
        QDir CurrentDir;
        Settings.setValue(DirectoriesRegistry::DOC_OUTDIR,
            CurrentDir.absoluteFilePath(OutDir));

        const QString& tempFilename = Utils::GetNewTempFilename();
        uint32_t CRC32 = 0;
        if (tempFilename != "") {
            // Saving to temp file..
            if (m_ModelsMgr->SaveTable(tempFilename, true, &CRC32))
            {
                // Using temp file in another child process..
                Generating w(this);
                // Utils::hexStr(reinterpret_cast<uint8_t*>(&CRC32), sizeof(CRC32)).c_str()
                QVector<QPair<QString,QString>> Params;
                w.StartProcess(InFile, tempFilename, OutDir, Params);
                w.exec();
            }
            else {
                showFileOpenWError(tempFilename);
            }

            // темповый файл нам больше не нужен..
            // TODO: потестировать при отсутствии файла, или отсутствии прав
            QFile::remove(tempFilename);
        }
        else {
            showMainTableWError();
        }
    }
}

void MainWindow::ReverseTable()
{
    if (m_IsReversed)
    {
        ui->tableView->show();
        ui->tableView_Reversed->hide();
        ui->toolButton_Insert->setEnabled(true);
        ui->toolButton_Delete->setEnabled(true);
        m_IsReversed = false;
    }
    else {
        ui->tableView->hide();
        ui->tableView_Reversed->show();
        ui->toolButton_Insert->setEnabled(false);
        ui->toolButton_Delete->setEnabled(false);
        m_IsReversed = true;
    }
}

// В возвращаемом значении флаг - давал ли пользователь согласие на выволнение операции
bool MainWindow::tryCloseFile()
{
    bool doIt = false;
    if (m_OpenedSubbtitle != nullptr)
    {
        // Запрашиваем у пользователя подтверждения только если есть реальные не сохраненные данные..
        if (m_ModelsMgr->constRegistryAPI()->getRealActorsCount() > 0 ||
                m_ModelsMgr->constRegistryAPI()->getRealPersonsCount() > 0)
        {
            auto reply = QMessageBox::question(this, "Подтверждение действия", "Вы действительно хотите закрыть текущую сессию? Все несохраненные данные будут утеряны.",
                                        QMessageBox::Yes|QMessageBox::No);
            if (reply==QMessageBox::Yes)
                doIt = true;
            else {
                return false;
            }
        }
        else {
            // .. в противном случае нет никакого смысла его беспокоить
            doIt = true;
        }
    }
    if (doIt)
    {
        delete m_OpenedSubbtitle;
        m_OpenedSubbtitle = nullptr;
        m_ModelsMgr->ClearAll();
        setWindowTitle(QApplication::applicationName());
    }
    return true;
}

void MainWindow::showFileOpenRError(const QString &fileName) const
{
    std::stringstream ss;
    ss << "Не удалось открыть файл '" << fileName.toUtf8().data() << "' на чтение";
    QMessageBox::critical(QApplication::activeWindow(), "Что-то пошло не так..", ss.str().c_str());
}

void MainWindow::showFileOpenWError(const QString &fileName) const
{
    std::stringstream ss;
    ss << "Не удалось открыть файл '" << fileName.toUtf8().data() << "' на запись";
    QMessageBox::critical(QApplication::activeWindow(), "Что-то пошло не так..", ss.str().c_str());
}

void MainWindow::showMainTableWError() const
{
    QMessageBox::critical(QApplication::activeWindow(), "Что-то пошло не так..", "Не удалось открыть временный файл для записи главной таблицы");
}

void MainWindow::on_action_open_triggered()
{
    if (!tryCloseFile())
        return;

    QSettings Settings;
    QString subbtitleFilename = QFileDialog::getOpenFileName(this,
        tr("Открыть субтитры"),
        Settings.value(DirectoriesRegistry::SUBBTITLES_INDIR).toString(),
        tr(SUBBTITLES_EXTENSIONS));

    if (subbtitleFilename!="")
    {
        QDir CurrentDir;
        Settings.setValue(DirectoriesRegistry::SUBBTITLES_INDIR,
                    CurrentDir.absoluteFilePath(subbtitleFilename));

        QStringList personsList;
        ConverterWaiting_ShowPersonList waiting(personsList);
        waiting.StartProcess( subbtitleFilename);
        int execStatus = waiting.exec();
        bool isCanceled = waiting.IsCanceledByUser();
        int procStatus = waiting.GetProcessStatus();
        if (!isCanceled)
        {
            if (0 == execStatus && 0 == procStatus)
            {
                m_ModelsMgr->LoadPersons(personsList);
                SubbtitleContext* ctx = new SubbtitleContext();
                ctx->FileName = subbtitleFilename;
                m_OpenedSubbtitle = ctx;
                setWindowTitle(subbtitleFilename + " - " + QApplication::applicationName());
            }
            else {
                QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось сгенерировать список персонажей");
            }
        }
    }
}

void MainWindow::on_action_save_triggered()
{
    // TODO: блокируем кнопку вообще, если ничего не открыто
    if (m_OpenedSubbtitle)
    {
        QSettings Settings;
        QString outFileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить субтитры"),
            Settings.value(DirectoriesRegistry::SUBBTITLES_OUTDIR).toString(),
            tr(SUBBTITLES_EXTENSIONS));

        if (outFileName!="")
        {
            QDir CurrentDir;
            Settings.setValue(DirectoriesRegistry::SUBBTITLES_OUTDIR,
                        CurrentDir.absoluteFilePath(outFileName));

            ConverterWaiting_SaveSubbtitle waiting;
            QVector<QPair<QString,QString>> Params;
            waiting.StartProcess( m_OpenedSubbtitle->FileName, outFileName, Params);
            int execStatus = waiting.exec();
            bool isCanceled = waiting.IsCanceledByUser();
            int procStatus = waiting.GetProcessStatus();

            if (!isCanceled)
            {
                if (0 == execStatus && 0 == procStatus)
                {
                }
                else {
                    QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось сохранить субтитры");
                }
            }
        }
    }
}

void MainWindow::on_action_close_triggered()
{
    tryCloseFile();
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_action_exit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_action_settings_triggered()
{
    Settings w(this);
    if (w.Initialize())
        w.exec();
}

void MainWindow::on_action_save_persons_triggered()
{
    const int personsCount = m_ModelsMgr->constRegistryAPI()->getRealPersonsCount();
    if (personsCount > 0)
    {
        QSettings Settings;
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить список персонажей"),
            Settings.value(DirectoriesRegistry::PERSONS_OUTDIR).toString(),
            tr("Text files (*.txt)"));

        if (fileName!="")
        {
            QDir CurrentDir;
            Settings.setValue(DirectoriesRegistry::PERSONS_OUTDIR,
                        CurrentDir.absoluteFilePath(fileName));

            if (!m_ModelsMgr->SavePersons(fileName))
                showFileOpenWError(fileName);
        }
    }
}

void MainWindow::on_action_open_table_triggered()
{
    if (m_OpenedSubbtitle == nullptr)
    {
        QMessageBox::critical(this, "Ошибка", "Нельзя открывать таблицу без субтитров");
        return ;
    }

    {
        QSettings Settings;
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Открыть таблицу"),
            Settings.value(DirectoriesRegistry::TABLE_INDIR).toString(),
            tr("Config files (*.cfg)"));

        if (fileName!="")
        {
            QDir CurrentDir;
            Settings.setValue(DirectoriesRegistry::TABLE_INDIR,
                        CurrentDir.absoluteFilePath(fileName));

            if (!m_ModelsMgr->OpenTable(fileName))
                showFileOpenRError(fileName);
        }
    }
}

void MainWindow::on_action_save_table_triggered()
{
    // Игнорируем, если сохранять нечего
    const int actorsCount = m_ModelsMgr->constRegistryAPI()->getRealActorsCount();
    const int personsCount = m_ModelsMgr->constRegistryAPI()->getRealPersonsCount();
    if (actorsCount > 0 || personsCount > 0)
    {
        if (actorsCount > 0)
        {
            QSettings Settings;
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Сохранить таблицу"),
                Settings.value(DirectoriesRegistry::TABLE_OUTDIR).toString(),
                tr("Config files (*.cfg)"));

            if (fileName!="")
            {
                QDir CurrentDir;
                Settings.setValue(DirectoriesRegistry::TABLE_OUTDIR,
                            CurrentDir.absoluteFilePath(fileName));

                if (!m_ModelsMgr->SaveTable(fileName))
                    showFileOpenWError(fileName);
            }
        }
        else {
            QMessageBox::critical(this, "Что-то пошло не так..", "Вы не можете сохранить таблицу, поскольку не прописали в ней ни одного актера");
        }
    }
}

void MainWindow::on_action_save_individual_triggered()
{
    // TODO: блокируем кнопку вообще, если ничего не открыто
    if (m_OpenedSubbtitle)
    {
        QSettings Settings;
        QString outFileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить субтитры как индивидуальные"),
            Settings.value(DirectoriesRegistry::SUBBTITLES_OUTDIR).toString(),
            tr(SUBBTITLES_EXTENSIONS));

        if (outFileName!="")
        {
            QDir CurrentDir;
            Settings.setValue(DirectoriesRegistry::SUBBTITLES_OUTDIR,
                        CurrentDir.absoluteFilePath(outFileName));

            const QString& tempFilename = Utils::GetNewTempFilename();
            if (tempFilename != "") {
                // Saving to temp file..
                if (m_ModelsMgr->SavePersons(tempFilename, true))
                {
                    // Using temp file in another child process..
                    ConverterWaiting_SaveSubbtitle waiting;
                    QVector<QPair<QString,QString>> Params;
                    Params.reserve(1);
                    Params.push_back({"-p", tempFilename});
                    waiting.StartProcess( m_OpenedSubbtitle->FileName, outFileName, Params);
                    int execStatus = waiting.exec();
                    bool isCanceled = waiting.IsCanceledByUser();
                    int procStatus = waiting.GetProcessStatus();

                    if (!isCanceled)
                    {
                        if (0 == execStatus && 0 == procStatus)
                        {
                        }
                        else {
                            QMessageBox::critical(this, "Что-то пошло не так..", "Не удалось сохранить субтитры");
                        }
                    }
                }
                else
                {
                    showFileOpenWError(tempFilename);
                }


                // темповый файл нам больше не нужен..
                // TODO: потестировать при отсутствии файла, или отсутствии прав
                QFile::remove(tempFilename);
            }
            else {
                showMainTableWError();
            }
        }
    }
}

void MainWindow::on_action_generate_doc_triggered()
{
    this->makeDoc();
}

void MainWindow::on_action_make_shared_flag_triggered()
{

}

void MainWindow::on_action_make_individual_flag_triggered()
{

}

void MainWindow::on_toolButton_Insert_clicked()
{
    if (m_IsReversed)
        m_ModelsMgr->GetModelReversed()->InsertRow();
    else {
        m_ModelsMgr->GetModel()->InsertRow();
    }
}

void MainWindow::on_toolButton_Delete_clicked()
{
    IMainTableModel*        activeModel = nullptr;
    QTableView*             activeView = nullptr;
    QSortFilterProxyModel*  activeProxy = nullptr;
    if (m_IsReversed)
    {
        activeView = ui->tableView_Reversed;
        activeModel = m_ModelsMgr->GetModelReversed();
        activeProxy = &m_ProxyModelReversed;
    }
    else {
        activeView = ui->tableView;
        activeModel = m_ModelsMgr->GetModel();
        activeProxy = &m_ProxyModel;
    }

    // preparing..
    QItemSelection selection( activeView->selectionModel()->selection() );

    QList<int> rows;
    QSet<int> rows_set; // rows_set only for checking
    foreach( const QModelIndex & value, selection.indexes() ) {
        const QModelIndex & translatedValue = activeProxy->mapToSource(value);
        int id = translatedValue.row();
        if (rows_set.find(id)==rows_set.end())
        {
            rows.append( id );
            rows_set.insert(id);
        }
    }

    std::sort( rows.begin(), rows.end() );

    // checking for zero size
    if (rows.size()==0)
    {
        return ;
    }

    // confirmation from user..
    std::stringstream ss;
    ss << "Вы действительно хотите удалить выделенные строки в таблице? (" << rows.size() << " шт.)";
    auto reply = QMessageBox::question(this, "Подтверждение действия", ss.str().c_str(),
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply!=QMessageBox::Yes)
    {
        return ;
    }

    // just doing..
    // strictly in reverse order!!
    int prev = -1;
    for( int i = rows.count() - 1; i >= 0; i -= 1 ) {
        int current = rows[i];
        if( current != prev ) {
            activeModel->RemoveRow(current);
            prev = current;
        }
    }
}

void MainWindow::on_toolButton_Reverse_clicked()
{
    ReverseTable();
}

void MainWindow::on_commandLinkButton_makeDoc_clicked()
{
    this->makeDoc();
}
