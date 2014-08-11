#include <CppUTest/TestHarness.h>
#include <unsockets.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

TEST_GROUP(UnixAddress)
{
    UnixAddress * address;

    void setup()
    {
        address = new UnixAddress();
    }

    void teardown()
    {
        delete address;
    }
};

TEST(UnixAddress, SetAddress)
{
    address->SetAddress("/tmp/test.sock");
    CHECK_TEXT(address->GetSize() == strlen("/tmp/test.sock") + sizeof(short), "Address should have its size");
}

TEST_GROUP(UnixSocket)
{
    UnixSocket * socket;

    void setup()
    {
        socket = new UnixSocket();
    }

    void teardown()
    {
        delete socket;
    }
};

TEST(UnixSocket, Constructor)
{
    CHECK_TEXT(socket->IsOk(), "Socket should be created");
}

TEST(UnixSocket, Close)
{
    socket->Close();
    CHECK_TEXT(socket->IsOk(), "Socket should close");
}

TEST(UnixSocket, Bind)
{
    socket->Bind("/tmp/test.sock");
    CHECK_TEXT(socket->IsOk(), "Socket should bind to address");
}

void * client(void *)
{
    UnixSocket * local = new UnixSocket();
    local->Connect("/tmp/test.sock");
    CHECK_TEXT(local->IsOk(), "Client should connect");
    
    int buffer[1];

    CHECK_TEXT(local->Recv(buffer, sizeof(int)) == sizeof(int), "Should receive a buffer");
    CHECK(local->IsOk());

    buffer[0]++;

    CHECK_TEXT(local->Send(buffer, sizeof(int)) == sizeof(int), "Should send a buffer");
    CHECK(local->IsOk());

    local->Close();
    CHECK_TEXT(local->IsOk(), "Client should close connection");

    delete local;
    pthread_exit(NULL);
}

TEST(UnixSocket, Accept)
{
    socket->Bind("/tmp/test.sock");
    CHECK(socket->IsOk());

    pthread_t client_thread;
    pthread_create(&client_thread, NULL, client, NULL);

    UnixSocket * remote = new UnixSocket();
    socket->Accept(remote);

    CHECK_TEXT(socket->IsOk(), "Server should accept connections");
    CHECK_TEXT(remote->IsOk(), "Client should be accepted");

    int buffer[1];
    buffer[0] = 1;

    CHECK_TEXT(remote->Send(buffer, sizeof(int)) == sizeof(int), "Should send a buffer");
    CHECK(remote->IsOk());
    
    CHECK_TEXT(remote->Recv(buffer, sizeof(int)) == sizeof(int), "Should receive a buffer");
    CHECK(remote->IsOk());

    CHECK_TEXT(buffer[0] == 2, "Client should increment the number");

    pthread_join(client_thread, NULL);
    remote->Close();
    socket->Close();
    
    CHECK_TEXT(remote->IsOk(), "Client should close connection");
    CHECK_TEXT(socket->IsOk(), "Server should close connection");

    delete remote;
}

void * early_closed_client(void *)
{
    UnixSocket * local = new UnixSocket();
    local->Connect("/tmp/test.sock");
    local->Close();
    delete local;
    pthread_exit(NULL);
}


TEST(UnixSocket, BrokenPipe)
{
    socket->Bind("/tmp/test.sock");
    
    pthread_t client_thread;
    pthread_create(&client_thread, NULL, early_closed_client, NULL);

    UnixSocket * remote = new UnixSocket();
    socket->Accept(remote);

    int buffer[1];
    buffer[0] = 1;

    CHECK_TEXT(remote->Send(buffer, sizeof(int)) == EPIPE, "Should identify an 'Broken Pipe'");

    pthread_join(client_thread, NULL);
    remote->Close();
    socket->Close();

    delete remote;
}
