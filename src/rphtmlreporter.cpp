#include "rphtmlreporter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include "reportaccount.h"
#include "reportbudget.h"

using std::endl;
using std::ofstream;
using std::string;
using std::stringstream;

RPHtmlReporter::RPHtmlReporter(string const& reportsDir,
                                     string const& dateFormat) :
   m_dateFormat(dateFormat),
   m_reportsDir(reportsDir)
{
}

void RPHtmlReporter::finish()
{
   m_accountFile.close();
   m_budgetFile.close();
}

bool RPHtmlReporter::processReport(ReportAccount const& account)
{
   m_accountFile.close();
   removeById(account);
   // TODO this is bad file handling
   m_accountFile = ofstream {m_reportsDir + "/Account Report [" +
               account.account() + "] -- " +
               account.dateRange().endDate().toString("yyyy-MM-dd") + " " +
               idStr(account) + ".html"};

   m_accountFile << "<html><head><title>Account Report</title></head>" << endl;
   m_accountFile << "<body><h1>Account Report</h1>" << endl;
   m_accountFile << "<p>Account: " << account.account() << "</p>" << endl;
   m_accountFile << "<p>Date Range: "
             << account.dateRange().startDate().toString(m_dateFormat) << " - "
             << account.dateRange().endDate().toString(m_dateFormat) << "</p>"
             << endl;
   m_accountFile << "<p>Starting Balance: " << account.balanceStart().toString()
             << "</p>" << endl;
   m_accountFile << "<p>Ending Balance: " << account.balanceEnd().toString()
             << "</p>" << endl;
   m_accountFile << "<table><tr><th>Date</th><th>Cleared</th><th>Text</th><th>Amount</th><th>Balance</th></tr>" << endl;
   m_balance = account.balanceStart();
   return true;
}

void RPHtmlReporter::processReport(ReportAccountEntry const& entry)
{
   removeById(entry);
   m_balance += entry.amount();

   m_accountFile << tr(td(entry.date().toString(m_dateFormat)) +
                   td(entry.cleared() ? "*" : "?") +
                   td(entry.text()) +
                   td(entry.amount().toString()) +
                   td(m_balance.toString())) << endl;
}

bool RPHtmlReporter::processReport(ReportBudget const& budget)
{
   m_budgetFile.close();
   removeById(budget);
   // TODO this is bad file handling
   m_budgetFile = ofstream {m_reportsDir + "/Budget Report -- " +
                  budget.dateRange().endDate().toString("yyyy-MM-dd") +
                  " " + idStr(budget) + ".html"};

   m_budgetFile << "<html><head><title>Budget Report</title></head>" << endl;
   m_budgetFile << "<body><h1>Budget Report</h1>" << endl;
   m_budgetFile << "<p>Budget Period: "
                << budget.dateRange().startDate().toString(m_dateFormat)
                << " - " << budget.dateRange().endDate().toString(m_dateFormat)
                << "</p>" << endl;
   return true;
}

void RPHtmlReporter::processReport(ReportBudgetCancelEntry const& entry)
{
   m_budgetFile << "<p>Cancelled goal '" << entry.goal() << "' in category '"
                << entry.category() << "'.  Adding "
                << entry.goalBalance().toString() << " to category balance "
                << entry.categoryStartBalance().toString()
                << " for total balance of "
                << entry.categoryEndBalance().toString() << "</p>" << endl;
}

void RPHtmlReporter::processReport(ReportBudgetCloseEntry const& entry)
{
   m_budgetFile << "<p>Closed category '" << entry.category() << "'";
   if (entry.owner() != "")
   {
      m_budgetFile << " owned by '" << entry.owner() << "'";
   }
   m_budgetFile << ".  Adding " << entry.categoryBalance().toString()
                << " to available balance "
                << entry.availableStartBalance().toString()
                << " for total balance of "
                << entry.availableEndBalance().toString() << "</p>" << endl;
}

void RPHtmlReporter::processReport(ReportBudgetWarningEntry const& entry)
{
   m_budgetFile << "<p>WARNING: In file '" << entry.fileName() << "' on line "
                << entry.lineNumber() << ", " << entry.text() << "</p>" << endl;
}

string RPHtmlReporter::idStr(Report const& report)
{
   stringstream ss;
   ss << "--" << report.id() << "--";
   return ss.str();
}

// TODO rework this with c++17
void RPHtmlReporter::removeById(Report const& report)
{
   DIR* dir = opendir(m_reportsDir.c_str());
   if (!dir) return;
   for (struct dirent* d = readdir(dir); d; d = readdir(dir))
   {
      auto filename = string {d->d_name};
      if (filename.find(idStr(report)) != string::npos)
      {
         filename = m_reportsDir + '/' + filename;
         remove(filename.c_str());
      }
   }
   closedir(dir);
}

string RPHtmlReporter::td(string const& s)
{
   return "<td>" + s + "</td>";
}

string RPHtmlReporter::tr(string const& s)
{
   return "<tr>" + s + "</tr>";
}
