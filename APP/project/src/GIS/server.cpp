#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

using namespace std;

int main( int argc, char *argv[] )
{
    int sock;
    int client;
    struct sockaddr_in  addr;
    struct sockaddr_in  client_addr;
    socklen_t addr_len=sizeof(addr);
    int bind_port=8090;
    int on=1;
    char buffer[10000]={0,};
    size_t read_len;
    const char *res_Header = "HTTP/1.1 200 OK\r\n"
                             "Content-Length: %d\r\n"
                             "\r\n";
    const char *res_Body = "{\"RESULT\":\"S\"}";

    const char *res_Header_Fail = "HTTP/1.1 200 \r\n"
                                  "Content-Length: %d\r\n"
                                  "\r\n";
    const char *res_Body_Fail = R"Body_Fail({"MSG":"Test_Fail_Body","RESULT":"F"})Body_Fail";

    if (argc != 2)
    {
        fprintf( stdout, "Usage: %s Port\n", argv[0] );
        return -1;
    }
	
    bind_port = atoi( argv[1] );

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on)) != 0)
    {
        return -1;
    }

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    addr.sin_port        = htons(bind_port);

    if (bind(sock, (struct sockaddr*)&addr, addr_len) != 0)
    {
        return -1;
    }

    if (listen(sock, 5) != 0)
    {
        return -1;
    }

    while (1)
    {
        client = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client < 0)
        {
            return -1;
        }

        memset(buffer, 0x00, sizeof(buffer));

        read_len = recv(client, buffer, 10000, 0);
        if (read_len < 0)
        {
            return -1;
        }

        char response[1024];

        sprintf(response, res_Header, strlen(res_Body));
        strcat(response, res_Body);
        send(client, response, strlen(response), 0);

        close(client);
    }

    close(sock);

    return 0;
}
