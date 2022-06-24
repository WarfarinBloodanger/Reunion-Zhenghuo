#include <bits/stdc++.h>
#include <windows.h>

using namespace std;

const char * portname = "bdutc";
int ibuf_size = 1024;
int obuf_size = 1024;
int baudrate = 4080;
int bytesize = 8;
int corrtime = 0;
int secid;

char buf[1024];
char databuf[1024 * 8];
char cursec[1024];
int loc;

bool work;

#define tprintf(type, str, ...) {\
	printf("[%s\t] ", type);\
	printf(str);\
	puts("");\
}
#ifndef TEST_MODE

#define stop() {\
	tprintf("INFO", "程序已停止");\
	while (1);\
}

#else
#define stop() (exit(0))
#endif

void deal(string s, string & name, string & val) {
	name = "", val = "";
	bool l = false;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '=') l = !l;
		else {
			if (!isspace(s[i])) {
				if (!l) name += s[i];
				else val += s[i];
			}
		}
	}
}

#define toint(s) (atoi(s.c_str()))

void operate(string name, string val) {
	if (name == "baudrate") baudrate = toint(val);
	else if (name == "ibufsize") ibuf_size = toint(val);
	else if (name == "obufsize") obuf_size = toint(val);
	else if (name == "bytesize") bytesize = toint(val);
	else if (name == "corrtime") corrtime = toint(val);
	else if (name == "portname") portname = val.c_str();
	else tprintf("WARN", ("配置文件中出现未知字段 '" + name + "'").c_str());
}

void trim(string & str) {
	if (!str.length()) return;
	while (isspace(str[0])) str = str.substr(1, str.length() - 1);
	if (!str.length()) return;
	while (isspace(str[str.length() - 1])) str = str.substr(0, str.length() - 1);
}

void chkini() {
	ifstream fcin("bdutc.ini");
	if (fcin) return;
	tprintf("INFO", "正在创建 'bdutc.ini' 作为默认配置文件");
	fcin.close(); ofstream fcout("bdutc.ini");
	fcout << "# address of your port" << endl;
	fcout << "portname=BDUTC" << endl;
	fcout << "# baudrate" << endl;
	fcout << "baudrate=4080" << endl;
	fcout << "# input buffer size" << endl;
	fcout << "ibufsize=1024" << endl;
	fcout << "# output buffer size" << endl;
	fcout << "obufsize=1024" << endl;
	fcout << "# size of each byte in input data" << endl;
	fcout << "bytesize=8" << endl;
	fcout << "# correction ms" << endl;
	fcout << "corrtime=10" << endl;
	fcout.close();
	tprintf("INFO", "默认配置文件 'bdutc.ini' 生成完成");
}

void read_config(const char * ini) {
	ifstream fcin(ini);
	if (!fcin) {
		tprintf("WARN", ("无法读取到配置文件 '" + string(ini) + "'，将使用默认配置").c_str());
		return;
	}
	string line;
	while (getline(fcin, line)) {
		trim(line); if (line[0] == '#') continue;
		string name, val; deal(line, name, val);
		operate(name, val);
	}
	tprintf("INFO", ("成功读取配置文件 '" + string(ini) + "'，配置生效").c_str());
}

int h, m, s, ms;
int d, mt, y;

void dealtime() {
	sscanf(cursec, "%2d%2d%2d.%d", &h, &m, &s, &ms);
	tprintf("SUCCESS", "尝试设置系统时间");
	printf(" %02d - %02d - %02d  %02d : %02d : %02d.%d\n", y, mt, d, h, m, s, ms);
	SYSTEMTIME st = {0};
	s += corrtime / 1000;
	ms += corrtime % 1000;
	st.wYear			=		y;
	st.wMonth			=		mt;
	st.wDay				= 		d;
	st.wHour			= 		h;
	st.wMinute			=		m;
	st.wSecond			=		s;
	st.wMilliseconds	=		ms;
	st.wDayOfWeek		= 		2;
	if (!SetLocalTime(&st)) {
		tprintf("WARN", "系统时间设置失败");
	}
	else tprintf("SUCCESS", "校准成功");
}

void dealdate() {
	sscanf(cursec, "%2d%2d%2d", &d, &mt, &y);
}

void chk(char dat) {
	if (dat == ',')  {
		cursec[loc] = '\0';
		switch(secid) {
			case 0: {
				if (!strcmp(cursec, "$BDRMC")) work = true;
				break;
			}
			case 1: {
				if (work) dealtime();
				break;
			}
			case 9: {
				if (work) dealdate();
				break;
			}
		}
		loc = 0, secid++;
	}
	else cursec[loc++] = dat;
}

int main(int argc, char ** argv) {

#ifndef TEST_MODE	
	if (argc > 1) read_config(argv[1]);
	else chkini(), read_config("bdutc.ini");
	
	HANDLE com;
	com = CreateFile(
		TEXT(portname),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	
	if (com == INVALID_HANDLE_VALUE) {
		tprintf("ERR", "端口打开失败");
		stop();
	}
	else {
		tprintf("SUCCESS", "端口成功打开");
	}
	SetupComm(com, ibuf_size, obuf_size);
	
	DCB dcb;
	GetCommState(com, &dcb);
	dcb.BaudRate 	= 	baudrate;
	dcb.ByteSize 	= 	8;
	dcb.Parity		=	NOPARITY;
	dcb.StopBits	= 	ONESTOPBIT;
	SetCommState(com, &dcb);
	
	DWORD cttlen;

#endif

	bool readok;
	bool start = false;
	while (true) {
		memset(buf, 0, sizeof(buf));
	#ifndef TEST_MODE
		readok = ReadFile(com, buf, sizeof(buf), &cttlen, NULL);
	#else
		string s; readok = getline(cin, s); memcpy(buf, s.c_str(), sizeof(buf));
	#endif
		if (!readok) {
			tprintf("ERR", "无法读取端口数据");
			stop();
		}
		else {
			tprintf("SUCCESS", "读取到数据");
			int len = strlen(buf);
			for (int i = 0; i < len; i++) {
				if (buf[i] == '$') loc = 0, secid = 0, start = true;
				if (isspace(buf[i])) chk(','), work = false, start = false;
				if (start) {
					chk(buf[i]);
				}
			}
		}
	}
	
#ifndef TEST_MODE
	CloseHandle(com);
#endif
	return 0;
}
