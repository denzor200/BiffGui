#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>

namespace Ui {
class MainWindow;
class Hider;
}

class Hider : public QObject
{
    Q_OBJECT
public:
    Hider(QObject * parent = nullptr) : QObject(parent) {}
    bool eventFilter(QObject *, QEvent * ev) {
        return ev->type() == QEvent::Paint;
    }
    void hide(QWidget * w) {
        w->installEventFilter(this);
        w->update();
    }
    void unhide(QWidget * w) {
        w->removeEventFilter(this);
        w->update();
    }
    Q_SLOT void hideWidget()
    {
        QObject * s = sender();
        if (s->isWidgetType()) { hide(qobject_cast<QWidget*>(s)); }
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Hider m_hider;
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
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
