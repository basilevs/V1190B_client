/*
 * socketbuf.cpp
 *
 *  Created on: 20.10.2009
 *      Author: gulevich
 */

#include "socketbuf.h"
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
using namespace std;

//int fdescwrapper::read(char * oBuffer, int length) {
//	if (_fd<0) {return 0;}
//	return ::read(_fd, oBuffer, length);
//}
//
//int fdescwrapper::write(const char * iBuffer, int length) {
//	if (_fd<0) {return 0;}
//	return ::write(_fd, iBuffer, length);
//}

static int setBlocking(int fd, bool doBlock) {
	int flags;
        if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
                flags = 0;
        if (!doBlock) {
                flags |= O_NONBLOCK;
        } else {
                flags &= ~O_NONBLOCK;
        }
        return fcntl(fd, F_SETFL, flags);
}

socketwrapper::socketwrapper(const Host & host, int port):
	_host(host),
	_port(port),
	_socket(-1),
	_noDelay(false)
{}

void socketwrapper::setNoDelay(bool noDelay) {
	_noDelay=noDelay;
}
socketwrapper::~socketwrapper() {
	disconnect();
}

bool socketwrapper::isConnected() const {
	return _socket>=0;
}

void socketwrapper::disconnect() {
	if (_socket==-1) return;
	shutdown(_socket, SHUT_RDWR);
	close(_socket);
	_socket=-1;
}

enum Errors {
	I_HOST_NOT_FOUND=HOST_NOT_FOUND,
	#undef HOST_NOT_FOUND

	I_NO_ADDRESS=NO_ADDRESS,
	#undef NO_ADDRESS

	I_TRY_AGAIN=TRY_AGAIN,
	#undef TRY_AGAIN

	I_NO_RECOVERY=NO_RECOVERY
	#undef NO_RECOVERY

};

#undef NAME_SERVER_ERROR
#undef TIMEOUT
#undef UNREACHABLE
#undef OK
#undef REFUSED


std::string socketwrapper::errorToString(Error error) {
#define p(x) case x: return #x
	switch (error) {
		p(OK);
		p(UNKNOWN_DNS_ERROR);
		p(HOST_NOT_FOUND);
		p(NO_ADDRESS);
		p(TRY_AGAIN);
		p(NAME_SERVER_ERROR);
		p(TIMEOUT);
		p(UNREACHABLE);
		p(UNKNOWN_CONNECT_ERROR);
		p(REFUSED);
		default: assert(!"Unknown error code"); return "UNKNOWN";
	}
#undef p
}


socketwrapper::Error socketwrapper::connect() {

	struct hostent *hp;
	struct sockaddr_in addr;

	typedef socketwrapper Err;
	if((hp = gethostbyname(_host.c_str())) == NULL){
		switch (h_errno) {
		case I_HOST_NOT_FOUND: return Err::HOST_NOT_FOUND;
		case I_NO_ADDRESS: return Err::NO_ADDRESS;
		case I_TRY_AGAIN: return Err::TRY_AGAIN;
		case I_NO_RECOVERY: return Err::NAME_SERVER_ERROR;
		default: return Err::UNKNOWN_DNS_ERROR;
		}
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(_port);
	addr.sin_family = AF_INET;
	disconnect();
	_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket < 0) {
		return UNKNOWN;
	}
	assert(ENOTCONN==107);
	assert(_socket>=0);
	errno=0;
	int on = _noDelay ? 1 : 0;
	if (::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const void *)&on, sizeof(int))== -1) {
		assert(false);
	}
	setBlocking(_socket, true);

	if(::connect(_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		int err=errno;
		switch(err){
			case EINPROGRESS: assert(!"Connection is delayed");
			case ENOTCONN:
			case EHOSTUNREACH: disconnect(); return UNREACHABLE; 
			case ETIMEDOUT: disconnect(); return TIMEOUT;
			case ENETUNREACH: disconnect(); return UNREACHABLE;
			case ECONNREFUSED: disconnect(); return REFUSED;
			case EAGAIN: return TRY_AGAIN;
			default: assert(!"Unknown connect error"); return UNKNOWN_CONNECT_ERROR;
		}
	} else {
		return OK;
	}

	if (errno == EINPROGRESS)	{
		//fd_set wset;
		//FD_ZERO(&wset);
		//assert(_socket>=0);
		//FD_SET(_socket,&wset);
		//timeval timeout;
		//timeout.tv_sec=10;
		//timeout.tv_usec=0;

		///* Wait for write bit to be set */
		//if (select(1,0,&wset,0,&timeout) > 0) {
		//	return OK;
		//} else {
		//	return TIMEOUT;
		//}
	}
	return UNKNOWN_CONNECT_ERROR;
}
inline void dump(ostream & str, char * data, int size, const string & title) {
	return;
	if (size < 0)  {
		str << "Socket returned negative value" << endl;
		return;
	}
	str << title << ":";
	for (int i = 0; i < size; i++) 
		str << hex << (unsigned(0xFF) & unsigned(data[i])) << " ";
	str << dec;
	str << endl;
}


int socketwrapper::read(char * oBuffer, int length) {
	if (!isConnected()) return 0;
	int rv = ::read(socket(), oBuffer, length);
	dump(cerr, oBuffer, length, "low level read");
	return rv;
}

int socketwrapper::write(const char * iBuffer, int length) {
	if (!isConnected()) return 0;
	return ::write(socket(), iBuffer, length);
}


