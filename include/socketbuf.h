#ifndef SOCKETBUF_H_
#define SOCKETBUF_H_

#include <streambuf>
#include <assert.h>

class socketwrapper {
public:
	typedef std::string Host;
private:
	Host _host;
	int _port, _socket;
	bool _noDelay;
public:
	int socket() {return _socket;}
	int read(char * oBuffer, int length);
	int write(const char * iBuffer, int length);
public:
	enum Error {
		OK, UNKNOWN_DNS_ERROR, HOST_NOT_FOUND, NO_ADDRESS, TRY_AGAIN, NAME_SERVER_ERROR, TIMEOUT, UNREACHABLE, UNKNOWN_CONNECT_ERROR, UNKNOWN, REFUSED
	};
	static std::string errorToString(Error);
	socketwrapper(const Host & host, int port);
	void setNoDelay(bool noDelay=true);
	virtual ~socketwrapper();
	Error connect();
	void disconnect();
	bool isConnected() const;
};


template<typename _CharT>
class socketbuf: public std::basic_streambuf<_CharT>, public socketwrapper {
	static const int BUFFER_SIZE=500;

	typedef std::basic_streambuf<_CharT> Parent;
	typedef typename Parent::char_type char_type;
	typedef typename Parent::int_type int_type;
	typedef typename Parent::traits_type traits_type;

	char_type _iBuffer[BUFFER_SIZE], _oBuffer[BUFFER_SIZE];

public:
	socketbuf(const Host & host, int port):
		socketwrapper(host, port) {
		setg(_iBuffer, _iBuffer+BUFFER_SIZE, _iBuffer+BUFFER_SIZE);
		setp(_oBuffer, _oBuffer+BUFFER_SIZE-1);
	}
	virtual ~socketbuf() {}

private:

	void memmove(char_type* to, char_type* from, int size) {
		assert(to <= from);
		if (to==from) return;
		for (int i=0; i<size; ++i) {
			to[i]=from[i];
		}
	}

	int_type underflow() {
		assert(isConnected());
		if (!isConnected()) return traits_type::eof();
		char_type * begin=this->eback(), * current=this->gptr(), * end=this->egptr();
		assert(begin==_iBuffer);
		assert(end<=_iBuffer+BUFFER_SIZE);
		assert(current>=begin);
		assert(current<=end);

		// Move non-read characters to the beginning of the buffer
		int length=end-current;
		memmove(_iBuffer, current, length);

		int byteCount=read(_iBuffer+length, (BUFFER_SIZE-length)*sizeof(char_type));
		if (byteCount<=0) return traits_type::eof();
		setg(_iBuffer, _iBuffer, _iBuffer+byteCount+length);
		return traits_type::to_int_type(_iBuffer[0]);
	}

	int_type writeChars(size_t toWriteCount) {
		assert(isConnected());
		if (!isConnected()) return traits_type::eof();
		char_type * begin=this->pbase(),* current=this->pptr(),* end=this->epptr();
		assert(begin==_oBuffer);
		assert(end<=_oBuffer+BUFFER_SIZE-1);
		assert(current>=begin);
		assert(current<=end);
		assert(toWriteCount <= size_t(BUFFER_SIZE));
		assert(begin+toWriteCount<=end);
		assert(toWriteCount>0);

		int byteCount=write(_oBuffer, toWriteCount*sizeof(char_type));
		if (byteCount<=0) return traits_type::eof();

		char_type lastWrittenCharacter=_oBuffer[byteCount-1];

		int bytesLeft=current-_oBuffer-byteCount;
		assert(bytesLeft < BUFFER_SIZE);
		memmove(_oBuffer, _oBuffer+byteCount, bytesLeft);
		setp(_oBuffer+bytesLeft, _oBuffer+BUFFER_SIZE-1);
		return traits_type::to_int_type(lastWrittenCharacter);
	}

	int_type overflow(int_type c) {
		char_type * current=this->pptr();
		assert(current < _oBuffer+BUFFER_SIZE);
		*current=traits_type::to_char_type(c); //We can write there as current is still not greater than _oBuffer+BUFFER_SIZE-1
		return writeChars(_oBuffer-current+1);
	}
	int sync() {
		int length=this->pptr() - _oBuffer;
		if (length>0) {
			return writeChars(length)==traits_type::eof() ? -1 : 0;
		} else {
			return 0;
		}
	}
};

#endif /* SOCKETBUF_H_ */
