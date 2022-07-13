#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

//최대 접속대기 클라이언트 수
constexpr unsigned int MAXCONNECTIONS = 5;

//대기 버퍼 크기
constexpr unsigned int MAXWAITBUFSIZE = 4096;

//SEND용 버퍼 크기
constexpr unsigned int MAXSENDBUFSIZE = 1024;

//RECV용 버퍼 크기
constexpr unsigned int MAXRECEIVEBUFSIZE = 1024;

//메세지 타입
#define ODSP_COMMAND 1
#define ODSP_REQUEST 2
#define ODSP_ACK     3
#define ODSP_ECHO    4
#define ODSP_END     5

//메세지 헤더
struct ODSP_HDR
{
    unsigned int msgType;
    unsigned int msgLen;
};

using string = std::string;

int main( int argc, char *argv[] )
{
    //헤더를 분석할 순서인지를 판별하기위한 변수?
    bool isHeader = true;

    //보낸 데이터 길이
    unsigned int sendByteLen;

    //받은 데이터, 보낸 데이터 바이트 길이를 받는 변수
    unsigned int byteLen;

    //현재 대기 버퍼안에 있는 데이터의 길이를 저장
    unsigned int curLen;

    int clientSockFD;

    //버퍼 인덱스 포인터
    char* ptrRecvBufIdx = nullptr;

    //data 정리용 포인터?
    char* ptrDataSortingIdx = nullptr;

    //send buf용 인덱스포인터
    char* ptrSendBufIdx = nullptr;

    //RECV용 버퍼
    char buf[MAXRECEIVEBUFSIZE];

    //SEND용 버퍼 (헤더 + 메세지)
    char bufSend[MAXSENDBUFSIZE];

    //message SEND용 버퍼
    char bufSendMsg[MAXSENDBUFSIZE];

    //시스템용 대기 버퍼
    char bufWait[MAXWAITBUFSIZE];

    //헤더 처리 버퍼
    char bufHdr[MAXWAITBUFSIZE];

    //메세지 처리 버퍼
    char bufMsg[MAXWAITBUFSIZE];

    //대기 버퍼 스왑용 버퍼
    char bufTemp[MAXWAITBUFSIZE];

    string Msg = "";

    //소켓 주소 구조체
    sockaddr_in server_addr, client_addr;

    //송신용 메세지 헤더 구조체
    ODSP_HDR hdr;

    //메세지 헤더 구조체 포인터
    ODSP_HDR* recv_hdr;

    //listen 소켓 생성
    int listenSockFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(listenSockFD < 0){
        std::cout << std::endl << "socket create error" << std::endl;
        return 0;
    }

    //소켓 초기화
    int on = 1;
    if(setsockopt(listenSockFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on)) < 0){
        std::cout << std::endl << "set option curl en = 0; error!!" << std::endl;
        return 0;
    }

    /* ip주소 설정, INDADDR_ANY : 모든주소*/
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //ipv4로 설정
    server_addr.sin_family = AF_INET;

    //포트설정
    server_addr.sin_port = htons(30000);

    //listen socket에 주소할당
    if(bind(listenSockFD, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        std::cout << std::endl << "bind error" << std::endl;
        return 0;
    }

    std::cout << "server running. waiting client..." << std::endl;

    //passive 대기 상태로 client의 접속 대기
    //client에서 connect를 통해 접속 시도시 3-way-handshake 발생
    if(listen(listenSockFD, MAXCONNECTIONS) < 0){
        std::cout << std::endl << "listen error" << std::endl;
        return 0;
    }

    int clientAddrSize = sizeof(client_addr);

    while (true)
    {
        //대기버퍼의 데이터 길이 초기화
        curLen = 0;
        
        //대기 버퍼 초기화
        memset(bufWait, 0, MAXWAITBUFSIZE);

        //버퍼 포인터 초기화 : 대기 버퍼의 시작주소
        ptrRecvBufIdx = bufWait;

        //client accept설정
        clientSockFD = accept(listenSockFD, (struct sockaddr*)&client_addr, (socklen_t*)&clientAddrSize);

        if(clientSockFD < 0){
            std::cout << std::endl << "accept error" << std::endl;
            return 0;
        }

        std::cout << std::endl << "client accepted" << std::endl;
        std::cout << "address : " << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::cout << "port : " << ntohs(client_addr.sin_port) << std::endl;
    
        //data 수신 처리
        while (true)
        {
            //receive버퍼 초기화
            memset(buf, 0, MAXRECEIVEBUFSIZE);

            //maxsize만큼 대기버퍼에 data할당, byteLen은 가져온 byte길이 할당
            byteLen = recv(clientSockFD, buf, MAXRECEIVEBUFSIZE, 0);

            //recv예외처리
            if(byteLen == 0){
                std::cout << std::endl << "client" << inet_ntoa(client_addr.sin_addr) << "closed." << std::endl;
                close(clientSockFD);
                break;
            }

            //recv예외처리
            if(byteLen > MAXRECEIVEBUFSIZE){
                std::cout << std::endl << "client" << inet_ntoa(client_addr.sin_addr) << "closed." << std::endl;
                close(clientSockFD);
                break;
            }

            if(byteLen > 0){
                memcpy(ptrRecvBufIdx, buf, byteLen);
                curLen += byteLen;
                ptrRecvBufIdx += byteLen;
                byteLen = 0;

                //헤더 분석순서
                if(isHeader){
                    //헤더 길이 이상의 data유무 확인
                    if(curLen >= sizeof(recv_hdr)){
                        memset(bufHdr, 0, sizeof(bufHdr));
                        memcpy(bufHdr, bufWait, sizeof(recv_hdr));
                        bufHdr[sizeof(recv_hdr)] = '\0';
                        recv_hdr = (ODSP_HDR*)bufHdr;

                        //버퍼 data 정리
                        ptrDataSortingIdx = bufWait + sizeof(recv_hdr);
                        memset(bufTemp, 0, sizeof(bufTemp));
                        memcpy(bufTemp, ptrDataSortingIdx, sizeof(bufWait) - sizeof(recv_hdr));
                        memset(bufWait, 0, sizeof(bufWait));
                        memcpy(bufWait, bufTemp, sizeof(bufTemp));

                        curLen -= sizeof(recv_hdr);
                        ptrRecvBufIdx -= sizeof(recv_hdr);
                        isHeader = false;
                    }
                }
            }

            if(!isHeader){
                //헤더 메세지 길이 정보를 통해 현재 버퍼에 메세지 길이 이상의 data가 있는지 확인
                if(curLen >= recv_hdr->msgLen){
                    memset(bufMsg, 0, sizeof(bufMsg));
                    memcpy(bufMsg, bufWait, recv_hdr->msgLen);
                    bufMsg[recv_hdr->msgLen] = '\0';

                    //버퍼의 data정리
                    ptrDataSortingIdx = bufWait + recv_hdr->msgLen;
                    memset(bufTemp, 0, sizeof(bufTemp));
                    memcpy(bufTemp, ptrDataSortingIdx, sizeof(bufWait)- recv_hdr->msgLen);
                    memset(bufWait, 0, sizeof(bufWait));
                    memcpy(bufWait, bufTemp, sizeof(bufTemp));

                    curLen -= recv_hdr->msgLen;
                    ptrRecvBufIdx -= recv_hdr->msgLen;
                    isHeader = true;

                    if(recv_hdr->msgType == ODSP_COMMAND){
                        std::cout << std::endl << "received command message" << std::endl;
                        std::cout << "message length is" << recv_hdr->msgLen << "byte" << std::endl;
                        std::cout << "receive message : " << bufMsg << std::endl;

                        Msg = "received command message";
                        strcpy(bufSendMsg, Msg.c_str());

                        memset(&hdr, 0, sizeof(hdr));
                        hdr.msgType = ODSP_ACK;
                        hdr.msgLen = Msg.length();

                        ptrSendBufIdx = bufSend;
                        memcpy(ptrSendBufIdx, &hdr, sizeof(hdr));
                        ptrSendBufIdx = ptrSendBufIdx + sizeof(hdr);
                        memcpy(ptrSendBufIdx, bufSendMsg, hdr.msgLen);

                        sendByteLen = send(clientSockFD, bufSend, sizeof(hdr) + hdr.msgLen, 0);

                        if(sendByteLen < 0){
                            std::cout << std::endl << "send error" << std::endl;
                            return 0;
                        }
                    }
                    else if(recv_hdr->msgType == ODSP_REQUEST){
                        std::cout << std::endl << "received request message" << std::endl;
                        std::cout << "message length is" << recv_hdr->msgLen << "byte" << std::endl;
                        std::cout << "receive message : " << bufMsg << std::endl;
                    }
                    else if(recv_hdr->msgType == ODSP_ACK){
                        std::cout << std::endl << "received ack message" << std::endl;
                        std::cout << "message length is" << recv_hdr->msgLen << "byte" << std::endl;
                        std::cout << "receive message : " << bufMsg << std::endl;
                    }
                    else if(recv_hdr->msgType == ODSP_ECHO){
                        std::cout << std::endl << "receved echo message" << std::endl;
                        std::cout << "message length is " << recv_hdr->msgLen << "byte" << std::endl;
                        std::cout << "receive message : " << bufMsg << std::endl;

                        Msg = bufMsg;
                        strcpy(bufSendMsg, Msg.c_str());

                        memset(&hdr, 0, sizeof(hdr));
                        hdr.msgType = ODSP_ECHO;
                        hdr.msgLen = recv_hdr->msgLen;

                        ptrSendBufIdx = bufSend;
                        memcpy(ptrSendBufIdx, &hdr, sizeof(hdr));
                        ptrSendBufIdx = ptrSendBufIdx + sizeof(hdr);
                        memcpy(ptrSendBufIdx, bufSendMsg, hdr.msgLen);

                        sendByteLen = send(clientSockFD, bufSend, sizeof(hdr) + hdr.msgLen, 0);

                        if(sendByteLen < 0){
                            std::cout << std::endl << "send error" << std::endl;
                            return 0;
                        }
                    }
                    else if(recv_hdr->msgType == ODSP_END){
                        std::cout << std::endl << "receved end message" << std::endl;
                        std::cout << "message length is " << recv_hdr->msgLen << "byte" << std::endl;
                        std::cout << "receive message : " << bufMsg << std::endl;
                    }
                    else{
                        std::cout << std::endl << "received unknown message" << std::endl;
                        std::cout << "exit server" << std::endl;
                        close(clientSockFD);
                        break;
                    }
                }
            } //byteLen > 0
        } //recv while
    } //accept while
    
    close(listenSockFD);

    std::cout << "server end" << std::endl;

    return 0;
}