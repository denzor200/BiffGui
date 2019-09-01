#include "actorslist.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolButton>
#include "lineedit.h"

#include <QFileDialog>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
    ui->tableWidget->setStyleSheet("QLineEdit { border: none }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{

}

void MainWindow::on_toolButton_2_clicked()
{
    // preparing..
    QItemSelection selection( ui->tableWidget->selectionModel()->selection() );

    QList<int> rows;
    QSet<int> rows_set; // rows_set only for checking
    foreach( const QModelIndex & value, selection.indexes() ) {
        int id = value.row();
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
          ui->tableWidget->removeRow( current);
          prev = current;
       }
    }
}

void MainWindow::on_toolButton_clicked()
{
    int r = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    ui->tableWidget->setRowHeight(r, ui->tableWidget->rowHeight(r)+5);

    {
        ui->tableWidget->setCellWidget(r, 0, new QLineEdit);
    }

    {
        ui->tableWidget->setCellWidget(r, 1, new LineEdit);

    }

    {

        QWidget * w = new QWidget();
        QHBoxLayout *l = new QHBoxLayout();
        l->setAlignment( Qt::AlignCenter );
        l->addWidget( new QCheckBox );
        w->setLayout( l );
        ui->tableWidget->setCellWidget(r, 2, w);

    }

}


void MainWindow::on_action_open_triggered()
{
    auto actors = ActorsList::Inctance().lock();
    assert(actors);

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Открыть список персонажей"), "/", tr("Text files (*.txt)"));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::stringstream ss;
        ss << "Не удалось открыть файл '" << fileName.toUtf8().data() << "' на чтение";
        QMessageBox::critical(this, "Ошибка", ss.str().c_str());
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8"); // change the file codec to UTF-8.
    actors->Native().clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        // Processing line..
        if (line.indexOf(';') != -1)
        {
            std::stringstream ss;
            ss <<  "Наличие символа ';' в имени персонажа недопустимо. Имя '" << line.toUtf8().data() << "' будет проигнорировано.";
            QMessageBox::warning(this, "Предупреждение", ss.str().c_str());
            continue;
        }
        actors->Native().insert(ActorName(line));
    }

}

void MainWindow::on_action_save_triggered()
{
    auto actors = ActorsList::Inctance().lock();
    assert(actors);

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Сохранить список персонажей"), "/", tr("Text files (*.txt)"));

    QFile fileOut(fileName);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        std::stringstream ss;
        ss << "Не удалось открыть файл '" << fileName.toUtf8().data() << "' на запись";
        QMessageBox::critical(this, "Ошибка", ss.str().c_str());
        return;
    }

    QTextStream streamFileOut(&fileOut);
    streamFileOut.setCodec("UTF-8");
    for (const auto& Value : actors->Native())
    {
        streamFileOut << Value.Get() << endl;
    }
    streamFileOut.flush();

    fileOut.close();
}

void MainWindow::on_action_close_triggered()
{
    auto actors = ActorsList::Inctance().lock();
    assert(actors);
    actors->Native().clear();
}


void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_action_exit_triggered()
{
    QApplication::quit();
}
