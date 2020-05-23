#include "rphtmlreporter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include "reportaccount.h"

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
   m_outFile.close();
}

bool RPHtmlReporter::processReport(ReportAccount const& account)
{
   m_outFile.close();
   removeById(account);
   // TODO this is bad file handling
   m_outFile = ofstream {m_reportsDir + "/Account Report [" +
               account.account() + "] -- " +
               account.dateRange().endDate().toString("yyyy-MM-dd") + " " +
               idStr(account) + ".html"};

   m_outFile << "<html><head><title>Account Report</title></head>" << endl;
   m_outFile << "<body><h1>Account Report</h1>" << endl;
   m_outFile << "<p>Account: " << account.account() << "</p>" << endl;
   m_outFile << "<p>Date Range: "
             << account.dateRange().startDate().toString(m_dateFormat) << " - "
             << account.dateRange().endDate().toString(m_dateFormat) << "</p>"
             << endl;
   m_outFile << "<p>Starting Balance: " << account.balanceStart().toString()
             << "</p>" << endl;
   m_outFile << "<p>Ending Balance: " << account.balanceEnd().toString()
             << "</p>" << endl;
   m_outFile << "<table><tr><th>Date</th><th>Cleared</th><th>Text</th><th>Amount</th><th>Balance</th></tr>" << endl;
   m_balance = account.balanceStart();
   return true;
}

void RPHtmlReporter::processReport(ReportAccountEntry const& entry)
{
   removeById(entry);
   m_balance += entry.amount();

   m_outFile << tr(td(entry.date().toString(m_dateFormat)) +
                   td(entry.cleared() ? "*" : "?") +
                   td(entry.text()) +
                   td(entry.amount().toString()) +
                   td(m_balance.toString())) << endl;
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
