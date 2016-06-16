#include "Connection.h"
#include "Server.h"

Connection::Connection(Server& server)
    : m_server(server)
    , m_next(nullptr)
    , m_closing(false)
    , m_writePending(false)
    , m_writesQueued(0) {
}

void Connection::Init(uv_loop_t& loop) {
    http_parser_init(&m_httpParser, HTTP_REQUEST);
    m_httpParser.data = this;

    http_parser_settings_init(&m_httpParserSettings);
    m_httpParserSettings.on_message_complete = OnMessageComplete;

    uv_tcp_init(&loop, &m_tcp);

    m_tcp.data = this;
    m_writeReq.data = this;

    m_closing = false;

    m_writePending = false;
    m_writesQueued = 0;
}

void Connection::Close() {
    if (!m_closing) {
        m_closing = true;
        uv_close(reinterpret_cast<uv_handle_t*>(&m_tcp), OnClose);
    }
}

void Connection::Read() {
    if (uv_read_start(reinterpret_cast<uv_stream_t*>(&m_tcp), OnAlloc,
                      OnRead)) {
        Close();
    }
}

void Connection::OnAlloc(size_t hint, uv_buf_t* buf) {
    buf->base = m_buffer;
    buf->len = sizeof(m_buffer);
}

void Connection::OnRead(ssize_t nread, const uv_buf_t* buf) {
    if (nread > 0) {
        size_t nparsed = http_parser_execute(
            &m_httpParser, &m_httpParserSettings, m_buffer, nread);

        if (nparsed != nread) {
            Close();
        }
    } else {
        Close();
    }
}

void Connection::OnMessageComplete() {
    if (m_writePending) {
        m_writesQueued += 1;
    } else {
        Write();
    }
}

void Connection::Write() {
    m_writePending = true;

    const char response[] = "HTTP/1.1 200 OK\r\n"
                            "Server: uvbench\r\n"
                            "Connection: keep-alive\r\n"
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
    m_writePending = false;

    if (status) {
        Close();
    } else if (m_writesQueued != 0) {
        m_writesQueued -= 1;
        Write();
    }
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

int Connection::OnMessageComplete(http_parser* httpParser) {
    Connection& connection = *static_cast<Connection*>(httpParser->data);
    connection.OnMessageComplete();
    return 0;
}
