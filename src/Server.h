#ifndef Server_h
#define Server_h

#include "Connection.h"
#include <vector>

class Server {
public:
    explicit Server(uv_loop_t&);

    void Listen(uint16_t port);
    void OnConnectionFree(Connection&);

private:
    void Accept();

    void PushFree(Connection&);
    Connection* PopFree();

    void OnConnection(int status);
    static void OnConnection(uv_stream_t* server, int status);

    uv_loop_t& m_loop;

    uv_tcp_t m_tcp;
    size_t m_acceptsQueued;

    Connection* m_nextFree;
    std::vector<Connection> m_connections;
};

#endif
