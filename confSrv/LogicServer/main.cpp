#include "StdAfx.h"
#include <dbghelp.h>
#include <mbctype.h>
#include <conio.h>
#include "PathThreadLocale.h"

const TCHAR szExceptionDumpFile[] = _T(".\\LogicServer.dmp");
BOOL SetupLogicServerConfig(CLogicServer *lpLogicServer, const char *pszConfigFileName = NULL);
VOID ServerMain(int argc, char **argv);
VOID ServiceMain(int argc, char** argv);

CLogicServer * GameServerEntry::g_pLogicServer  = NULL;


void TestMemory()
{
#ifdef _DEBUG

#else

	printf("***********************");
	for(int j=1; j<= 1; j++)
	{
		printf("Times=:%d\n",j);
		DWORD dwStart = GetTickCount();
		for(int i = 0; i < 1000000; i++) { char *p = (char*)malloc(i%1000000); free(p); } 

		DWORD dwEnd = GetTickCount();
		
		printf("100wMemoryTest:%d ms\n",dwEnd - dwStart);
		/*
		dwStart = GetTickCount();

		CBufferAllocator data ;
		for(int i = 0; i < 1000000; i++) 
		{ 
			char *p =(char*) data.AllocBuffer(i%1000000);

			data.FreeBuffer(PVOID (p));
		} 
		dwEnd = GetTickCount();
		printf("100wMemoryTest2:%d ms\n",dwEnd - dwStart);
		*/

	}
	
	

#endif
}


class CLogicService : public CWinService
{
public:
	CLogicService(LPCSTR pServiceName, DWORD dwServiceType = SERVICE_WIN32_OWN_PROCESS):CWinService(pServiceName, dwServiceType){};
	int AppMain()
	{
		SetCurrentDirectory("../");		//CWinService���ѵ�ǰĿ¼�ĳ�exe�����Ŀ¼������������תһ��		
		CFileLogger flog(_T("log/LogicServer_%s.log.html"), getCurrentTimeDesc());

		GameServerEntry::g_pLogicServer  = new CLogicServer();

		if ( SetupLogicServerConfig(GameServerEntry::g_pLogicServer) )
		{
			if ( GameServerEntry::g_pLogicServer->StartServer() )
			{
				//in_addr ia;
				//ia.S_un.S_addr = LOGIC_KN_VERSION;

				OutputMsg( rmTip, _T("-------------------------------------------") );
				OutputMsg( rmTip, _T("�߼������������ɹ������İ汾����%s"),CLogicServer::GetLogicVersion() );
				OutputMsg( rmTip, _T("-------------------------------------------") );
				while (!m_boServiceExit  && GameServerEntry::g_pLogicServer->IsStart() )
				{
					Sleep(1000);
				}
				GameServerEntry::g_pLogicServer->StopServer();
			}			
		}

		delete GameServerEntry::g_pLogicServer;

		return 0;
	}
};

int initLocalePath()
{
	//_MB_CP_SBCS   �������ַ�����Ϊ���ֽ��ַ����Ա�ͬʱ֧��UTF8�Լ�MBCS
	//Use single-byte code page. 
	//When the code page is set to _MB_CP_SBCS, a routine such as _ismbblead always returns false.
	_setmbcp(_MB_CP_SBCS);
	//��ؼ��� ����"C"locale���������ַ�����Ϊ���ֽ��ַ����Ա�ͬʱ֧��UTF8�Լ�MBCS
	return InstallThreadLocalePath("C");
}

