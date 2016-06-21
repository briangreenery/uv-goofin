#include "Connection.h"
#include "Server.h"

Connection::Connection(Server& server) : m_server(server), m_next(nullptr) {
}

void Connection::Init(uv_loop_t& loop) {
    uv_tcp_init(&loop, &m_tcp);
    m_tcp.data = this;
    m_closing = false;

    m_shutdownReq.data = this;

    m_writeReq.data = this;
    m_writePending = false;
    m_writesQueued = 0;

    http_parser_init(&m_httpParser, HTTP_REQUEST);
    m_httpParser.data = this;

    http_parser_settings_init(&m_httpParserSettings);
    m_httpParserSettings.on_message_complete = OnHttpRequestEnd;

    m_keepAlive = true;
}

void Connection::Close() {
    if (!m_closing) {
        m_closing = true;
        uv_close(reinterpret_cast<uv_handle_t*>(&m_tcp), OnClose);
    }
}

void Connection::OnClose() {
    m_server.OnConnectionFree(*this);
}

void Connection::Read() {
    if (uv_read_start(reinterpret_cast<uv_stream_t*>(&m_tcp), OnAlloc,
                      OnRead)) {
        Close();
    }
}

void Connection::OnAlloc(size_t, uv_buf_t* buf) {
    buf->base = m_buffer;
    buf->len = sizeof(m_buffer);
}

void Connection::OnRead(ssize_t nread, const uv_buf_t* buf) {
    if (nread > 0) {
        OnReadData(nread);
    } else if (nread == 0) {
        OnReadEnd();
    } else {
        Close();
    }
}

void Connection::OnReadData(size_t nread) {
    size_t nparsed = http_parser_execute(&m_httpParser, &m_httpParserSettings,
                                         m_buffer, nread);

    if (nparsed == nread) {
        Write();
    } else {
        Close();
    }
}

void Connection::OnReadEnd() {
    if (m_writePending) {
        m_keepAlive = false;
    } else {
        Shutdown();
    }
}

void Connection::OnHttpRequestEnd() {
    m_keepAlive = http_should_keep_alive(&m_httpParser) ? true : false;
    m_writesQueued += 1;
}

static uv_buf_t KeepAliveResponse() {
    static char response[] = "HTTP/1.1 200 OK\r\n"
                             "Server: uvbench\r\n"
                             "Date: Thu, 11 Mar 1984 00:00:00 GMT\r\n"
                             "Connection: keep-alive\r\n"
                             "Content-Length: 4\r\n"
                             "\r\n"
                             "ok\r\n";

    uv_buf_t buf;
    buf.base = response;
    buf.len = sizeof(response) - 1;

    return buf;
}

static uv_buf_t CloseResponse() {
    static char response[] = "HTTP/1.1 200 OK\r\n"
                             "Server: uvbench\r\n"
                             "Date: Thu, 11 Mar 1984 00:00:00 GMT\r\n"
                             "Connection: close\r\n"
                             "Content-Length: 8\r\n"
                             "\r\n"
                             "donezo\r\n";

    uv_buf_t buf;
    buf.base = response;
    buf.len = sizeof(response) - 1;

    return buf;
}

void Connection::Write() {
    if (m_writePending) {
        return;
    }

    if (m_writesQueued == 0) {
        return;
    }

    m_writePending = true;
    m_writesQueued -= 1;

    if (m_keepAlive || m_writesQueued != 0) {
        m_writeBuf = KeepAliveResponse();
    } else {
        m_writeBuf = CloseResponse();
    }

    if (uv_write(&m_writeReq, reinterpret_cast<uv_stream_t*>(&m_tcp),
                 &m_writeBuf, 1, OnWrite)) {
        Close();
    }
}

void Connection::OnWrite(int status) {
    if (status) {
        Close();
        return;
    }

    m_writePending = false;

    if (m_writesQueued != 0) {
        Write();
    } else if (!m_keepAlive) {
        Shutdown();
    }
}

void Connection::Shutdown() {
    if (uv_shutdown(&m_shutdownReq, reinterpret_cast<uv_stream_t*>(&m_tcp),
                    OnShutdown)) {
        Close();
    }
}

void Connection::OnShutdown(int status) {
    Close();
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

void Connection::OnShutdown(uv_shutdown_t* req, int status) {
    Connection& connection = *static_cast<Connection*>(req->data);
    connection.OnShutdown(status);
}

void Connection::OnClose(uv_handle_t* handle) {
    Connection& connection = *static_cast<Connection*>(handle->data);
    connection.OnClose();
}

int Connection::OnHttpRequestEnd(http_parser* parser) {
    Connection& connection = *static_cast<Connection*>(parser->data);
    connection.OnHttpRequestEnd();
    return 0;
}
