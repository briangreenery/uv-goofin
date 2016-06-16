#ifndef Connection_h
#define Connection_h

#include <uv.h>

class Server;

class Connection {
public:
    Connection(uv_loop_t* loop, Server&);

    void Init();
    void Close();

    Connection* NextConnection();
    void SetNextConnection(Connection*);

    uv_tcp_t* GetHandle();

    void Read();
    void Write();

private:

    void OnAlloc(size_t, uv_buf_t*);
    static void OnAlloc(uv_handle_t*, size_t, uv_buf_t*);

    void OnRead(ssize_t, const uv_buf_t*);
    static void OnRead(uv_stream_t*, ssize_t, const uv_buf_t*);

    void OnWrite(int);
    static void OnWrite(uv_write_t*, int);

    void OnClose();
    static void OnClose(uv_handle_t*);

    uv_loop_t* m_loop;
    Server& m_server;

    Connection* m_next;

    uv_tcp_t m_tcp;
    uv_write_t m_writeReq;

    uv_buf_t m_writeBuf;
    uint8_t m_buffer[1024];
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
