#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>

#include "MainTableModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_close_triggered();

    void on_actionAbout_Qt_triggered();
    void on_action_exit_triggered();

    void on_action_settings_triggered();

    void on_action_open_persons_triggered();
    void on_action_save_persons_triggered();
    void on_action_close_persons_triggered();

    void on_action_close_all_triggered();

    void on_action_open_table_triggered();
    void on_action_save_table_triggered();
    void on_action_close_table_triggered();

    void on_action_generate_doc_triggered();

    void on_action_save_individual_triggered();

    void on_action_make_shared_flag_triggered();

    void on_action_make_individual_flag_triggered();

    void on_pushButton_MakeDoc_clicked();

    void on_toolButton_Insert_clicked();

    void on_toolButton_Delete_clicked();

    void on_toolButton_Reverse_clicked();

private:
    Ui::MainWindow* ui;
    MainTableModel*             m_Model;
    MainTableModel_Reversed*    m_ModelReversed;
};

#endif // MAINWINDOW_H
