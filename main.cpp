//#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <conio.h>


int main(int argc, char * argv[])
{
  char	szServerName[1024] = "NONE";
  char szBuf[2048], lastStr[500]="";
  POINT p;
  HWND wfp, lastWfp=NULL;
  LONG lResult;
  struct sockaddr_in servaddr;
  struct hostent *host = NULL;
  UINT16 port = 3100;
  SOCKET	sSocket;
  WSADATA	wsd;

  servaddr.sin_addr.s_addr = INADDR_NONE;
  //cout << "select HWND" << endl;
//  SetConsoleCP(1251);
//  SetConsoleOutputCP(1251);
//  wprintf(L"ляляля\n");
//  printf("ляляляsdsdsвавыаы\n");

  printf("Grabber v1.0. mailTo: murinets.d@ya.ru \n");

  for(int i=0; i<argc; i++){
    //printf("%s\n", argv[i]);
    if(strncmp(argv[i], "-d",2) == 0){
        strcpy(szServerName, argv[i]+2);

        servaddr.sin_addr.s_addr = inet_addr(szServerName);

        if (servaddr.sin_addr.s_addr == INADDR_NONE){
            host = gethostbyname(szServerName);
            if (host == NULL){
                printf("Unable to resolve server");
                return 1;
            }
            CopyMemory(&servaddr.sin_addr, host->h_addr_list[0], host->h_length);
        }
    }
    else if(strncmp(argv[i], "-p",2) == 0){
        port = atoi(argv[i]+2);
        printf("port set to %d \n", port);
    }
    else {
        printf("-d dest ip. Example: -d10.0.0.1\n");
        printf("-p dest port. Example: -p3100 Default: 3100\n");
        return 0;
    }
  }

  if( servaddr.sin_addr.s_addr != INADDR_NONE ){
      if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
      {
        printf("Can't load WinSock");
        return 0;
      }

      sSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (sSocket == INVALID_SOCKET)
      {
        printf("Can't create socket");
        return 0;
      }
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(port);
  }

  printf("Select HWND\n");

  while(1){
    GetCursorPos(&p);
    wfp = WindowFromPoint(p);
    lResult = ::SendMessageA( wfp, WM_GETTEXT, sizeof( szBuf ) / sizeof( szBuf[0] ), (LPARAM)szBuf );
    if(wfp != lastWfp){
      //str =  "HWND 0x" +QString::number((quint32)wfp,16) + ". Copied "+ QString::number(lResult) +  " ch.  Contents: " + QString::fromLatin1(szBuf ) + "";
      //sprintf(str, "HWND 0x%x. Copied %lu ch.  Contents: %s ", (UINT32)wfp, lResult, szBuf);
      //cout << str << endl;
      printf("HWND 0x%08x text \"%s\"\n", (UINT32)wfp, szBuf);
      strcpy(lastStr, szBuf);
      lastWfp = wfp;
    }

    if(kbhit()){
      //printf("char in\n");
      if(getch() == '\r'){
        //printf("selected\n");
        break;
      }
    }
    Sleep(50);
  }

  DWORD lastSendMs = 0, curMs=0;
  bool bFirstTime = true;
  printf("Sending data to %s:%d \n", szServerName, port);
  while(1){      
    lResult = ::SendMessageA( wfp, WM_GETTEXT, sizeof( szBuf ) / sizeof( szBuf[0] ), (LPARAM)szBuf );
    curMs = GetTickCount();
    bool bDataChanged = (strcmp(szBuf, lastStr) != 0);
    bool bTimeOut = ((curMs - lastSendMs)> 2000);
    if( bDataChanged || bTimeOut || bFirstTime){
      //str =  "HWND 0x" +QString::number((quint32)wfp,16) + ". Copied "+ QString::number(lResult) +  " ch.  Contents: " + QString::fromLatin1(szBuf ) + "";
      //printf("HWND 0x%x. Copied %lu ch.  Contents: %s \n", (UINT32)wfp, lResult, szBuf);
        if( (bDataChanged == true) || (bFirstTime == true) )
            printf("> %s \n", szBuf);
        if( servaddr.sin_addr.s_addr != INADDR_NONE ){
            sendto(sSocket, szBuf, strlen(szBuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        }
        strcpy(lastStr, szBuf);
        lastSendMs = curMs;
        bFirstTime = false;
    }
    Sleep(1);
  }

  return 0;
}