int main(int argc, char** argv)
{		
	unsigned short version;
	WSADATA wsadata;
	version = MAKEWORD(2, 2);
	WSAStartup(version, &wsadata);

	SetMiniDumpFlag(MiniDumpWithFullMemory);
	SetUnhandledExceptionFilter( DefaultUnHandleExceptionFilter );	
	InitDefMsgOut();
	//InitChatMsgOut();
	CTimeProfMgr::getSingleton().InitMgr();
	TestMemory();
	//��װ�߳�UTF-8��locale����
	if (initLocalePath())
	{
		OutputMsg( rmError, _T("can not set locale path") );
		getc(stdin);
		return -2;
	}
	
	{
		if (argc == 2 && _tcsncicmp("/svc",argv[1],4)==0)
		{
			ServiceMain(argc, argv);
		}
				
		SetCurrentDirectory("../");
		ServerMain(argc, argv);
	}
	printf("CSingleObjectAllocStatMgr logToFile...\n");
	CSingleObjectAllocStatMgr::getSingleton().logToFile();
	printf("CounterManager logToFile...\n");
	CounterManager::getSingleton().logToFile();
	printf("HandleMgrCollector logToFile...\n");
	HandleMgrCollector::getSingleton().logToFile();
	printf("CTimeProfMgr logToFile...\n");
	CTimeProfMgr::getSingleton().clear();
	printf("CounterManager logToFile...\n");
	CounterManager::getSingleton().clear();	
	printf("CSingleObjectAllocStatMgr logToFile...\n");
	CSingleObjectAllocStatMgr::getSingleton().clear();
	//ClearChatMsgOut();
	printf("UninitDefMsgOut...\n");
	UninitDefMsgOut();

	printf("AbortCloseThread...\n");
	CloseServerHelper::GetInst().AbortCloseThread();
	
#ifdef	_MLIB_DUMP_MEMORY_LEAKS_
	_CrtDumpMemoryLeaks();
#endif
	WSACleanup();
	return 0;
}




