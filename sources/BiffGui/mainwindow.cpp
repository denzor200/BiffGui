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

#include "settings.h"
#include "generating.h"

#include "settingsregistry.h"
#include "maintabledelegates.h"
#include "utils.h"
#include "convertersyncapi.h"
#include "converterwaiting.h"
#include "controlinfo.h"
#include "auth.h"

#ifdef _DEBUG
#include <QAbstractItemModelTester>
#endif // _DEBUG

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

    ui->action_make_shared_flag->setChecked(QSettings().value(SettingsRegistry::MAKE_SHARED_FLAG, true).toBool());
    ui->action_make_individual_flag->setChecked(QSettings().value(SettingsRegistry::MAKE_INDIVIDUAL_FLAG, true).toBool());
    ui->action_make_clean_flag->setChecked(QSettings().value(SettingsRegistry::MAKE_CLEAN_FLAG, false).toBool());

#ifdef _DEBUG
    // Очень странно..
    // Зачем QAbstractItemModelTester требует от таблицы(которая даже не является деревом) наличия дочерних объектов?
    // Пока пришлось убрать

    // new QAbstractItemModelTester(m_ModelsMgr->GetModel(), QAbstractItemModelTester::FailureReportingMode::Fatal, this);
    // new QAbstractItemModelTester(m_ModelsMgr->GetModelReversed(), QAbstractItemModelTester::FailureReportingMode::Fatal, this);
