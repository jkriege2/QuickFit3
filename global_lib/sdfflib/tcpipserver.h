/*
  Name: tcpipserver.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup network
 */
/*@{*/

/** \file tcpipserver.h
 *
 */

#ifndef TCPIPSERVER_H
#define TCPIPSERVER_H

#include "tools.h"
#include "sequencererror.h"
#include "baseserver.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <fcntl.h>

/* now first we will have to check the operating system and from this decide
   which libraries (H files) to include */
   
#ifndef __WINDOWS__
#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__
  /* headerfiles for Windows */
  #include <winsock.h>
  #include <io.h>
#else
  /* headerfiles for Unix/Linux */
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  /* this macro implements a method not present on Unix systems */
  #define closesocket(s) close(s)
#endif

namespace sequencer {

/** \brief data about one connection
 * \ingroup network
 */
typedef struct {
    struct sockaddr_in client; /** \brief socket data for the client */
    int fd; /** \brief socket number of the client socket */
} tcpip_connection;

/**
 * \brief This class encapsules the TCP/IP functionality of network sockets on Windows and Linux. It provides methods to program a server that listens for incomming connections and may answer them.
 *
 * \ingroup network
 *
 * <b>Standard Initial Values:</b>
 *  - port number: \c 48888
 *  - backlog: \c 1
 * .
 * 
 * <b>timeout behaviour:</b> If a timeout > 0 (see sequencer::BaseServer) is set then all read and 
 * write methods implement a timeout behaviour. They try to read or write only until the timeout
 * period ended. This is implemented with the \c select() method of the socket API.
 *  - <a href="http://msdn2.microsoft.com/en-us/library/ms740141.aspx">MSDN: select()</a>
 * .
 *
 * <b>Linking and Compiling:</b> You will need to link your program against
 * the network library of your programming environment. On MS Windows systems
 * you will have to link \c -lwsock32. On Linux systems you do not have to link to
 * a special library, as the \c libc already contains the socket methods and functions. 
 * Some preprocessor directives in the header of this file do the switching between the 
 * two possible systems.
 *
 * <b>Sockets on Windows and Unix/Linux:</b>
 * It is possible to write source code compatible programs for Windows and Linux, because
 * The Winsock library from Microsoft implements all the POSIX (Linux/Unix) socket functions
 * and methods. For more details see the 
 * <a href="http://en.wikipedia.org/wiki/Winsock">Wikipedia article on Winsock</a>.
 *
 * There is one incompatibility between Windows and Linux: We first have to open Winsock,
 * which is not necessary in the POSIX environment. But this will be dealt with in the
 * class constructor TCPIPserver::TCPIPserver().
 *
 * <b>Further Reading:</b> There are some good tutorials about socket programming on the
 * internet. Here is a list:
 *  - <a href="http://cplus.kompf.de/artikel/httpserv.html">a simple HTTP server example</a> <i>(german)</i>
 *  - <a href="http://cplus.kompf.de/artikel/httpget.html">a simple HTTP client example</a> <i>(german)</i>
 *  - <a href="http://www.beej.us/guide/bgnet/">Beej's Guide to Network Programming</a>: a very 
 *    comprehensive tutorial which is also available as PDF. <i>(english0)</i>
 *  - <a href="http://www.pc-adviser.de/socket_programmierung.html">Mavericks Tipps zur Socket-Programmierung</a> <i>(german)</i>
 *  - <a href="http://www.willemer.de/informatik/unix/unprsock.htm">Client-Server Socketprogrammierung</a> <i>(german)</i>
 * .
 * This is the Winsock32 Documentation:
 *  - <a href="http://msdn2.microsoft.com/en-us/library/ms740673.aspx">MSDN: Windows Sockets 2</a>   
 * .
 */
class TCPIPserver: public BaseServer
{
  protected:
    #ifdef __WINDOWS__
      /** \brief winsock32 data structure ... needed and implemented only on win32 systems */
      WSADATA wsaData;
    #endif
    /** \brief network port to listen on */
    int port;
    /** \brief how many bytes should be sent at once at the max */
    int chunksize;
    /** \brief socket number of the server socket */
    int sockfd;
    /** \brief number of pending connections allowed */
    int backlog;
    /** \brief socket data for the server */
    struct sockaddr_in server;
    /** \brief a map that contains info about open connections */
    std::map<int, tcpip_connection> connections;
    /** \brief return a number for a connection. This has to be a number that is not yet in \c connections. */
    int get_new_connection_num();
    /** \brief set socket into non-blocking mode. */
    inline void set_nonblocking() {
      #ifdef __WINDOWS__
        DWORD NonBlock = 1;
        ioctlsocket(sockfd, FIONBIO, &NonBlock);
      #else
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)
      #endif  
    };
    /** \brief set socket into blocking mode. */
    inline void set_blocking() {
      #ifdef __WINDOWS__
        DWORD NonBlock = 0;
        ioctlsocket(sockfd, FIONBIO, &NonBlock);
      #else
        int flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK))
      #endif
    };
    /** \brief uses select (timeout given in \c to_secs, \c to_msecs) on socket \c sock to find out whether 
     * we can read data (return 1) or whether there was an error (return -1). If the socket is OK, but there 
     * is no data to read it returns 0. For more information see this 
     * <a href="http://www.developerweb.net/forum/showthread.php?t=2935">forum post</a> .
     */
    int get_socket_status(int sock, int to_secs, int to_msecs) ;
	public:
		/** \brief class constructor this also starts WinSock in Windows systems (see class description for details). */
		TCPIPserver();
		/** \brief class destructor */
		~TCPIPserver();
		
		/** set the TCP/IP port number this component is listening to. Standard port is \c 48888. */
		SetMacro(int, port);
		/** \brief return the TCP/IP port number this component is listening to */
		GetMacro(int, port);

		/** set the size of one chunk of data that should be sent. Default is \c 256 Bytes. */
		SetMacro(int, chunksize);
		/** \brief get the size of one chunk of data that should be sent */
		GetMacro(int, chunksize);

		/** \brief return the backlog property.*/
		SetMacro(int, backlog);
		/** \brief set the backlog property, which sepcifies the number of pending connections allowed.*/
	 	GetMacro(int, backlog);

		/** \brief start the socket and bind to it. */
		void open_socket();

		/** \brief close the socket and clean up */
		void close_socket();

		/** \brief accept the current connection. This method waits/blocks until there is an acceptable connection. 
     * Returns an integer that represents the connection and is needed for subsequent calls of read and write. 
     */
		virtual int accept_connection();
		
    /** \brief check whether there is an entry in \c connections for the given integer */
    virtual bool connection_exists(int conn);
    
    /** \brief check whether the client is still online and data can be sent */
    virtual bool is_online(int conn);

		/** \brief close the current connection */
		virtual void close_connection(int conn);

		/** \brief read some data. returns the number of bytes read*/
		virtual int read(int conn, char* buffer, size_t len);
		
    /** \brief read one character from the connection. waits for \c timeout seconds until it returns 
     * with an error if it could not receive a char. If \c timeout=0 then it waits for an indefinite time. 
     */
		virtual char read_char(int conn);

    /** \brief read characters from the connection until the character \c endchar was received. Waits for 
     * timeout seconds until it returns with an error if it could not receive the \c endchar. If \c timeout=0 
     * then it waits for an indefinite time. 
     */
		virtual std::string read_str_until(int conn, char endchar);

		/** \brief write some data. 
		 *
     * This method makes sure that all data is really sent and may therefore break up the data into smaller peaces. 
     * See <a href="http://www.beej.us/guide/bgnet/">Beej's Guide to Network Programming</a> (chapter 6.3, method 
     * \c sendall() ) for details. This method breaks up the data in buffer into packet with the size chunksize
     * that will be sent one by the other. This allows to wait for the client to empty its input buffer.
     *
     * This is important, as the send() method of the socket API does not guarantee to send all bytes out of the
     * buffer. It may (for one or another system-specific reason) decide to send only a part of them. It will then
     * return the number of sent bytes and the user will have to retry sending the rest. This method implements 
     * this retry scheme.
     * 
     * If \c timeout>0 then this method will wait \c timeout seconds at the most until it stops sending with a
     * SEQUENCER_NETERROR_TIMEOUT_NUM exception. If \c timeout=0 This method will not use any timeout behaviour
     * and simply send the data or exit with an exception if any error occured (client not ready or so).
     */
		virtual void write(int conn, char* buffer, size_t len);

		/** \brief write a string */
		virtual void write(int conn, std::string data);

		/** \brief returns the client adress/name */
		std::string get_client_name(int conn);
		
		/** \brief return version information about winsock32 on windows systems 
     *
     * On windows systems this returns some etries of the WSADATA record written by WSAStartup(). You can find
     * more information about this in <a href="http://msdn2.microsoft.com/en-us/library/ms742213.aspx">MSDN</a>.
     */
		virtual std::string get_version();
};

}

#endif // TCPIPSERVER_H
