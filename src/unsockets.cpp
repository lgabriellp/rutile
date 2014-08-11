#include <unsockets.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

UnixAddress::UnixAddress()
{
    bzero((char *)&m_internal, sizeof(m_internal));
}

void UnixAddress::SetAddress(const char * path)
{
    bzero((char *)&m_internal, sizeof(m_internal));
    m_internal.sun_family = AF_LOCAL;
    strncpy(m_internal.sun_path, path, sizeof(m_internal.sun_path));
}

UnixAddress::operator sockaddr *()
{
    return (sockaddr *)&m_internal;
}

int UnixAddress::GetSize() const
{
    return SUN_LEN(&m_internal);
}

UnixSocket::UnixSocket()
:   m_internal(-1),
    m_status(0)
{
    m_internal = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (m_internal < 0) SetStatus();
}

int UnixSocket::IsOpen() const
{
    return m_internal >= 0;
}
#include <stdio.h>

int UnixSocket::IsOk() const
{
    return m_status == 0;
}

int UnixSocket::GetStatus() const
{
    return m_status;
};


int UnixSocket::SetStatus()
{
    m_status = errno;
    return m_status;
}

int UnixSocket::Close()
{
    if (!IsOk()) return GetStatus();
    
    int ret;
    ret = close(m_internal);
    if (ret < 0) return SetStatus();

    m_internal = -1;
    return GetStatus();
}

int UnixSocket::SetFd(int fd)
{
    if (!IsOk()) return GetStatus();

    int ret = 0;
    if (IsOpen()) ret = Close();
    if (ret < 0) return SetStatus();

    m_internal = fd;
    return GetStatus();
}

int UnixSocket::Connect(const char * path)
{
    if (!IsOk()) return GetStatus();

    int ret;
    UnixAddress address;
    address.SetAddress(path);

    ret = connect(m_internal, (sockaddr *)address, address.GetSize());
    if (ret < 0) return SetStatus();

    return GetStatus();
}

int UnixSocket::Bind(const char * path)
{
    if (!IsOk()) return GetStatus();

    int ret;
    UnixAddress address;
    address.SetAddress(path);

    unlink(path);

    ret = bind(m_internal, (sockaddr *)address, address.GetSize());
    if (ret < 0) return SetStatus();

    ret = listen(m_internal, 5);
    if (ret < 0) return SetStatus();

    return GetStatus();
}

int UnixSocket::Accept(UnixSocket * client)
{
    if (!IsOk()) return GetStatus();
    if (!client->IsOk()) return client->GetStatus();
    
    int ret;

    ret = accept(m_internal, NULL, NULL);
    if (ret < 0) return SetStatus();

    ret = client->SetFd(ret);
    if (ret < 0) return SetStatus();

    return GetStatus();
}

int UnixSocket::Send(void * buffer, int size)
{
    if (!IsOk()) return GetStatus();

    int ret;
    ret = send(m_internal, buffer, size, MSG_NOSIGNAL);
    if (ret <= 0) return SetStatus();

    return ret;
}

int UnixSocket::Recv(void * buffer, int size)
{
    if (!IsOk()) return GetStatus();

    int ret;
    ret = recv(m_internal, buffer, size, MSG_NOSIGNAL);
    if (ret <= 0) return SetStatus();

    return ret;
}
