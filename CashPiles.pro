TEMPLATE = app
CONFIG -= qt
CONFIG += console
HEADERS += \
   src/cashpiles.h \
   src/csvreader.h \
   src/currency.h \
   src/date.h \
   src/daterange.h \
   src/filereader.h \
   src/filereaderregex.h \
   src/filewriter.h \
   src/interval.h \
   src/ipaccountbalancer.h \
   src/ipbudgetallocator.h \
   src/ipdatevalidator.h \
   src/iptransactioncategorizer.h \
   src/itemprocessor.h \
   src/ledger.h \
   src/ledgeraccount.h \
   src/ledgeraccountbalance.h \
   src/ledgerblank.h \
   src/ledgerbudget.h \
   src/ledgerbudgetcancelentry.h \
   src/ledgerbudgetcloseentry.h \
   src/ledgerbudgetentry.h \
   src/ledgerbudgetgoalentry.h \
   src/ledgerbudgetgoalsentry.h \
   src/ledgerbudgetincomeentry.h \
   src/ledgerbudgetreserveamountentry.h \
   src/ledgerbudgetreservepercententry.h \
   src/ledgerbudgetroutineentry.h \
   src/ledgerbudgetwithholdingentry.h \
   src/ledgercomment.h \
   src/ledgeritem.h \
   src/ledgertransaction.h \
   src/ledgertransactionv2.h \
   src/model/account.h \
   src/model/accountclosed.h \
   src/model/accountclosure.h \
   src/model/accountexists.h \
   src/model/accountnotexists.h \
   src/model/accountopen.h \
   src/model/accountstatement.h \
   src/model/accountwrongtype.h \
   src/model/blank.h \
   src/model/budgetaccount.h \
   src/model/budgetcancelentry.h \
   src/model/budgetcategoryentry.h \
   src/model/budgetentry.h \
   src/model/budgetgoalentry.h \
   src/model/budgetgoalsentry.h \
   src/model/budgetincomeentry.h \
   src/model/budgetperiod.h \
   src/model/budgetuninitialized.h \
   src/model/categorynotexists.h \
   src/model/categoryownernotexists.h \
   src/model/model.h \
   src/model/modeldata.h \
   src/model/modelreader.h \
   src/model/modelreaderformat.h \
   src/model/modelregex.h \
   src/model/referenceaccount.h \
   src/model/referencetransaction.h \
   src/model/rubbish.h \
   src/model/transaction.h \
   src/model/transactionaccountentry.h \
   src/model/transactioncategoryentry.h \
   src/model/transactioncategorytrackingentry.h \
   src/model/transactionentry.h \
   src/model/transactionflag.h \
   src/model/transactionflaginvalid.h \
   src/model/transactionnotexists.h \
   src/model/transactionownerentry.h \
   src/model/transactionownertrackingentry.h \
   src/report.h \
   src/reportaccount.h \
   src/reportbudget.h \
   src/reporter.h \
   src/reportprocessor.h \
   src/rphtmlreporter.h \
   src/texttable.h \
   src/ynabregisterreader.h
SOURCES += \
   src/cashpiles.cpp \
   src/csvreader.cpp \
   src/currency.cpp \
   src/date.cpp \
   src/daterange.cpp \
   src/filereader.cpp \
   src/filereaderregex.cpp \
   src/filewriter.cpp \
   src/interval.cpp \
   src/ipaccountbalancer.cpp \
   src/ipbudgetallocator.cpp \
   src/ipdatevalidator.cpp \
   src/iptransactioncategorizer.cpp \
   src/itemprocessor.cpp \
   src/ledger.cpp \
   src/ledgeraccount.cpp \
   src/ledgeraccountbalance.cpp \
   src/ledgerblank.cpp \
   src/ledgerbudget.cpp \
   src/ledgerbudgetcancelentry.cpp \
   src/ledgerbudgetcloseentry.cpp \
   src/ledgerbudgetentry.cpp \
   src/ledgerbudgetgoalentry.cpp \
   src/ledgerbudgetgoalsentry.cpp \
   src/ledgerbudgetincomeentry.cpp \
   src/ledgerbudgetreserveamountentry.cpp \
   src/ledgerbudgetreservepercententry.cpp \
   src/ledgerbudgetroutineentry.cpp \
   src/ledgerbudgetwithholdingentry.cpp \
   src/ledgercomment.cpp \
   src/ledgeritem.cpp \
   src/ledgertransaction.cpp \
   src/ledgertransactionv2.cpp \
   src/model/account.cpp \
   src/model/accountclosed.cpp \
   src/model/accountclosure.cpp \
   src/model/accountexists.cpp \
   src/model/accountnotexists.cpp \
   src/model/accountopen.cpp \
   src/model/accountstatement.cpp \
   src/model/accountwrongtype.cpp \
   src/model/blank.cpp \
   src/model/budgetaccount.cpp \
   src/model/budgetcancelentry.cpp \
   src/model/budgetcategoryentry.cpp \
   src/model/budgetentry.cpp \
   src/model/budgetgoalentry.cpp \
   src/model/budgetgoalsentry.cpp \
   src/model/budgetincomeentry.cpp \
   src/model/budgetperiod.cpp \
   src/model/budgetuninitialized.cpp \
   src/model/categorynotexists.cpp \
   src/model/categoryownernotexists.cpp \
   src/model/model.cpp \
   src/model/modeldata.cpp \
   src/model/modelreader.cpp \
   src/model/modelreaderformat.cpp \
   src/model/modelregex.cpp \
   src/model/referenceaccount.cpp \
   src/model/referencetransaction.cpp \
   src/model/rubbish.cpp \
   src/model/transaction.cpp \
   src/model/transactionaccountentry.cpp \
   src/model/transactioncategoryentry.cpp \
   src/model/transactioncategorytrackingentry.cpp \
   src/model/transactionentry.cpp \
   src/model/transactionflaginvalid.cpp \
   src/model/transactionnotexists.cpp \
   src/model/transactionownerentry.cpp \
   src/model/transactionownertrackingentry.cpp \
   src/report.cpp \
   src/reportaccount.cpp \
   src/reportbudget.cpp \
   src/reporter.cpp \
   src/reportprocessor.cpp \
   src/rphtmlreporter.cpp \
   src/texttable.cpp \
   src/ynabregisterreader.cpp
TARGET = cashpiles
