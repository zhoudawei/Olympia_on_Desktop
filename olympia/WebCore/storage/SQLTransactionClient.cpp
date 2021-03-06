/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SQLTransactionClient.h"

#if ENABLE(DATABASE)

#include "Chrome.h"
#include "ChromeClient.h"
#include "Database.h"
#include "DatabaseThread.h"
#include "DatabaseTracker.h"
#include "Document.h"
#include "Page.h"
#include "SQLTransaction.h"

namespace WebCore {

void SQLTransactionClient::didCommitTransaction(SQLTransaction* transaction)
{
    ASSERT(currentThread() == transaction->database()->scriptExecutionContext()->databaseThread()->getThreadID());
    Database* database = transaction->database();
#if OS(OLYMPIA)
    DatabaseTracker::tracker(database->groupName()).scheduleNotifyDatabaseChanged(
        database->securityOrigin(), database->stringIdentifier());
#else
    DatabaseTracker::tracker().scheduleNotifyDatabaseChanged(
        database->securityOrigin(), database->stringIdentifier());
#endif
}

void SQLTransactionClient::didExecuteStatement(SQLTransaction* transaction)
{
    ASSERT(currentThread() == transaction->database()->scriptExecutionContext()->databaseThread()->getThreadID());
#if OS(OLYMPIA)
    Database* database = transaction->database();
    DatabaseTracker::tracker(database->groupName()).databaseChanged(database);
#else
    DatabaseTracker::tracker().databaseChanged(transaction->database());
#endif
}

bool SQLTransactionClient::didExceedQuota(SQLTransaction* transaction)
{
    ASSERT(transaction->database()->scriptExecutionContext()->isContextThread());
    Database* database = transaction->database();

#if OS(OLYMPIA)
    unsigned long long currentQuota = DatabaseTracker::tracker(database->groupName()).quotaForOrigin(database->securityOrigin());
    database->scriptExecutionContext()->databaseExceededQuota(database->stringIdentifier());
    unsigned long long newQuota = DatabaseTracker::tracker(database->groupName()).quotaForOrigin(database->securityOrigin());
#else
    unsigned long long currentQuota = DatabaseTracker::tracker().quotaForOrigin(database->securityOrigin());
    database->scriptExecutionContext()->databaseExceededQuota(database->stringIdentifier());
    unsigned long long newQuota = DatabaseTracker::tracker().quotaForOrigin(database->securityOrigin());
#endif

    return (newQuota > currentQuota);
}

}

#endif // ENABLE(DATABASE)
