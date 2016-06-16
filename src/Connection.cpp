#include "Connection.h"
#include "Server.h"

Connection::Connection(uv_loop_t* loop, Server& server)
    : m_loop(loop), m_server(server), m_next(nullptr) {
}

void Connection::Init() {
    m_tcp.data = this;
    m_writeReq.data = this;
    uv_tcp_init(m_loop, &m_tcp);
}

void Connection::Close() {
    uv_close(reinterpret_cast<uv_handle_t*>(&m_tcp), OnClose);
}

void Connection::OnAlloc(size_t hint, uv_buf_t* buf) {
    buf->base = reinterpret_cast<char*>(m_buffer);
    buf->len = sizeof(m_buffer);
}

void Connection::Read() {
    if (uv_read_start(reinterpret_cast<uv_stream_t*>(&m_tcp), OnAlloc,
                      OnRead)) {
        Close();
    }
}

void Connection::OnRead(ssize_t nread, const uv_buf_t* buf) {
    if (nread >= 0) {
        Write();
    } else if (nread < 0) {
        Close();
    }
}

void Connection::Write() {
    const char response[] = "HTTP/1.1 200 OK\r\n"
                            "Server: uvbench\r\n"
                            "Connection: close\r\n"
                            "Date: Thu, 16 Jun 2016 00:10:17 GMT\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, world!";

    m_writeBuf.base = const_cast<char*>(response);
    m_writeBuf.len = sizeof(response) - 1;

    if (uv_write(&m_writeReq, reinterpret_cast<uv_stream_t*>(&m_tcp),
                 &m_writeBuf, 1, OnWrite)) {
        Close();
    }
}

void Connection::OnWrite(int status) {
    Close();
}

void Connection::OnClose() {
    m_server.OnConnectionClosed(*this);
}

void Connection::OnAlloc(uv_handle_t* handle, size_t hint, uv_buf_t* buf) {
    Connection& connection = *static_cast<Connection*>(handle->data);
    connection.OnAlloc(hint, buf);
}

void Connection::OnRead(uv_stream_t* stream,
                        ssize_t nread,
                        const uv_buf_t* buf) {
    Connection& connection = *static_cast<Connection*>(stream->data);
    connection.OnRead(nread, buf);
}

void Connection::OnWrite(uv_write_t* req, int status) {
    Connection& connection = *static_cast<Connection*>(req->data);
    connection.OnWrite(status);
}

void Connection::OnClose(uv_handle_t* handle) {
    Connection& connection = *static_cast<Connection*>(handle->data);
    connection.OnClose();
}
