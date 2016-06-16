#include "Server.h"
#include "UVWrap.h"

Server::Server(uv_loop_t& loop)
    : m_loop(loop), m_nextFree(nullptr), m_pendingAccepts(0) {

    for (size_t i = 0; i < 4096; ++i) {
        m_connections.push_back(Connection(*this));
    }

    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        AddFreeConnection(*it);
    }
}

void Server::AddFreeConnection(Connection& connection) {
    connection.SetNextConnection(m_nextFree);
    m_nextFree = &connection;
}

void Server::Listen(uint16_t port) {
    struct sockaddr_in addr;
    UV::ThrowError(uv_ip4_addr("0.0.0.0", port, &addr));

    UV::ThrowError(uv_tcp_init(&m_loop, &m_tcp));
    m_tcp.data = this;

    UV::ThrowError(uv_tcp_bind(
        &m_tcp, reinterpret_cast<const struct sockaddr*>(&addr), 0));

    UV::ThrowError(
        uv_listen(reinterpret_cast<uv_stream_t*>(&m_tcp), 64, OnConnection));
}

void Server::Accept() {
    if (m_nextFree == nullptr) {
        return;
    }

    m_pendingAccepts -= 1;

    Connection& connection = *m_nextFree;
    m_nextFree = connection.NextConnection();

    connection.Init(m_loop);

    if (uv_accept(reinterpret_cast<uv_stream_t*>(&m_tcp),
                  reinterpret_cast<uv_stream_t*>(connection.GetHandle()))) {
        AddFreeConnection(connection);
        return;
    }

    connection.Read();
}

void Server::OnConnectionClosed(Connection& connection) {
    AddFreeConnection(connection);

    if (m_pendingAccepts != 0) {
        Accept();
    }
}

void Server::OnConnection(int status) {
    if (status == 0) {
        m_pendingAccepts += 1;
        Accept();
    }
}

void Server::OnConnection(uv_stream_t* stream, int status) {
    Server& server = *static_cast<Server*>(stream->data);
    server.OnConnection(status);
}
