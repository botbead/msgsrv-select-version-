#ifndef SMSDLL_H
#define SMSDLL_H

typedef struct _sms_report_t_ {
	unsigned long index; // ����Ϣ���:index����0��ʼ����
	char Msg[256]; // ��������
	int Success; // �Ƿ��ͳɹ� 0Ϊʧ�ܣ���0Ϊ�ɹ�
	char PhoneNo[32]; // Ŀ���ֻ�����
} SMSReportStruct;

typedef struct _sms_msg_t_ {
	char Msg[256]; // ��������
	char PhoneNo[32]; // �Է��ֻ�����
	char ReceTime[32]; // ����ʱ��
} SMSMessageStruct;

// ��������,�򿪴��ڣ���ʼ��Modem, 0Ϊʧ�ܣ���0Ϊ�ɹ�
// У��λ�� EvenParity :0,MarkParity:1,NoParity:2,OddParity:3,SpaceParity,4
// ֹͣλ OneStopBit 0,OnePointFiveStopBits:1,TwoStopBits 2
// ����:NoFlowControl:0,    CtsRtsFlowControl:1,    CtsDtrFlowControl:2,    DsrRtsFlowControl:3,    DsrDtrFlowControl:4,    XonXoffFlowControl:5
extern "C" __declspec(dllimport) int _stdcall SMSStartService(int nPort,
	unsigned long BaudRate = 57600, int Parity = 2, int DataBits = 8,
	int StopBits = 0, int FlowControl = 0, char* csca = "card");

// ֹͣ���񣬲��رմ���,0Ϊʧ�ܣ���0Ϊ�ɹ�
extern "C" __declspec(dllimport) int _stdcall SMSStopSerice();

// ���Ͷ���Ϣ,���ض���Ϣ���:index����0��ʼ�������ú����������������ȷ��أ����ú���SMSQuery(unsigned long index)����ѯ�Ƿ��ͳɹ�
extern "C" __declspec(dllimport) unsigned long _stdcall SMSSendMessage
	(char* Msg, char* PhoneNo);

// ������ŷ���׳̬(�ɹ����)0Ϊ�б��棬��0Ϊ��
extern "C" __declspec(dllimport) int _stdcall SMSReport(SMSReportStruct* rept);

// ��ѯָ����ŵĶ����Ƿ��ͳɹ�(�������SMSSendMessage����)
// ���� 0 ��ʾ����ʧ��
// 1 ��ʾ���ͳɹ�
// -1 ��ʾû�в�ѯ������ŵĶ���,�������ڷ����С�
extern "C" __declspec(dllimport) int _stdcall SMSQuery(unsigned long index);

// ���ն���,0Ϊ�ж��ţ���0Ϊ��
extern "C" __declspec(dllimport) int _stdcall SMSGetNextMessage
	(SMSMessageStruct* Msg);

// ���ش������ݵĳ���
extern "C" __declspec(dllimport) int _stdcall SMSGetLastError(char* err);

#endif
