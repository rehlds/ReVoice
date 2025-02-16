#include "precompiled.h"

cvar_t *pcv_mp_logecho;
char g_szLogstring[2048];

void LCPrintf(bool critical, const char *fmt, ...)
{
	va_list argptr;
	const int prefixlen = 11; //sizeof(LOG_PREFIX) - 1;

	va_start(argptr, fmt);
	vsnprintf(g_szLogstring + prefixlen, sizeof(g_szLogstring) - prefixlen, fmt, argptr);
	va_end(argptr);

	bool bNeedWriteInConsole = critical;
	bool bNeedWriteInLog = critical;

	if (bNeedWriteInConsole && bNeedWriteInLog && g_RehldsSvs && g_RehldsSvs->IsLogActive())
	{
		if (pcv_mp_logecho && pcv_mp_logecho->value != 0.0)
			bNeedWriteInConsole = false;
	}

	if (bNeedWriteInConsole)
		SERVER_PRINT(g_szLogstring);

	if (bNeedWriteInLog)
		ALERT(at_logged, g_szLogstring);
}

bool Revoice_Utils_Init()
{
	pcv_mp_logecho = g_engfuncs.pfnCVarGetPointer("mp_logecho");
	strcpy(g_szLogstring, LOG_PREFIX);

	return true;
}

char *trimbuf(char *str)
{
	char *ibuf = str;
	int i = 0;

	if (str == NULL)
		return NULL;

	for (ibuf = str; *ibuf && (byte)(*ibuf) < (byte)0x80 && isspace(*ibuf); ++ibuf)
		;

	i = strlen(ibuf);

	if (str != ibuf)
		memmove(str, ibuf, i);

	str[i] = 0;

	while (--i >= 0) {
		if (!isspace(ibuf[i]))
			break;
	}

	ibuf[++i] = 0;

	return str;
}

uint32 crc32(const void *buf, unsigned int bufLen)
{
	CRC32_t hCrc;
	g_engfuncs.pfnCRC32_Init(&hCrc);
	g_engfuncs.pfnCRC32_ProcessBuffer(&hCrc, (void*)buf, bufLen);
	hCrc = g_engfuncs.pfnCRC32_Final(hCrc);
	return hCrc;
}

void NormalizePath(char *path)
{
	for (char *cp = path; *cp; cp++) {
		if (isupper(*cp))
			*cp = tolower(*cp);

		if (*cp == '\\')
			*cp = '/';
	}
}

void util_syserror(const char *fmt, ...)
{
	va_list	argptr;
	char buf[4096];

	va_start(argptr, fmt);
	vsnprintf(buf, ARRAYSIZE(buf) - 1, fmt, argptr);
	buf[ARRAYSIZE(buf) - 1] = 0;
	va_end(argptr);

	LCPrintf(true, "ERROR: %s", buf);

	int *null = 0;
	*null = 0;
	exit(-1);
}