#endif // _DEBUG
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
    const bool needMakeShared = ui->action_make_shared_flag->isChecked();
    const bool needMakeIndividual = ui->action_make_individual_flag->isChecked();
    const bool needMakeClean = ui->action_make_clean_flag->isChecked();

    if (!needMakeShared && !needMakeIndividual && !needMakeClean) {
        QMessageBox::critical(this, "Ошибка", "Выберите хотя бы один флаг из меню 'Опции -> Генерация монтажных листов'");
        return;
    }

    if (!m_OpenedSubbtitle) {
        QMessageBox::critical(this, "Ошибка", "Не открыт файл с субтитрами");
        return;
    }
    if (actorsCount <= 0 && personsCount <= 0) {
        QMessageBox::critical(this, "Ошибка", "Таблица пуста");
        return;
    }
    if (actorsCount <= 0) {
        QMessageBox::critical(this, "Ошибка", "Список актеров пуст");
        return;
    }
    if (personsCount <= 0) {
        QMessageBox::critical(this, "Ошибка", "Список персонажей пуст");
        return;
    }

    QString OutDir;
    OutDir = QFileDialog::getExistingDirectory(this, "Создать монтажные листы", QSettings().value(SettingsRegistry::DOC_OUTDIR).toString());

    if (OutDir!="")
    {
        QDir CurrentDir;
        QSettings().setValue(SettingsRegistry::DOC_OUTDIR,
            CurrentDir.absoluteFilePath(OutDir));

        Utils::TempFilenameGuard tempTableFilename;
        Utils::TempFilenameGuard tempDecisionsFilename;
        ControlInfo ctrlTable;
        ControlInfo ctrlDecisions;
        if (static_cast<QString>(tempTableFilename) != "" && static_cast<QString>(tempDecisionsFilename) != "") {
            // Saving to temp files..
            if (
                    tempTableFilename.Write([&](auto W){m_ModelsMgr->SaveTable(W, true);}, &ctrlTable) &&
                    tempDecisionsFilename.Write([&](auto W){SaveDecisions(W);}, &ctrlDecisions))
            {
                // Using temp files in another child process..
                Generating w(this);
                QVector<QPair<QString,QString>> Params;
                // TODO: завести список "коротких" параметров где нибудь в h-нике

                // Control information
                Params.push_back({"-z", PrintControlInfo(ctrlTable)});
                Params.push_back({"-d", PrintControlInfo(ctrlDecisions)});
                Params.push_back({"-c", m_OpenedSubbtitle->CtrlData});

                // Optional parameters
                if (m_OpenedSubbtitle->MarkupTypeInitialized)
                    Params.push_back({"-m", QString::number(static_cast<int>(m_OpenedSubbtitle->MarkupType))});
                if (!needMakeShared)
                    Params.push_back({"-s", "true"});
                if (!needMakeIndividual)
                    Params.push_back({"-i", "true"});
                if (!needMakeClean)
                    Params.push_back({"-p", "true"});

                w.StartProcess(m_OpenedSubbtitle->FileName, tempDecisionsFilename, tempTableFilename, OutDir, Params);
                w.exec();

                return; // all right
            }
        }
        showTempFileOpenWError();
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

void MainWindow::InsertRow()
{
    if (m_IsReversed)
        m_ModelsMgr->GetModelReversed()->InsertRow();
    else {
        m_ModelsMgr->GetModel()->InsertRow();
    }
}

void MainWindow::RemoveAllSelectedRows()
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

void MainWindow::SetDenyValueToAll(bool value)
{
    QAbstractTableModel* model = nullptr;
    if (m_IsReversed)
        model = m_ModelsMgr->GetModelReversed();
    else {
        model = m_ModelsMgr->GetModel();
    }
    int count = model->rowCount();
    for (int i=0;i<count;++i)
    {
        QModelIndex index = model->index(i, 2, QModelIndex());
        model->setData(index, value, Qt::EditRole);
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

void MainWindow::showTempFileOpenWError() const
{
    QMessageBox::critical(QApplication::activeWindow(), "Что-то пошло не так..", "Не удалось открыть временный файл на запись");
}

void MainWindow::SaveDecisions(QByteArray *StreamOut) const
{
    Q_ASSERT(m_OpenedSubbtitle);

    QTextStream streamFileOut(StreamOut);
    streamFileOut.setCodec("UTF-8");
    streamFileOut.setGenerateByteOrderMark(true);
    for (const QString& line : m_OpenedSubbtitle->UsersDecisions)
        streamFileOut << line << endl;
    streamFileOut << endl;
    streamFileOut.flush();
}

// параметр ext актуален только для индивидуальных субтитров
void MainWindow::SaveSubbtitle(const QString &outFileName, const QString& ext, bool isIndividual)
{
    bool isFull = !isIndividual;
    Utils::TempFilenameGuard tempDecisionsFilename;
    Utils::TempFilenameGuard tempTableFilename;
    ControlInfo ctrlDecisions;
    ControlInfo ctrlTable;
    if (static_cast<QString>(tempDecisionsFilename) != "" && (isFull || static_cast<QString>(tempTableFilename) != ""))
    {
        if (
                tempDecisionsFilename.Write([&](auto W){SaveDecisions(W);}, &ctrlDecisions) &&
                (isFull || tempTableFilename.Write([&](auto W){m_ModelsMgr->SaveTable(W, true);}, &ctrlTable)))
        {
            ConverterWaiting_SaveSubbtitle waiting;
            QVector<QPair<QString,QString>> Params;

            Params.push_back({"-c", m_OpenedSubbtitle->CtrlData});
            Params.push_back({"-d", PrintControlInfo(ctrlDecisions)});

            // Optional parameters
            if (m_OpenedSubbtitle->MarkupTypeInitialized)
                Params.push_back({"-m", QString::number(static_cast<int>(m_OpenedSubbtitle->MarkupType))});
            if (isIndividual)
            {
                Params.push_back({"-a", tempTableFilename});
                Params.push_back({"-z", PrintControlInfo(ctrlTable)});
                Params.push_back({"-e", ext});
            }

            waiting.StartProcess( m_OpenedSubbtitle->FileName, tempDecisionsFilename, outFileName, Params);
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

            return; // all right with decisions
        }
    }
    showTempFileOpenWError();
}

void MainWindow::ExportIndividualSubbtitles(const QString &ext)
{
    if (!m_OpenedSubbtitle)
        return;

    if (m_ModelsMgr->constRegistryAPI()->getRealActorsCount() > 0)
    {
        QString OutDir;
        OutDir = QFileDialog::getExistingDirectory(this, "Сохранить субтитры как индивидуальные", QSettings().value(SettingsRegistry::SUBBTITLES_INDIVIDUAL_OUTDIR).toString());

        if (OutDir!="")
        {
            QDir CurrentDir;
            QSettings().setValue(SettingsRegistry::SUBBTITLES_INDIVIDUAL_OUTDIR,
                CurrentDir.absoluteFilePath(OutDir));

            SaveSubbtitle(OutDir, ext, true);
        }
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Вы не можете экспортировать индивидуальные субтитры, поскольку не прописали ни одного актера в таблице");
    }

}

void MainWindow::on_action_open_triggered()
{
    if (!tryCloseFile())
        return;

    QString subbtitleFilename = QFileDialog::getOpenFileName(this,
        tr("Открыть субтитры"),
        QSettings().value(SettingsRegistry::SUBBTITLES_INDIR).toString(),
        tr(SUBBTITLES_EXTENSIONS));

    if (subbtitleFilename!="")
    {
        QDir CurrentDir;
        QSettings().setValue(SettingsRegistry::SUBBTITLES_INDIR,
                    CurrentDir.absoluteFilePath(subbtitleFilename));

        QStringList     personsList;
        QString         CRC;
        SrtFormat::MarkupType mt;
        ConverterWaiting_ShowPersonList waiting(personsList);
        waiting.StartProcess( subbtitleFilename);

        int execStatus              = waiting.exec();
        bool isCanceled             = waiting.IsCanceledByUser();
        int procStatus              = waiting.GetProcessStatus();
        bool isCRC_Inited           = waiting.GetCRC(CRC);
        bool isMarkupType_Inited    = waiting.GetMarkupType(mt);

        if (!isCanceled)
        {
            if (0 == execStatus)
            {
                if (0 == procStatus)
                {
                    if (!isCRC_Inited)
                    {
                        QMessageBox::critical(this, "Что-то пошло не так..", "Дочерний процесс не послал контрольную сумму входного файла");
                        return ;
                    }

                    if (m_ModelsMgr->LoadPersons(personsList) > 0)
                    {
                        SubbtitleContext* ctx = new SubbtitleContext();
                        ctx->FileName = subbtitleFilename;
                        ctx->CtrlData = CRC;
                        ctx->UsersDecisions = waiting.GetUsersDecisions();
                        ctx->MarkupTypeInitialized = isMarkupType_Inited;
                        ctx->MarkupType = mt;
                        m_OpenedSubbtitle = ctx;
                        setWindowTitle(subbtitleFilename + " - " + QApplication::applicationName());
                    }
                    else {
                        QMessageBox::critical(this, "Ошибка", "Не удалось извлечь ни одного имени персонажа из файла.");
                    }
                }
                // else .. msg box is not needed
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
        QString outFileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить субтитры"),
            QSettings().value(SettingsRegistry::SUBBTITLES_OUTDIR).toString(),
            tr(SUBBTITLES_EXTENSIONS));

        if (outFileName!="")
        {
            QDir CurrentDir;
            QSettings().setValue(SettingsRegistry::SUBBTITLES_OUTDIR,
                        CurrentDir.absoluteFilePath(outFileName));

            SaveSubbtitle(outFileName, "", false);
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
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("Сохранить список персонажей"),
            QSettings().value(SettingsRegistry::PERSONS_OUTDIR).toString(),
            tr("Text files (*.txt)"));

        if (fileName!="")
        {
            QDir CurrentDir;
            QSettings().setValue(SettingsRegistry::PERSONS_OUTDIR,
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
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Открыть таблицу"),
            QSettings().value(SettingsRegistry::TABLE_INDIR).toString(),
            tr("Config files (*.cfg)"));

        if (fileName!="")
        {
            QDir CurrentDir;
            QSettings().setValue(SettingsRegistry::TABLE_INDIR,
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
            QString fileName = QFileDialog::getSaveFileName(this,
                tr("Сохранить таблицу"),
                QSettings().value(SettingsRegistry::TABLE_OUTDIR).toString(),
                tr("Config files (*.cfg)"));

            if (fileName!="")
            {
                QDir CurrentDir;
                QSettings().setValue(SettingsRegistry::TABLE_OUTDIR,
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

void MainWindow::on_action_generate_doc_triggered()
{
    this->makeDoc();
}

void MainWindow::on_action_make_shared_flag_triggered()
{
    QSettings().setValue(SettingsRegistry::MAKE_SHARED_FLAG,
        ui->action_make_shared_flag->isChecked());
}

void MainWindow::on_action_make_individual_flag_triggered()
{
    QSettings().setValue(SettingsRegistry::MAKE_INDIVIDUAL_FLAG,
        ui->action_make_individual_flag->isChecked());
}

void MainWindow::on_action_make_clean_flag_triggered()
{
	QSettings().setValue(SettingsRegistry::MAKE_CLEAN_FLAG,
        ui->action_make_clean_flag->isChecked());
}

void MainWindow::on_toolButton_Insert_clicked()
{
    InsertRow();
}

void MainWindow::on_toolButton_Delete_clicked()
{
    RemoveAllSelectedRows();
}

void MainWindow::on_toolButton_Reverse_clicked()
{
    ReverseTable();
}

void MainWindow::on_commandLinkButton_makeDoc_clicked()
{
    this->makeDoc();
}

void MainWindow::on_action_select_all_triggered()
{
    if (m_IsReversed)
        ui->tableView_Reversed->selectAll();
    else {
        ui->tableView->selectAll();
    }
}

static const QString s_DenzorEmail = "denismikhaylov38@gmail.com";
static const QString s_CheEmail = "che.rskov@yandex.ru";

void MainWindow::on_action_about_this_triggered()
{
    const char *qt_version = qVersion();
    QString info = "Генератор монтажных листов 'Biff Tannen'\n";
    info += QString() + "Основан на Qt " + qt_version + " (" + Utils::compilerString().c_str() + ")\n";
    info += QString() + "Собрано " + Utils::compileDatetime() + "\n";
    info += "В разработке принимали участие:\n";
    info += "\tМихайлов Денис aka denzor - Программирование, UI-дизайн\n";
    info += "\tЧерсков Станислав aka Che - Идейный вдохновитель проекта\n";
    info += "Контакты для связи:\n";
    info += "\t" + s_DenzorEmail + "\n";
    info += "\t" + s_CheEmail + "\n";

    QMessageBox::information(this, "О программе Biff Tannen", info);
}

void MainWindow::on_action_deny_all_triggered()
{
    SetDenyValueToAll(true);
}

void MainWindow::on_action_deny_disable_all_triggered()
{
    SetDenyValueToAll(false);
}

void MainWindow::on_action_remove_all_links_triggered()
{
    QAbstractTableModel* model = nullptr;
    if (m_IsReversed)
        model = m_ModelsMgr->GetModelReversed();
    else {
        model = m_ModelsMgr->GetModel();
    }
    int count = model->rowCount();
    for (int i=0;i<count;++i)
    {
        QModelIndex index = model->index(i, 1, QModelIndex());
        QStringList empty;
        model->setData(index, empty, Qt::EditRole);
    }
}

void MainWindow::on_action_register_software_triggered()
{
    ConverterWaiting_VerifyCert waiting;
    waiting.StartProcess(true);
    int execStatus = waiting.exec();
    bool isCanceled = waiting.IsCanceledByUser();
    int procStatus = waiting.GetProcessStatus();
	
	if (isCanceled)
		return;

    if (0 == execStatus)
    {
        if (0 == procStatus)
        {
            QMessageBox::information(this, "Порядок", "Ваша копия Biff Tannen уже активирована!");
            return ;
        }

    }
    else {
        // anomal case..
        return;
    }

    Auth w;
    w.exec();
}

void MainWindow::on_action_add_triggered()
{
    if (m_IsReversed)
    {
        QMessageBox::critical(nullptr, "Ошибка", "Нельзя добавлять новых персонажей. Список персонажей должен соответствовать списку из субтитров.");
        return;
    }
    InsertRow();
}

void MainWindow::on_action_remove_triggered()
{
    if (m_IsReversed)
    {
        QMessageBox::critical(nullptr, "Ошибка", "Нельзя удалять персонажей. Список персонажей должен соответствовать списку из субтитров.");
        return;
    }
    RemoveAllSelectedRows();
}

void MainWindow::on_action_ass_triggered()
{
    ExportIndividualSubbtitles(".ass");
}

void MainWindow::on_action_srt_triggered()
{
    ExportIndividualSubbtitles(".srt");
}
