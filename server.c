#include <stdio.h>
#include <winsock2.h>

int main(void) {
  struct sockaddr_in dest1;
  memset(&dest1, 0, sizeof(dest1));
  
  dest1.sin_family = AF_INET;
  dest1.sin_port = htons(8080);
  dest1.sin_addr.s_addr = inet_addr("127.0.0.1");

  WSADATA data;
  WSAStartup(MAKEWORD(2, 0), &data);

  SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

  if (connect(s, (struct sockaddr *) &dest1, sizeof(dest1)) != 0) {
    printf("接続に失敗しました\n");
    return -1;
  }

  int x = 10;
  int y = 10;

  // ここでサーバーと通信する
  char command[100];
  sprintf(command, "%d,%d", x, y);

  // コマンドを送信
  send(s, command, strlen(command), 0);

  // レスポンスを受信
  char response[1024];
  int bytes_received = recv(s, response, 1024, 0);
  response[bytes_received] = '\0';
  x = 3;
  y = 3;
  sprintf(command, "%d,%d", x, y);
  printf(response);

  memset(&dest1, 0, sizeof(dest1));
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(s, (struct sockaddr *) &dest1, sizeof(dest1)) != 0) {
    printf("接続に失敗しました\n");
    return -1;
  }

  // コマンドを送信
  send(s, command, strlen(command), 0);

  // レスポンスを受信
  bytes_received = recv(s, response, 1024, 0);
  response[bytes_received] = '\0';

  // レスポンス表示
  printf(response);
  closesocket(s);
  WSACleanup();

  return 0;
}