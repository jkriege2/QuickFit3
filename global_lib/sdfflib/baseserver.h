/*
  Name: baseserver.h
  Copyright: (c) 2007
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/
/**
 * \ingroup network
 */
/*@{*/

/** \file baseserver.h
 *
 */

#ifndef BASESERVER_H
#define BASESERVER_H

#include <string>
#include "tools.h"

namespace sequencer {

/**
 * \brief virtual base class for a server (cocket) class.
 * 
 * This is a virtual base class that defines an interface for simple reading and writing
 * functionality of a server class, i.e. a program module that waits for connections and 
 * then accepts them. After Accepting it may send and receive data like from a stream.
 * This base class is implemented by sequencer::TCPIPserver. 
 * 
 * <b>timeout beaviour:</b> The protected property \c timeout
 * can be used by descendents to implement a timeout behaviour on read and write methods, i.e.
 * these methods only wait for the specified to for a method to complete.
 *
 * \ingroup network
 */
class BaseServer
{
  protected:
    /** \brief number of seconds to wait for a read or write call to complete, default is 10 seconds. */
    int timeout;
	public:
    /** \brief virtual class destructor */
    virtual ~BaseServer() {};
    /** \brief class constructor */
    BaseServer() {
      timeout=10;
    }
    
		/** \brief return the timeout for reading and writing operations in seconds */
		GetMacro(int, timeout);
		/** \brief set the timeout for reading and writing operations in seconds */
		SetMacro(int, timeout);

    /** \brief accept the current connection. This method waits/blocks until there is an acceptable connection. Returns an integer that represents the connection and is needed for subsequent calls of read and write. */
		virtual int accept_connection()=0;

    /** \brief check whether there is an entry in \c connections for the given integer */
    virtual bool connection_exists(int conn)=0;

    /** \brief check whether the client is still online and data can be sent */
    virtual bool is_online(int conn)=0;

		/** \brief close the current connection */
		virtual void close_connection(int conn)=0;

		/** \brief read some data. returns the number of bytes read*/
		virtual int read(int conn, char* buffer, size_t len)=0;

    /** \brief read one character from the connection. waits for timeout seconds until it returns with an error if it could not receive a char. If \c timeout=0 then it waits for an indefinite time. */
		virtual char read_char(int conn)=0;

    /** \brief read characters from the connection until the character \c endchar was received. Waits for timeout seconds until it returns with an error if it could not receive the \c endchar. If \c timeout=0 then it waits for an indefinite time. */
		virtual std::string read_str_until(int conn, char endchar)=0;

    /** \brief read a line of characters (ended by '\n') from the connection. Waits for timeout seconds until it returns with an error if it could not receive the \c endchar. If \c timeout=0 then it waits for an indefinite time. */
		virtual inline std::string read_line(int conn) {
      return read_str_until(conn, '\n');
    }

		/** \brief write some data */
		virtual void write(int conn, char* buffer, size_t len)=0;

		/** \brief write a string */
		virtual void write(int conn, std::string data)=0;
		
		/** \brief return some version and status information about the server */
		virtual std::string get_version()=0;
};

}

#endif // BASESERVER_H
