#ifndef MODEL_MODEL_H
#define MODEL_MODEL_H

#include <QString>
#include "account.h"
#include "accountclosure.h"

namespace model { class Model; }

// TODO replace chompchomp with readXXX and writeXXX methods so that we can
// have error fallout that is in line with Qt's ways (no exceptions, no errorbit or failbit on streams, etc.)


class model::Model
{
public:
   // read the model data from the file
   Model(QString const& fileName);

   // write the model data to the file
   ~Model();

   // queries to operate on the data
//   QStringList accounts();
//   int totalWorth();
   // etc... queries to operate on the data

protected:
   // you could inherit from the model to work on the data directly
   QHash<QString, Account> m_accounts;
   QHash<QString, AccountClosure> m_accountClosures;

private:
   QString m_fileName;
};

#endif
