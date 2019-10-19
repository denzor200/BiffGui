#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QSortFilterProxyModel>

#include "maintablemodel.h"
#include "srt.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void makeDoc();
    void ReverseTable();

    void InsertRow();
    void RemoveAllSelectedRows();

    void SetDenyValueToAll(bool value);

    // В возвращаемом значении флаг - давал ли пользователь согласие на выволнение операции
    bool tryCloseFile();

    void showFileOpenRError(const QString& fileName) const;
    void showFileOpenWError(const QString& fileName) const;
    void showTempFileOpenWError() const;

    void SaveDecisions(QByteArray* StreamOut) const;

    void SaveSubbtitle(const QString& outFileName, const QString& ext, bool isIndividual);

    void ExportIndividualSubbtitles(const QString& ext);

private slots:
    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_close_triggered();

    void on_actionAbout_Qt_triggered();
    void on_action_exit_triggered();

    void on_action_settings_triggered();

    void on_action_save_persons_triggered();

    void on_action_open_table_triggered();
    void on_action_save_table_triggered();

    void on_action_generate_doc_triggered();

    void on_action_make_shared_flag_triggered();
    void on_action_make_individual_flag_triggered();

    void on_toolButton_Insert_clicked();
    void on_toolButton_Delete_clicked();
    void on_toolButton_Reverse_clicked();

    void on_commandLinkButton_makeDoc_clicked();

    void on_action_select_all_triggered();
    void on_action_about_this_triggered();
    void on_action_deny_all_triggered();
    void on_action_deny_disable_all_triggered();
    void on_action_remove_all_links_triggered();
    void on_action_register_software_triggered();
    void on_action_add_triggered();
    void on_action_remove_triggered();

    void on_action_ass_triggered();

    void on_action_srt_triggered();

    void on_action_make_clean_flag_triggered();

private:
    Ui::MainWindow* ui;
    MainTableModelsManager*     m_ModelsMgr;
    QSortFilterProxyModel       m_ProxyModel;
    QSortFilterProxyModel       m_ProxyModelReversed;
    bool                        m_IsReversed = false;

    struct SubbtitleContext
    {
        QString FileName;
        QString CtrlData;

        // Следующие поля актуальны только для формата .srt
        QStringList UsersDecisions;

        SrtFormat::MarkupType MarkupType;
        bool MarkupTypeInitialized;
    };
    SubbtitleContext*           m_OpenedSubbtitle = nullptr;
};

#endif // MAINWINDOW_H
