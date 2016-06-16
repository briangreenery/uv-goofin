#ifndef Connection_h
#define Connection_h

#include "http_parser.h"
#include <uv.h>

class Server;

class Connection {
public:
    explicit Connection(Server&);

    void Init(uv_loop_t&);
    void Close();

    Connection* NextConnection();
    void SetNextConnection(Connection*);

    uv_tcp_t* GetHandle();

    void Read();

private:
    void Write();

    void OnAlloc(size_t, uv_buf_t*);
    static void OnAlloc(uv_handle_t*, size_t, uv_buf_t*);

    void OnRead(ssize_t, const uv_buf_t*);
    static void OnRead(uv_stream_t*, ssize_t, const uv_buf_t*);

    void OnWrite(int);
    static void OnWrite(uv_write_t*, int);

    void OnClose();
    static void OnClose(uv_handle_t*);

    void OnMessageComplete();
    static int OnMessageComplete(http_parser*);

    Server& m_server;
    Connection* m_next;

    uv_tcp_t m_tcp;
    bool m_closing;

    uv_write_t m_writeReq;
    uv_buf_t m_writeBuf;
    bool m_writePending;
    size_t m_writesQueued;

    http_parser m_httpParser;
    http_parser_settings m_httpParserSettings;

    char m_buffer[1024];
};

inline Connection* Connection::NextConnection() {
    return m_next;
}

inline void Connection::SetNextConnection(Connection* next) {
    m_next = next;
}

inline uv_tcp_t* Connection::GetHandle() {
    return &m_tcp;
}

#endif
