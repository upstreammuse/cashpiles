#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>
#include <QSettings>

namespace
{
   QString const MODE_KEY = "Mode";
   QString const MODE_VALUE_CASHPILES = "CashPiles";
   QString const MODE_VALUE_YNAB = "YNAB";

   QString const CASHPILES_FILENAME_KEY = "CashPilesFilename";
   QString const YNAB_FILENAME_KEY = "YNABFilename";
}

SettingsDialog::SettingsDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::SettingsDialog),
   m_settings(new QSettings("upstreammuse", "CashPiles", this))
{
   ui->setupUi(this);
   if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_CASHPILES)
   {
      ui->cashPilesRadio->click();
   }
   else if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_YNAB)
   {
      ui->ynabRadio->click();
   }
   else
   {
      ui->cashPilesRadio->click();
   }
}

SettingsDialog::~SettingsDialog()
{
   delete ui;
}

void SettingsDialog::on_cashPilesRadio_toggled(bool checked)
{
   if (checked)
   {
      m_settings->setValue(MODE_KEY, MODE_VALUE_CASHPILES);
      ui->fileName->setText(
               m_settings->value(CASHPILES_FILENAME_KEY).toString());
   }
}

void SettingsDialog::on_fileBrowse_clicked()
{
   QString filename(QFileDialog::getOpenFileName(this));
   if (filename.isNull())
   {
      return;
   }
   if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_CASHPILES)
   {
      m_settings->setValue(CASHPILES_FILENAME_KEY, filename);
   }
   else if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_YNAB)
   {
      m_settings->setValue(YNAB_FILENAME_KEY, filename);
   }
   else
   {
      Q_ASSERT(false);
   }
   ui->fileName->setText(filename);
}

void SettingsDialog::on_fileName_textChanged(QString const& text)
{
   Mode mode;
   if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_CASHPILES)
   {
      mode = Mode::NATIVE;
   }
   else if (m_settings->value(MODE_KEY).toString() == MODE_VALUE_YNAB)
   {
      mode = Mode::YNAB;
   }
   else
   {
      Q_ASSERT(false);
   }
   emit fileSelected(mode, text);
}

void SettingsDialog::on_ynabRadio_toggled(bool checked)
{
   if (checked)
   {
      m_settings->setValue(MODE_KEY, MODE_VALUE_YNAB);
      ui->fileName->setText(
               m_settings->value(YNAB_FILENAME_KEY).toString());
   }
}
