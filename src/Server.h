#ifndef Server_h
#define Server_h

#include "Connection.h"
#include <vector>

class Server {
public:
    explicit Server(uv_loop_t&);

    void Listen(uint16_t port);
    void OnConnectionClosed(Connection&);

private:
    void Accept();
    void AddFreeConnection(Connection&);

    void OnConnection(int status);
    static void OnConnection(uv_stream_t* server, int status);

    uv_loop_t& m_loop;

    uv_tcp_t m_tcp;
    Connection* m_nextFree;

    size_t m_pendingAccepts;
    std::vector<Connection> m_connections;
};

#endif
