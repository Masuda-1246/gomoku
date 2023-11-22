//http://blog.livedoor.jp/akf0/archives/51585502.html

#include <stdio.h>

#include <winsock2.h>



int main(void) {

	struct sockaddr_in dest;

	memset(&dest, 0, sizeof(dest));

	char input_addr[20];
	printf("input addres: \n");
	scanf("%s", input_addr);
	printf("your addres is %s \n", input_addr);

	char input_port[20];
	printf("input port: \n");
	scanf("%s", input_port);
	int port = atoi(input_port);
	printf("your port is %d \n", port);

	dest.sin_port = htons(port);
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(input_addr);
	WSADATA data;

	WSAStartup(MAKEWORD(2, 0), &data);

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(s, (struct sockaddr *) &dest, sizeof(dest))) {
		printf("%s: connection refused ...\n", input_addr);
		return -1;
	}

	printf("%s conncected! \n", input_addr);

	char buffer[1024];
	char name[20];
	recv(s, buffer, 1024, 0);
	printf("%s\n", buffer);
	scanf("%s", name);
	send(s, name, strlen(name), 0);

	while (1) {
		memset(&buffer, 0, sizeof(buffer));
		recv(s, buffer, 1024, 0);
		printf("from server ---> %s\n", buffer);
		char msg[20];
		scanf("%s", msg);
		send(s, msg, strlen(msg), 0);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
