#include "messageui.h"
#include "ui_messageui.h"

#include "model/ledgeritem.h"

MessageUI::MessageUI(QWidget* parent) :
   QWidget(parent),
   ui(new Ui::MessageUI)
{
   ui->setupUi(this);
}

MessageUI::~MessageUI()
{
   delete ui;
}

void MessageUI::appendMessage(QString const& msg)
{
   ui->messages->appendPlainText(msg);
}

void MessageUI::appendMessage(LedgerItem const& item, QString const& msg)
{
   ui->messages->appendPlainText(QString("%1, file '%2', line %3")
                                 .arg(msg)
                                 .arg(item.fileName())
                                 .arg(item.lineNum()));
}
