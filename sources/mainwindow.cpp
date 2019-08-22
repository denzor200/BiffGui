#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolButton>
#include "lineedit.h"

#include <algorithm>
#include <sstream>

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

void MainWindow::on_toolButtonCreate_clicked()
{

}

void MainWindow::on_toolButtonDestroy_clicked()
{

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

        auto l = new QLineEdit;
        //m_hider.hide(l);
        ui->tableWidget->setCellWidget(r, 0, l);

    }

    /*{
        auto LE = new QLineEdit;
        auto B = new QToolButton;


        QWidget * w = new QWidget();
        QHBoxLayout *l = new QHBoxLayout();
        l->setAlignment( Qt::AlignRight );
        l->addWidget( LE );
        l->addWidget( B );
        w->setLayout( l );

        ui->tableWidget->setCellWidget(r, 1, w);

    }*/


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

void MainWindow::on_OpenActorsList_clicked()
{


}
void MainWindow::on_SaveActorsList_clicked()
{


}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_action_exit_triggered()
{
    QApplication::quit();
}
