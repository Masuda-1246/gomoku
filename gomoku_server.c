#include <stdio.h>
#include <unistd.h>
#include <winsock2.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>



// display 

#define BOARD_SQUARE 15
int board[BOARD_SQUARE][BOARD_SQUARE] = {{0}};  

int displayBoard(char *str, int turn){
    

    char *token;
    token=strtok(str,",");

    if(token==NULL){
        printf("error msg=%s\n",str);
        return 0;
    }

    int x = atoi(token);
    
    if( x<1 || x>BOARD_SQUARE) {
        printf("error x=%s\n",x);
        return 0;
    }

    token=strtok(NULL,",");
 
    if(token==NULL){
        printf("error msg=%s\n",str);
        return 0;
    }

   int y = atoi(token);

    if( y<1 || y>BOARD_SQUARE) {
        printf("error y=%s\n",y);
        return 0;
    }

    if(board[x-1][y-1]!=0){
        printf("already put\n");
        return 0;
    }
    board[x-1][y-1] = turn;
    
    int i=0;
    printf("  ");
    for(i = 1; i < BOARD_SQUARE+1; i++ ){
        printf("%2d",i);
    }
    puts("");


    int j=0;
    for(i = 1; i < BOARD_SQUARE+1; i++ ){        
        printf("%2d",i);        
        for(j = 1; j < BOARD_SQUARE+1; j++ ){
            
	    if(board[j-1][i-1]==0) printf(" -");
	    if(board[j-1][i-1]==1) printf(" o");
	    if(board[j-1][i-1]==2) printf(" x");

        }
        puts("");
    }
    puts("");

    token=strtok(NULL,",");

    if(token!=NULL){
        printf("recieved judge = %s\n",token);
        return 0;
    }

    Sleep(2000);
    return 1;
    
}


