#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui { class SettingsDialog; }
class QSettings;

class SettingsDialog : public QDialog
{
   Q_OBJECT

public:
   enum class Mode
   {
      NATIVE,
      YNAB
   };

public:
   explicit SettingsDialog(QWidget* parent = 0);
   ~SettingsDialog();

signals:
   void fileSelected(Mode mode, QString const& filename);

private slots:
   void on_cashPilesRadio_toggled(bool checked);
   void on_fileBrowse_clicked();
   void on_fileName_textChanged(QString const& text);
   void on_ynabRadio_toggled(bool checked);

private:
   Ui::SettingsDialog* ui;
   QSettings* m_settings;
};

#endif
