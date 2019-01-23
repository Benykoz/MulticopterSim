/*

Class declaration for threaded socket server

Copyright Simon D. Levy 2018

MIT License
*/

#pragma once

class ThreadedSocketServer {

private:

	void * _sockinfo; // kludge to support different OSs

    char _host[100];
    int _port;

public:

	static const int BUFLEN = 512; // abitrary

	ThreadedSocketServer(int port, const char * host = "localhost");

	bool start(void);

	void stop(void);

	bool connected(void);

    bool disconnect();

	int sendBuffer(char * buf, int len);

	int receiveBuffer(char * buf, int len);

    const char * lastError(void);
};


