#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#ifdef __linux__
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <time.h>

using namespace std;

typedef void (*nullCallback)();
typedef void (*defaultCallback)(size_t);
typedef void (*messageCallback)(size_t, string);

#define WS_FIN 128
#define WS_MASK 128

#define WS_OPCODE_CONTINUATION 0
#define WS_OPCODE_TEXT         1
#define WS_OPCODE_BINARY       2
#define WS_OPCODE_CLOSE        8
#define WS_OPCODE_PING         9
#define WS_OPCODE_PONG         10

#define WS_PAYLOAD_LENGTH_16 126
#define WS_PAYLOAD_LENGTH_63 127

#define WS_READY_STATE_CONNECTING 0
#define WS_READY_STATE_OPEN       1
#define WS_READY_STATE_CLOSING    2
#define WS_READY_STATE_CLOSED     3

#define WS_STATUS_NORMAL_CLOSE             1000
#define WS_STATUS_GONE_AWAY                1001
#define WS_STATUS_PROTOCOL_ERROR           1002
#define WS_STATUS_UNSUPPORTED_MESSAGE_TYPE 1003
#define WS_STATUS_MESSAGE_TOO_BIG          1004

#define WS_STATUS_TIMEOUT 3000

#define WS_TIMEOUT_RECV 10
#define WS_TIMEOUT_PONG 5

class wsClient{
public:
    wsClient(int _socket, in_addr _addr){
        socket = _socket;
        MessageBuffer.clear();
        ReadyState = WS_READY_STATE_CONNECTING;
        LastRecvTime = time(NULL);
        PingSentTime = 0;
        CloseStatus = 0;
        addr = _addr;
        FramePayloadDataLength = 0;
        FrameBytesRead = 0;
        FrameBuffer.clear();
        MessageOpcode = 0;
        MessageBufferLength = 0;
    }

    int socket;                            // client socket
    string MessageBuffer;                  // a blank string when there's no incoming frames
    int ReadyState;                        // between 0 and 3
    time_t LastRecvTime;                   // set to time() when the client is added
    time_t PingSentTime;                   // 0 when the server is not waiting for a pong
    int CloseStatus;                       // close status that wsOnClose() will be called with
    in_addr addr;
    size_t FramePayloadDataLength;         // length of a frame's payload data.
    int FrameBytesRead;                    // amount of bytes read for a frame, reset to 0 when all frame data has been read
    string FrameBuffer;                    // joined onto end as a frame's data comes in, reset to blank string when all frame data has been read
    unsigned char MessageOpcode;           // stored by the first frame for fragmented messages, default value is 0
    size_t MessageBufferLength;            // the payload data length of MessageBuffer
};

class webSocket{
public:
    webSocket(){
        callOnOpen = NULL;
        callOnClose = NULL;
        callOnMessage = NULL;
        callPeriodic = NULL;
    }

    void setOpenHandler(defaultCallback callback);
    void setCloseHandler(defaultCallback callback);
    void setMessageHandler(messageCallback callback);
    void setPeriodicHandler(nullCallback callback);
    void startServer(int port);
    void stopServer();
    bool wsSendCompleted(size_t clientID, string message);
    bool wsSend(size_t clientID, string message, bool binary = false);
    void wsClose(size_t clientID);
    vector<size_t> getClientIDs();
    string getClientIP(size_t clientID);
private:
    vector<wsClient *> wsClients;
    map<int, int> socketIDmap;
    fd_set fds;
    size_t fdmax;
    size_t listenfd;
    std::string bad_msg = "Request is not allowed";

    void wsCheckIdleClients();
    bool wsSendClientMessage(size_t clientID, unsigned char opcode, string message);
    void wsSendClientClose(size_t clientID, unsigned short status = -1);
    bool wsCheckSizeClientFrame(size_t clientID);
    void wsRemoveClient(size_t clientID);
    bool wsProcessClientMessage(size_t clientID, unsigned char opcode, string data, int dataLength);
    bool wsProcessClientFrame(size_t clientID);
    bool wsBuildClientFrame(size_t clientID, char *buffer, int bufferLength);
    bool wsProcessClientHandshake(size_t clientID, char *buffer);
    bool wsProcessClient(size_t clientID, char *buffer, int bufferLength);
    size_t wsGetNextClientID();
    void wsAddClient(size_t socket, in_addr ip);

    defaultCallback callOnOpen;
    defaultCallback callOnClose;
    messageCallback callOnMessage;
    nullCallback callPeriodic;
};

#endif
