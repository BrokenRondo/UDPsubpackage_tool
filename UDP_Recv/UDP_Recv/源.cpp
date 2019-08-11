#include <string>
#include "osapi/osapi.h"
#include <iostream>
using namespace std;
int Udp_Recv(string &sig_buf,OS_UdpSocket &sock)
{
	cout << "开始接收：" << endl;
	OS_SockAddr peer;
	char tempstr[1050] = { 0 };
	char buf[5] ="OK";
	while (true)
	{
		memset(tempstr, 0, sizeof(tempstr));
		int n = sock.RecvFrom(tempstr, 1024, peer);
		if (n==1024)
		{
			sig_buf += tempstr;
		}
		if(n<1024)
		{
			sig_buf += tempstr;
			break;
		}
		if (tempstr == "END")
		{
			break;
		}
		if (n >= 0)
		{
			sock.SendTo(buf, 4, peer);
		}
		if(n<0)
		{
			//memset(tempstr, 0, 520);
			cout << "超时" << endl;
			return -1;
		}
	}
	return 0;
}
int main()
{
	string sig_buf;
	OS_UdpSocket sock;
	OS_SockAddr local("192.168.0.100", 55556);
	sock.Open(local, true);
	sock.SetOpt_RecvTimeout(12 * 1000);
	Udp_Recv(sig_buf, sock);
	sock.Close();
	cout << sig_buf << endl;

	cout << "接收完毕" << endl;
	getchar();
}