#define LUA_CORE

#include "lua.h"
#include "lapi.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lpacker.h"
#include <string.h>

#define FAILED_JUMP(Condition)\
	do	\
	{	\
		if (!(Condition))\
			goto Exit0;	\
	} while (0)

typedef unsigned char	DATA;
typedef unsigned short	OFFSET;

#define TRUE	1
#define FALSE	0

int lua_setValue(lua_State* L, TValue* v, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount);
int lua_packValue(lua_State* L, TValue* v, const TValue* v2, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount, DATA** pPos);

int lua_setTableValue(lua_State* L, Table* t, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount)
{
	int nRet;
	int nResult = FALSE;
	int i = 0;
	int oldasize = t->sizearray;
	int oldhsize = t->lsizenode;
	Node *node = t->node;

	for (i = 0; i < oldasize; i++) 
	{
		TValue* v = &t->array[i];
		if (ttistable(v))
		{
			nRet = lua_setTableValue(L, hvalue(v), pBuffer, dwMaxBufferSize, dwMaxValueCount);
			FAILED_JUMP(nRet);
		}
		else if (ttisnumber(v))
		{
			nRet = lua_setValue(L, v, pBuffer, dwMaxBufferSize, dwMaxValueCount);
			FAILED_JUMP(nRet);
		}
	}

	for (i = 0; i < sizenode(t); i++)
	{
		Node* n = gnode(t, i);
		TValue* v = gval(n);
		if (ttistable(v))
		{
			nRet = lua_setTableValue(L, hvalue(v), pBuffer, dwMaxBufferSize, dwMaxValueCount);
			FAILED_JUMP(nRet);
		}
		else if (ttisnumber(v))
		{
			nRet = lua_setValue(L, v, pBuffer, dwMaxBufferSize, dwMaxValueCount);
			FAILED_JUMP(nRet);
		}
	}

	nResult = TRUE;
Exit0:
	return nResult;
}

int lua_setValue(lua_State* L, TValue* v, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount)
{
	int			nResult	= FALSE;
	int			nIdx;
	DATA		byType;
	OFFSET		uOffset;
	OFFSET		uDataOffset;
	
	FAILED_JUMP(ttisnumber(v));

	nIdx = (int)nvalue(v);
	FAILED_JUMP(nIdx <= dwMaxValueCount);

	uOffset = ((OFFSET*)pBuffer)[nIdx - 1];
	FAILED_JUMP(uOffset > 0 && uOffset < dwMaxBufferSize);

	byType = pBuffer[uOffset];
	uDataOffset = uOffset + (OFFSET)sizeof(byType);

	switch (byType)
	{
		case LUA_TNUMBER:
		{
			double fValue = 0.0f;

			FAILED_JUMP(dwMaxBufferSize - uDataOffset >= sizeof(fValue));
			memcpy(&fValue, pBuffer + uDataOffset, sizeof(fValue));
			v->value.n = fValue;
			break;
		}
		case LUA_TSTRING:
		{
			TValue str;
			OFFSET len = 0;

			FAILED_JUMP(dwMaxBufferSize - uDataOffset >= sizeof(len));
			memcpy(&len, pBuffer + uDataOffset, sizeof(len));

			FAILED_JUMP(dwMaxBufferSize - uDataOffset >= sizeof(len) + len);
			setsvalue(L, &str, luaS_newlstr(L, pBuffer + uDataOffset + sizeof(len), len));
			setobj2t(L, v, &str);
			break;
		}
		case LUA_TBOOLEAN:
		{
			TValue boolean;
			DATA bValue;

			FAILED_JUMP(dwMaxBufferSize - uDataOffset >= sizeof(bValue));
			memcpy(&bValue, pBuffer + uDataOffset, sizeof(bValue));
			setbvalue(&boolean, (int)bValue);
			setobj2t(L, v, &boolean);
			break;
		}
		case LUA_TNIL:
		{
			setnilvalue(v);
			break;
		}
		default:
		{
			FAILED_JUMP(FALSE);
			break;
		}
	}

	nResult = TRUE;
Exit0:
	return nResult;
}

LUA_API int lua_SetTableValue(lua_State* L, int idx, void* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount)
{
	int		nResult = FALSE;
	int		nRet;
	StkId	t;

	FAILED_JUMP(sizeof(OFFSET) * dwMaxValueCount < dwMaxBufferSize);
	t = index2adr_ext(L, idx);
	FAILED_JUMP(ttistable(t));

	nRet = lua_setTableValue(L, hvalue(t), (DATA*)pBuffer, dwMaxBufferSize, dwMaxValueCount);
	FAILED_JUMP(nRet);

	nResult = TRUE;
Exit0:
	return nResult;
}