int main(int argc, char* argv[]){

  SOCKET s, s1;         //É\ÉPÉbÉg
  int result;          //ñﬂÇËíl

  //ê⁄ë±Çãñâ¬Ç∑ÇÈÉNÉâÉCÉAÉìÉgí[ññÇÃèÓïÒ

  struct sockaddr_in source;
  char name1[1024], name2[1024];

  memset(name1, '\0', sizeof(name1));

  //ëóêMå≥ÇÃí[ññèÓïÒÇìoò^Ç∑ÇÈ

  memset(&source, 0, sizeof(source));
  source.sin_family = AF_INET;

  //É|Å[Égî‘çÜÇÕÉNÉâÉCÉAÉìÉgÉvÉçÉOÉâÉÄÇ∆ã§í 

  source.sin_port = htons(12345);
  source.sin_addr.s_addr = htonl(INADDR_ANY);

  //É\ÉPÉbÉgí êMÇÃäJénèÄîı
  WSADATA data;
  result = WSAStartup(MAKEWORD(2, 0), &data);
  if (result < 0){
    printf("%d\n", GetLastError());
    printf("É\ÉPÉbÉgí êMèÄîıÉGÉâÅ[\n");
  }



  //É\ÉPÉbÉgÇÃê∂ê¨
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0){
    printf("%d\n", GetLastError());
    printf("É\ÉPÉbÉgê∂ê¨ÉGÉâÅ[\n");
  }

  //É\ÉPÉbÉgÇÃÉoÉCÉìÉh
  result = bind(s, (struct sockaddr *)&source, sizeof(source));
  if (result < 0){
    printf("%d\n", GetLastError());
    printf("ÉoÉCÉìÉhÉGÉâÅ[\n");
  }

  //ê⁄ë±ÇÃãñâ¬
  result = listen(s, 1);
  if (result < 0){
    printf("ê⁄ë±ãñâ¬ÉGÉâÅ[\n");
  }


  printf("wait for the first client ... (port 12345)\n");



  //ÉNÉâÉCÉAÉìÉgÇ©ÇÁí êMÇ™Ç†ÇÈÇ‹Ç≈ë“ã@
  s1 = accept(s, NULL, NULL);
  if (s1 < 0){
    printf("ë“ã@ÉGÉâÅ[\n");
  }

  //ÉNÉâÉCÉAÉìÉgÇ÷ÉfÅ[É^ÇëóêMÇ∑ÇÈ
  result = send(s1, "player1 name =", 20, 0);

  //ÉNÉâÉCÉAÉìÉgÇ©ÇÁëóêMÇ≥ÇÍÇΩÉfÅ[É^ÇÃéÛêM
  result = recv(s1, name1, 20, 0);
  if (result < 0){
    printf("éÛêMÉGÉâÅ[\n");
  }

  printf("%sÇéÛêMÇµÇ‹ÇµÇΩ\n", name1);



  SOCKET s2, s3;         //É\ÉPÉbÉg

  //ê⁄ë±Çãñâ¬Ç∑ÇÈÉNÉâÉCÉAÉìÉgí[ññÇÃèÓïÒ

  struct sockaddr_in source2;

  memset(name2, '\0', sizeof(name2));

  //ëóêMå≥ÇÃí[ññèÓïÒÇìoò^Ç∑ÇÈ

  memset(&source2, 0, sizeof(source2));
  source2.sin_family = AF_INET;

  //É|Å[Égî‘çÜÇÕÉNÉâÉCÉAÉìÉgÉvÉçÉOÉâÉÄÇ∆ã§í 

  source2.sin_port = htons(12346);
  source2.sin_addr.s_addr = htonl(INADDR_ANY);

  //É\ÉPÉbÉgí êMÇÃäJénèÄîı
  WSADATA data2;
  result = WSAStartup(MAKEWORD(2, 0), &data2);
  if (result < 0){
    printf("%d\n", GetLastError());
    printf("É\ÉPÉbÉgí êMèÄîıÉGÉâÅ[\n");
  }

  //É\ÉPÉbÉgÇÃê∂ê¨
  s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s2 < 0){
    printf("%d\n", GetLastError());
    printf("É\ÉPÉbÉgê∂ê¨ÉGÉâÅ[\n");
  }

  //É\ÉPÉbÉgÇÃÉoÉCÉìÉh
  result = bind(s2, (struct sockaddr *)&source2, sizeof(source2));
  if (result < 0){
    printf("%d\n", GetLastError());
    printf("ÉoÉCÉìÉhÉGÉâÅ[\n");
  }

  //ê⁄ë±ÇÃãñâ¬
  result = listen(s2, 1);
  if (result < 0){
    printf("ê⁄ë±ãñâ¬ÉGÉâÅ[\n");
  }


  printf("wait for the second client ... (port 12346)\n");



  //ÉNÉâÉCÉAÉìÉgÇ©ÇÁí êMÇ™Ç†ÇÈÇ‹Ç≈ë“ã@
  s3 = accept(s2, NULL, NULL);
  if (s3 < 0){
    printf("ë“ã@ÉGÉâÅ[\n");
  }

  //ÉNÉâÉCÉAÉìÉgÇ÷ÉfÅ[É^ÇëóêMÇ∑ÇÈ
  result = send(s3, "player2 name=", 20, 0);

  //ÉNÉâÉCÉAÉìÉgÇ©ÇÁëóêMÇ≥ÇÍÇΩÉfÅ[É^ÇÃéÛêM
  result = recv(s3, name2, 20, 0);
  if (result < 0){
    printf("éÛêMÉGÉâÅ[\n");
  }

  printf("%sÇéÛêMÇµÇ‹ÇµÇΩ\n", name2);


  char buffer[1024] = "start";  //éÛêMÉfÅ[É^ÇÃÉoÉbÉtÉ@óÃàÊ

  while(1){

    result = send(s1, buffer, strlen(buffer), 0);
    //buffer[0] = '\0';
    memset(&buffer, '\0', sizeof(buffer));

    result = recv(s1, buffer, sizeof(buffer), 0);

    if (result < 0){
      printf("éÛêMÉGÉâÅ[\n");
      break;
    }

    printf("%sÇéÛêMÇµÇ‹ÇµÇΩ(%s)\n", buffer, name1);


    char buffer1[1024];
    strcpy(buffer1,buffer);
    if(!displayBoard(buffer1,1)){
      printf("ÉQÅ[ÉÄÉZÉbÉg\n");
      break;
    }

    result = send(s3, buffer, strlen(buffer), 0);
    //buffer[0] = '\0';
    memset(&buffer, '\0', sizeof(buffer));
    result = recv(s3, buffer, sizeof(buffer), 0);

    if (result < 0){
      printf("éÛêMÉGÉâÅ[\n");
      break;
    }
    

    printf("%sÇéÛêMÇµÇ‹ÇµÇΩ(%s)\n", buffer, name2);

    char buffer2[256];
    strcpy(buffer2,buffer);

    if(!displayBoard(buffer2,2)){
      printf("ÉQÅ[ÉÄÉZÉbÉg\n");
      break;
    }

  }

  result = send(s1, "end", 10, 0);
  result = send(s3, "end", 10, 0);

  printf("ê⁄ë±èIóπ\n");
  closesocket(s1);
  closesocket(s3);


  //É\ÉPÉbÉgí êMÇÃèIóπ
  WSACleanup();

  printf("END\n");


  return 0;

}

