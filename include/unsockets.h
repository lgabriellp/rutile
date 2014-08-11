#ifndef _UNSOCKETS_H_
#define _UNSOCKETS_H_

#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>

class UnixAddress
{
public:
    UnixAddress();
    void SetAddress(const char * path);
    int GetSize() const;
    operator sockaddr* ();
private:
    sockaddr_un m_internal;
};

class UnixSocket
{
public:
    UnixSocket();
    UnixSocket(int fd);
    int IsOpen() const;
    int IsOk() const;
    int Close();
    int Connect(const char * path);
    int Bind(const char * path);
    int Accept(UnixSocket * client);
    int Send(void * buffer, int size);
    int Recv(void * buffer, int size);
    int SetFd(int fd);
    int GetStatus() const;
    int SetStatus();
private:
    int m_internal;
    int m_status;
};

#endif//_UNSOCKETS_H_