int lua_packTableValue(lua_State* L, Table* t1, Table* t2, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount, DATA** pPos)
{
	int nResult		= FALSE;
	int				i;
	int				nRet;
	TValue*			v;
	TValue			key;
	const TValue*	v2;

	int nArrayIdx = t1->sizearray < t2->sizearray ? t1->sizearray : t2->sizearray;
	for (i = 0; i < nArrayIdx; ++i)
	{
		v = &t1->array[i];
		if (ttisnil(v))
			continue;

		nRet = lua_packValue(L, v, &t2->array[i], pBuffer, dwMaxBufferSize, dwMaxValueCount, pPos);
		FAILED_JUMP(nRet);
	}

	for (i = nArrayIdx; i < t1->sizearray; ++i)
	{
		v = &t1->array[i];
		if (ttisnil(v))
			continue;

		setnvalue(&key, i + 1);
		v2 = luaH_get(t2, &key);

		nRet = lua_packValue(L, v, v2, pBuffer, dwMaxBufferSize, dwMaxValueCount, pPos);
		FAILED_JUMP(nRet);
	}

	for (i = 0; i < sizenode(t1); ++i)
	{
		Node* n = gnode(t1, i);
		v = gval(n);
		if (ttisnil(v))
			continue;

		v2 = luaH_get(t2, (const TValue*)gkey(n));

		nRet = lua_packValue(L, v, v2, pBuffer, dwMaxBufferSize, dwMaxValueCount, pPos);
		FAILED_JUMP(nRet);
	}

	nResult = TRUE;
Exit0:
	return nResult;
}

int lua_packValue(lua_State* L, TValue* v, const TValue* v2, DATA* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount, DATA** pPos)
{
	int		nResult = FALSE;
	DATA*	pbyPos = *pPos;
	int		nValueIndex;
	OFFSET	uOffset;


	if (ttistable(v) && ttistable(v2))
	{
		return lua_packTableValue(L, hvalue(v), hvalue(v2), pBuffer, dwMaxBufferSize, dwMaxValueCount, pPos);
	}

	FAILED_JUMP(ttisnumber(v));

	nValueIndex = (int)(nvalue(v)) - 1;
	FAILED_JUMP(nValueIndex >= 0 && nValueIndex < dwMaxValueCount);


	FAILED_JUMP((pbyPos + sizeof(OFFSET)) - pBuffer < dwMaxBufferSize);
	FAILED_JUMP(pbyPos - pBuffer <= USHRT_MAX);

	uOffset = (OFFSET)(pbyPos - pBuffer);
	memcpy(pBuffer + nValueIndex * sizeof(OFFSET), (void*)(&uOffset), sizeof(OFFSET));

	FAILED_JUMP((pbyPos + sizeof(DATA)) - pBuffer < dwMaxBufferSize);
	*pbyPos++ = v2->tt;

	switch (v2->tt)
	{
		case LUA_TNUMBER:
		{
			FAILED_JUMP((pbyPos + sizeof(double)) - pBuffer < dwMaxBufferSize);

			double fValue = nvalue(v2);
			memcpy(pbyPos, (void*)(&fValue), sizeof(double));
			pbyPos += sizeof(double);
			break;
		}
		case LUA_TBOOLEAN:
		{
			DATA byValue;
			FAILED_JUMP((pbyPos + sizeof(DATA)) - pBuffer < dwMaxBufferSize);

			byValue = (DATA)bvalue(v2);
			memcpy(pbyPos, (void*)(&byValue), sizeof(DATA));
			pbyPos += sizeof(DATA);
			break;
		}
		case LUA_TSTRING:
		{
			const char* pStr = svalue(v2);
			OFFSET uLen = tsvalue(v2)->len;

			FAILED_JUMP((pbyPos + sizeof(OFFSET) + uLen) - pBuffer < dwMaxBufferSize);
			memcpy(pbyPos, (void*)(&uLen), sizeof(OFFSET));
			pbyPos += sizeof(OFFSET);

			memcpy(pbyPos, pStr, uLen);
			pbyPos += uLen;

			break;
		}
		case LUA_TNIL:
		{
			break;
		}
		default:
		{
			FAILED_JUMP(FALSE);
			break;
		}
	}

	*pPos = pbyPos;
	nResult = TRUE;

Exit0:
	return nResult;
}

LUA_API int lua_PackTableValue(lua_State* L, int idx, int idx2, void* pBuffer, size_t dwMaxBufferSize, size_t dwMaxValueCount, size_t* pUsedSize)
{
	int			nResult = FALSE;
	int			nRet;
	StkId		t1;
	StkId		t2;
	DATA*		pPos;

	t1 = index2adr_ext(L, idx);
	t2 = index2adr_ext(L, idx2);

	FAILED_JUMP(ttistable(t1));
	FAILED_JUMP(ttistable(t2));
	FAILED_JUMP(sizeof(OFFSET) * dwMaxValueCount < dwMaxBufferSize);

	pPos = (DATA*)pBuffer + sizeof(OFFSET) * dwMaxValueCount;

	nRet = lua_packTableValue(L, hvalue(t1), hvalue(t2), (DATA*)pBuffer, dwMaxBufferSize, dwMaxValueCount, &pPos);
	FAILED_JUMP(nRet);

	*pUsedSize = (size_t)(pPos - (DATA*)pBuffer);
	nResult = TRUE;
Exit0:
	return nResult;
}