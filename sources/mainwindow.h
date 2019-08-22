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
    void on_toolButtonCreate_clicked();

    void on_toolButtonDestroy_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_OpenActorsList_clicked();
    void on_SaveActorsList_clicked();

    void on_actionAbout_Qt_triggered();

    void on_action_exit_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
