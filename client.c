#include <stdio.h>
#include <winsock2.h>

int main(void) {
    struct sockaddr_in dest1;
    struct sockaddr_in dest2;

    memset(&dest1, 0, sizeof(dest1));
    memset(&dest2, 0, sizeof(dest2));

    char input_addr[20];
    printf("input address: \n");
    scanf("%s", input_addr);
    printf("your address is %s \n", input_addr);

    char input_port[20];
    printf("input port: \n");
    scanf("%s", input_port);
    int port = atoi(input_port);
    printf("your port is %d \n", port);

    dest1.sin_port = htons(port);
    dest1.sin_family = AF_INET;
    dest1.sin_addr.s_addr = inet_addr(input_addr);

    dest2.sin_family = AF_INET;
    dest2.sin_port = htons(8080);
    dest2.sin_addr.s_addr = inet_addr("127.0.0.1");

    WSADATA data;
    WSAStartup(MAKEWORD(2, 0), &data);

    SOCKET s1 = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s1, (struct sockaddr*)&dest1, sizeof(dest1))) {
        printf("%s connection refused\n", input_addr);
        return -1;
    }

    SOCKET s2 = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(s2, (struct sockaddr*)&dest2, sizeof(dest2)) != 0) {
        printf("サーバー2への接続に失敗しました\n");
        closesocket(s1);
        WSACleanup();
        return -1;
    }

    printf("%s connected! \n", input_addr);

    char buffer[1024];
    char name[20];
    recv(s1, buffer, 1024, 0);
    printf("%s\n", buffer);
    scanf("%s", name);
    send(s1, name, strlen(name), 0);

    while (1) {
        memset(&buffer, 0, sizeof(buffer));
        recv(s1, buffer, 1024, 0);
        printf("from server ---> %s\n", buffer);
        char msg[20];
        if (strcmp(buffer, "start") == 0) {
            // msgに"8,8"を入れる
            sprintf(msg, "8,8");
            send(s1, msg, strlen(msg), 0);
        } else {
            printf("buffer: %s\n", buffer);
            send(s2, buffer, strlen(buffer), 0);
            char response[1024];
            int bytes_received = recv(s2, response, 1024, 0);
            response[bytes_received] = '\0';
            printf(response);
            send(s1, response, strlen(response), 0);
        }
    }

    closesocket(s1);
    closesocket(s2);
    WSACleanup();

    return 0;
}