VOID ServerMain(int argc, char **argv)
{	
	const char *pszLogdir = "log";
	if (argc >= 2)
		pszLogdir = argv[1];

	if (!FDOP::FileExists(pszLogdir))
	{
		if (!FDOP::DeepCreateDirectory(pszLogdir))
		{
			printf("error LogicServer::DeepCreateDirectory() fail\n");
			return;
		}
	}	

	CFileLogger flog(_T("%s/LogicServer_%s.log.txt"), pszLogdir, getCurrentTimeDesc());
	char logPath[MAX_PATH];
	_snprintf(logPath, MAX_PATH, "%s/quick", pszLogdir);
	LocalLogger::Inst().Initialize(logPath);

	char sPath[32];
	sprintf_s(sPath, sizeof(sPath), "stop.var");
	if (FDOP::FileExists(sPath))
	{
		DeleteFileA(sPath);
	}

	GameServerEntry::g_pLogicServer  = new CLogicServer();
	if ( SetupLogicServerConfig(GameServerEntry::g_pLogicServer) )
	{
		if ( GameServerEntry::g_pLogicServer->StartServer() )
		{
			TCHAR sCmdBuf[512];
			in_addr ia;
			ia.S_un.S_addr = LOGIC_KN_VERSION;
			bool bSaveFlag = false;

			OutputMsg( rmTip, _T("-------------------------------------------") );
			OutputMsg( rmTip, _T("�߼������������ɹ������İ汾����%s"), inet_ntoa(ia) );
			OutputMsg( rmTip, _T("quit�������ֹͣ�����˳�����") );
			OutputMsg( rmTip, _T("-------------------------------------------") );
			OutputMsg(rmTip,"Main thread id=%d",GetCurrentThreadId());

			while (GameServerEntry::g_pLogicServer->IsStart())
			{
 				sCmdBuf[0] = 0;

				if(_kbhit())
				{
					_getts(sCmdBuf);
				}
				else
				{
					if (FDOP::FileExists(sPath))
					{
						OutputMsg( rmTip, _T("stop server by sharefile") );
						CLogicServer::s_pLogicEngine->GetNetWorkHandle()->PostInternalMessage(SSM_STOP_SERVER_CMD);
						DeleteFileA(sPath);
					}

					Sleep(10000);
					continue;
				}
				
				//�˳�����
				if ( _tcsncicmp(sCmdBuf, _T("\\q"), 2) == 0 
					|| _tcsncicmp(sCmdBuf, _T("exit"), 4) == 0
					|| _tcsncicmp(sCmdBuf, _T("quit"), 4) == 0 )
				{
					OutputMsg( rmTip, _T("����ֹͣ�߼�����...") );
					CEntityManager::s_OpenNotDestroyNpc = 0;
					break;
				}
				else if (_tcsncicmp(sCmdBuf, _T("dmpexit"), 7) == 0)
				{
					OutputMsg( rmTip, _T("����ֹͣ�߼�����...") );
					bSaveFlag = true;
					break;
				}
				else if (_tcsncicmp(sCmdBuf, _T("memory"), 6) == 0)
				{
					GetGlobalLogicEngine()->DumpDataAllocator();
				}
				//��ʾ�ű��ڴ������״̬
				else if (_tcsncicmp(sCmdBuf, _T("tsms"), 4) == 0)
				{

					OutputMsg( rmTip, _T("---------------------------------------") );					
					OutputMsg( rmTip, _T("Script Memory Manager Status:") );					
					OutputMsg( rmTip, _T("  Block Count: %I64d"), ScriptMemoryManager::getAvaliableMemBlockCount());					
					OutputMsg( rmTip, _T("  Total Size : %I64d MB"), ScriptMemoryManager::getAvaliableMemSize()/1024/1024);					
					OutputMsg( rmTip, _T("---------------------------------------") );					
				}
				else if(_tcsncicmp(sCmdBuf,"spf",3) ==0 )
				{
					OutputMsg( rmTip, _T("log the perfermance ") );
					GetGlobalLogicEngine()->GetStatisticMgr()->LogTimeFile();
					CSingleObjectAllocStatMgr::getSingleton().logToFile();
					CounterManager::getSingleton().logToFile();
					HandleMgrCollector::getSingleton().logToFile();
					CTimeProfMgr::getSingleton().dump();
					flog.Dump();
				}
				else if(_tcsncicmp(sCmdBuf,"dmp",3) ==0 ) 
				{
					DebugBreak();
				}
				else if (_tcsncicmp(sCmdBuf, "smdn", 4) == 0) // ���ù�����߼�ѭ���ȷ���Ŀ
				{
					char *pParam = sCmdBuf + 4;
					while (*pParam == ' ')
						pParam++;
					if (pParam)
					{
						UINT_PTR nCount = atoi(pParam);
						if (nCount > 0)
							GetGlobalLogicEngine()->GetEntityMgr()->SetMonsterDivNum(nCount);
					}	
				}
				else if (_tcsncicmp(sCmdBuf, "sndn", 4) == 0) // ����NPC���߼�ѭ���ȷ���Ŀ
				{
					char *pParam = sCmdBuf + 4;
					while (*pParam == ' ')
						pParam++;
					if (pParam)
					{
						UINT_PTR nCount = atoi(pParam);
						if (nCount > 0)
							GetGlobalLogicEngine()->GetEntityMgr()->SetNpcDivNum(nCount);
					}	
				}
				else if (_tcsncicmp(sCmdBuf, "spdn", 4) == 0) // ����NPC���߼�ѭ���ȷ���Ŀ
				{
					char *pParam = sCmdBuf + 4;
					while (*pParam == ' ')
						pParam++;
					if (pParam)
					{
						UINT_PTR nCount = atoi(pParam);
						if (nCount > 0)
							GetGlobalLogicEngine()->GetEntityMgr()->SetPetDivNum(nCount);
					}	
				}
				else if(_tcsncicmp(sCmdBuf, "asi", 3) == 0)
				{
					char *pParam = sCmdBuf + 3;
					while (*pParam == ' ')
						pParam++;
					if (pParam)
					{
						UINT_PTR nInterval = atoi(pParam);
						if (nInterval > 0)
							CActor::m_sSaveDBInterval = (int)nInterval*1000;
					}	
				}
				else if(_tcsncicmp(sCmdBuf, "send", 4) == 0)  // ��ʼ�������ݰ�
				{
					CActor::s_nTestSendPkgFlag = 1;
				}
				else if(_tcsncicmp(sCmdBuf, "stopsend", 8) == 0)  // ֹͣ�������ݰ�
				{
					CActor::s_nTestSendPkgFlag = 0;
				}
				else if(_tcsncicmp(sCmdBuf, "item", 4) == 0)  // �����Ʒ�ķ������
				{
					GetGlobalLogicEngine()->TraceItem();
				}
				else if(_tcsncicmp(sCmdBuf, "rkf", 3) == 0)  // ˢ�¿������
				{
					GetLogicServer()->ReloadCrossConfig();
				}
				else if(_tcsncicmp(sCmdBuf, "kfid", 4) == 0)
				{
					char *pParam = sCmdBuf + 4;
					while (*pParam == ' ')
						pParam++;
					if (pParam)
					{
						int nId = atoi(pParam);
						if (nId >= 0)
							GetLogicServer()->SetCommonServerId(nId);
					}	
				}
				else if(_tcsncicmp(sCmdBuf, "openperf", 8) == 0)
				{
					GetGlobalLogicEngine()->SetOpenPerfLog(true);
				}
				else if(_tcsncicmp(sCmdBuf, "closeperf", 9) == 0)
				{
					GetGlobalLogicEngine()->SetOpenPerfLog(false);
				}
				else if(_tcsncicmp(sCmdBuf, "fb", 2) == 0)
				{
					GetGlobalLogicEngine()->GetFuBenMgr()->Trace();
				}
				else if(_tcsncicmp(sCmdBuf, "ref", 3) == 0)
				{
					GetGlobalLogicEngine()->GetFuBenMgr()->TraceRefreshPos();
				}
				else if(_tcsncicmp(sCmdBuf, "resetfb", 7) == 0)
				{
					GetGlobalLogicEngine()->GetFuBenMgr()->ResetFbRefresh();
				}
				else if(_tcsncicmp(sCmdBuf, "resetkf", 7) == 0)		//����ʱ�����ô���ʮ���嵲
				{
					//GetGlobalLogicEngine()->GetTenActivityManager().ClearTenDaysAllActivity();
				}
				else if (_tcsncicmp(sCmdBuf, "pid", 3) == 0)
				{			
					OutputMsg(rmTip, _T("-------The PID of this process is %d"), GetCurrentProcessId());
				}
				else if (_tcsncicmp(sCmdBuf, "loginlog", 8) == 0) // loginlog 0��1
				{
					char *pParam = sCmdBuf + 8;
					while (*pParam == ' ')
						pParam++;

					if (pParam)
					{
						UINT_PTR is_print = atoi(pParam);
						get_global_data().set_print_login_log(is_print > 0 ? true : false);
						
						OutputMsg(rmTip, is_print > 0 ? _T("-------enable login log") : _T("-------disable login log"));
					}					
				}
				else if(_tcsncicmp(sCmdBuf, "rsf", 3) == 0)
				{
					GetGlobalLogicEngine()->GetGlobalNpc()->LoadScript(CLogicEngine::szGlobalFuncScriptFile, true);
				}
				else if(_tcsncicmp(sCmdBuf, "checks", 6) == 0)			//���ű�
				{
					char *pParam = sCmdBuf + 6;
					pParam++;
					GetGlobalLogicEngine()->GetGlobalNpc()->CheckAllScript(pParam);
				}
				else if(_tcsncicmp(sCmdBuf, "check", 5) == 0)
				{
					GetGlobalLogicEngine()->GetGlobalNpc()->CheckScript(CLogicEngine::szGlobalFuncScriptFile);
				}

				Sleep(10);
			}
			GameServerEntry::g_pLogicServer->StopServer(bSaveFlag);
		}
		else
		{
			printf("Press Any Key To Quit...\n");
			int c = getc(stdin);
		}
	}
	OutputMsg(rmTip, "delete GameServerEntry::g_pLogicServer...");
	delete GameServerEntry::g_pLogicServer;
}

VOID ServiceMain(int argc, char** argv)
{
	LPTSTR sCmd = NULL;
	if (argc >= 2)
	{
		sCmd = argv[1];
	}
	CLogicService logicService("LogicService");
	logicService.Run(sCmd);
}

BOOL SetupLogicServerConfig(CLogicServer *lpLogicServer, const char *pszConfigFileName)
{
	CLogicServerConfig config;	
	if (config.loadServerConfig(lpLogicServer, pszConfigFileName) == false)
	{
		return FALSE;
	}
	
	config.LoadCrossServerConfig(lpLogicServer);
	config.LoadGmLevel(lpLogicServer, pszConfigFileName);
	return TRUE;
}

