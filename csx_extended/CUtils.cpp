#include "CHeader.h"

CRITICAL_SECTION *log_cs = nullptr;

void MF_SyncLog(const char *fmt, ...)
{
	if(log_cs == nullptr)
	{
		log_cs = new CRITICAL_SECTION;
		if(InitializeCriticalSectionAndSpinCount(log_cs, 0x00000400) == FALSE)
			MF_Log("RankSystem: Couldn't create critical section");
	}

	// Enter Critical Section
	EnterCriticalSection(log_cs);

	char msg[3072];
	va_list arglst;
	va_start(arglst, fmt);
	vsnprintf(msg, sizeof(msg) - 1, fmt, arglst);
	va_end(arglst);

	g_fn_Log("[%s] %s", MODULE_LOGTAG, msg);

	// Leave Critical Section
	LeaveCriticalSection(log_cs);
}

void MF_SyncLogError(AMX *amx, int err, const char *fmt, ...)
{
	if(log_cs == nullptr)
	{
		log_cs = new CRITICAL_SECTION;
		if(InitializeCriticalSectionAndSpinCount(log_cs, 0x00000400) == FALSE)
			MF_Log("RankSystem: Couldn't create critical section");
	}

	// Enter Critical Section
	EnterCriticalSection(log_cs);

	char msg[3072];
	va_list arglst;
	va_start(arglst, fmt);
	vsnprintf(msg, sizeof(msg) - 1, fmt, arglst);
	va_end(arglst);

	g_fn_LogErrorFunc(amx, err, "[%s] %s", MODULE_LOGTAG, msg);

	// Leave Critical Section
	LeaveCriticalSection(log_cs);
}