// ---------------------------------------------------------------------------

#pragma hdrstop

#include "compon.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma link "DBAccess"
#pragma link "SQLiteUniProvider"
#pragma link "Uni"
#pragma link "UniProvider"
#pragma link "DAScript"
#pragma link "UniScript"
#pragma link "MemDS"
#pragma resource "*.dfm"
TDataModule1 *DataModule1;
String exe_path;

// ---------------------------------------------------------------------------
__fastcall TDataModule1::TDataModule1(TComponent* Owner) : TDataModule(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TDataModule1::DataModuleCreate(TObject *Sender) {
        UniConnection1->ProviderName = L"SQLite";
        UniConnection1->SpecificOptions->Values[L"Direct"] = L"True";
        UniConnection1->SpecificOptions->Values[L"ForceCreateDatabase"] = L"True";
        UniConnection1->Database = exe_path + L"srvConf.db";
        UniConnection1->Connect();
        UniScript1->Connection = UniConnection1;
        UniScript1->SQL->Clear();
        UniScript1->SQL->Add("CREATE TABLE IF NOT EXISTS back ( \
spguid INTEGER NOT NULL,  \
utf8 INTEGER NOT NULL CHECK(utf8 = 1 or utf8 = 0), \
db_port INTEGER DEFAULT 3306, \
srv_port INTEGER NOT NULL CHECK(1 <= srv_port and srv_port <= 65535), \
srv_httpport INTEGER NOT NULL CHECK(1 <= srv_httpport and srv_httpport <= 65535), \
backsrv_port INTEGER NOT NULL CHECK(1 <= backsrv_port and backsrv_port <= 65535), \
db_usr TEXT DEFAULT 'root', \
db_name TEXT NOT NULL, \
db_pwd TEXT NOT NULL, \
db_domain TEXT DEFAULT 'localhost', \
srv_domain TEXT DEFAULT '0.0.0.0', \
backsrv_domain TEXT DEFAULT '0.0.0.0', \
srv_name TEXT NOT NULL, \
backsrv_name TEXT NOT NULL, \
backip TEXT DEFAULT '', \
rowid_2th INTEGER PRIMARY KEY AUTOINCREMENT );");
        UniScript1->Execute();
        UniQuery1->Connection = UniConnection1;
}
// ---------------------------------------------------------------------------
