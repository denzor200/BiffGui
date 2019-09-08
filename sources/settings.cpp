#include "settings.h"
#include "ui_dialog.h"

// TODO: fix it
Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

