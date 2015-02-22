/*
  Name: tcpipserver.cpp
  Copyright: (c) 2007-2015
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
  License: GPL 3.0
*/

#include "tcpipserver.h" // class's header file

std::string __format(std::string templ, ...){
  va_list ap;
  char buffer[4096];
  va_start (ap, templ);
  vsprintf (buffer, templ.c_str(), ap);
  va_end (ap);
  std::string ret(buffer);
  return ret;
};

std::string __inttostr(int i) {
    return __format("%d", i);
}

#define mmin(x,y) (((x)<(y))?(x):(y))


#define SEQUENCER_ERROR(n, m, w) { fprintf(stderr, "ERROR %d: %s [%s]", n, std::string(m).c_str(), std::string(w).c_str()); }
#define SEQUENCER_ERRORN(n, w) { SEQUENCER_ERROR(n, get_errormessage(n), w) }
#define SEQUENCER_ERRORV(n, w, args...) { SEQUENCER_ERROR(n, __format(sequencer::get_errormessage(n), args), w); }


#define SEQUENCER_NETERROR_STARTFAIL_NUM 42
#define SEQUENCER_NETERROR_SOCKETFAIL_NUM 43
#define SEQUENCER_NETERROR_SENDFAIL_NUM 44
#define SEQUENCER_NETERROR_LISTENFAIL_NUM 45
#define SEQUENCER_NETERROR_BINDFAIL_NUM 46
#define SEQUENCER_NETERROR_ACCEPTFAIL_NUM 47
#define SEQUENCER_NETERROR_TIMEOUT_NUM 48
#define SEQUENCER_NETERROR_CRANGE_NUM 49
#define SEQUENCER_NETERROR_SELECTFAIL_NUM 50
#define SEQUENCER_NETERROR_UTOKEN_NUM 51
#define SEQUENCER_NETERROR_TUNEXPECTED_NUM 52

std::string get_errormessage(int num) {
  switch(num) {
    case SEQUENCER_NETERROR_TUNEXPECTED_NUM:
      return "TCP/IP instruction parser error: found token %s, but expected %s.";
    case SEQUENCER_NETERROR_UTOKEN_NUM:
      return "TCP/IP instruction parser error: unkown token '%c'.";
    case SEQUENCER_NETERROR_SELECTFAIL_NUM:
      return "TCP/IP error: error while waiting for data to read.\nError description: %s";
    case SEQUENCER_NETERROR_CRANGE_NUM:
      return "TCP/IP error: connection index out of range: connection does not exist";
    case SEQUENCER_NETERROR_TIMEOUT_NUM:
      return "TCP/IP error: timeout while reading or writing data";
    case SEQUENCER_NETERROR_ACCEPTFAIL_NUM:
      return "TCP/IP error: accepting connection failed.\nError description: %s";
    case SEQUENCER_NETERROR_SOCKETFAIL_NUM:
      return "TCP/IP error: socket generation failed.\nError description: %s";
    case SEQUENCER_NETERROR_SENDFAIL_NUM:
      return "TCP/IP error: sending data failed.\nError description: %s";
    case SEQUENCER_NETERROR_LISTENFAIL_NUM:
      return "TCP/IP error: could not listen on socket.\nError description: %s";
    case SEQUENCER_NETERROR_BINDFAIL_NUM:
      return "TCP/IP error: could not bind to socket.\nError description: %s";
    case SEQUENCER_NETERROR_STARTFAIL_NUM:
      return "TCP/IP error: WSAStartup failed.";
    default:
      return "";
  }
};

// class constructor
TCPIPserver::TCPIPserver()
{
  backlog=1;
  timeout=10;
  port=48888;
  chunksize=512;
  // Windows WinSock initialisation code
  #ifdef __WINDOWS__
    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
      SEQUENCER_ERRORN(SEQUENCER_NETERROR_STARTFAIL_NUM, "TCPIPserver::TCPIPserver()");
    }
  #endif
}

// class destructor
TCPIPserver::~TCPIPserver()
{
	// insert your code here
}

void TCPIPserver::open_socket(){
  // generate a new TCP/IP stream socket
  sockfd=socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_SOCKETFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SOCKETFAIL_NUM), strerror(errno)), "TCPIPserver::open_socket()");
	return;
  }
  memset(server.sin_zero, '\0', sizeof(server));
  server.sin_family=AF_INET;
  server.sin_port=htons(port);
  server.sin_addr.s_addr = htonl( INADDR_ANY);

  if (bind(sockfd, (struct sockaddr*)&server, sizeof(server))==-1) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_BINDFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_BINDFAIL_NUM), strerror(errno)), "TCPIPserver::open_socket()");
	return;
  }
  if (listen(sockfd, backlog) == -1) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_LISTENFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_LISTENFAIL_NUM), strerror(errno)), "TCPIPserver::open_socket()");
	return;
  }
}

void TCPIPserver::close_socket(){
  close(sockfd);
  shutdown(sockfd,2);
}


int TCPIPserver::accept_connection(){
  int num=get_new_connection_num();
  int len = sizeof(connections[num].client);
  connections[num].fd = accept(sockfd, (struct sockaddr*)&connections[num].client, &len);
  if (connections[num].fd < 0) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_ACCEPTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_ACCEPTFAIL_NUM), strerror(errno)), "TCPIPserver::accept_connection()");
	return 0;
	return 0;
  }
  return num;
}


void TCPIPserver::close_connection(int conn){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::close_connection()");
	return;
  }
  closesocket(connections[conn].fd);
  connections.erase(conn);
}

bool TCPIPserver::connection_exists(int conn){
  for(std::map<int, tcpip_connection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
    if (i->first==conn) return true;
  }
  return false;
}

int TCPIPserver::get_new_connection_num(){
  if (connections.empty()) return 0;
  int maxv=0;
  for(std::map<int, tcpip_connection>::const_iterator i=connections.begin(); i!=connections.end(); ++i) {
    if (i->first>maxv) maxv=i->first;
  }
  return maxv+1;
}




int TCPIPserver::read(int conn, char* buffer, size_t len){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::read()");
	return 0;
  }
  if (timeout==0) return recv(connections[conn].fd, buffer, len, 0);

  int n, rv;
  fd_set readfds;
  struct timeval tv;

  FD_ZERO(&readfds);
  FD_SET(connections[conn].fd, &readfds);
  n=connections[conn].fd+1;
  tv.tv_sec=timeout;
  rv=select(n, &readfds, NULL, NULL, &tv);
  //std::cout<<rv<<std::endl;
  if (rv==-1) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read()");
	return 0;
  } else if (rv==0) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read()");
	return 0;
  } else {
    if (FD_ISSET(connections[conn].fd, &readfds)) {
      return recv(connections[conn].fd, buffer, len, 0);
    } else {
	  SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read()");
	  return 0;
    }
  }
}

char TCPIPserver::read_char(int conn){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::read_char()");
	return 0;
  }
  char d;
  int count=0;
  int n, rv;
  fd_set readfds;
  struct timeval tv;

  if (timeout==0) {
    while (count<1) {
      count=recv(connections[conn].fd, &d, 1, 0);
    }
    return d;
  }

  //std::cout<<"reading one char ... ";
  FD_ZERO(&readfds);
  FD_SET(connections[conn].fd, &readfds);
  n=connections[conn].fd+1;
  tv.tv_sec=timeout;
  rv=select(n, &readfds, NULL, NULL, &tv);
  if (rv==-1) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
	return 0;
  } else if (rv==0) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read_char()");
	return 0;
  } else {
    if (FD_ISSET(connections[conn].fd, &readfds)) {
      count=recv(connections[conn].fd, &d, 1, 0);
      //std::cout<<chartostr(d)<<std::endl;
      if (count>0) return d;
    } else {
	  SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
      return 0;
	}
  }
  return 0;
}

std::string TCPIPserver::read_str_until(int conn, char endchar){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::read_str_until()");
	return "";
  }
  clock_t start;//, end;
  double cpu_time_used;
  char d;
  int count=0;
  std::string answer="";
  int n, rv;
  fd_set readfds;
  struct timeval tv;

  if (timeout==0) {
    do {
      count = recv(connections[conn].fd, &d, 1, 0);
      if ((count>0)&&(d!=endchar)) answer+=d;
    } while ((count > 0)&&(d!=endchar));
    return answer;
  }

  start = clock();
  cpu_time_used=0;
  do {
    FD_ZERO(&readfds);
    FD_SET(connections[conn].fd, &readfds);
    n=connections[conn].fd+1;
    tv.tv_sec=timeout;
    rv=select(n, &readfds, NULL, NULL, &tv);
    //std::cout<<rv<<std::endl;
    if (rv==-1) {
      SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
	  return "";
    } else if (rv==0) {
      SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read_char()");
	  return "";
    } else {
      if (FD_ISSET(connections[conn].fd, &readfds)) {
        count=recv(connections[conn].fd, &d, 1, 0);
      } else {
	    SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
	    return "";
	  }
    }
    if ((count>0)&&(d!=endchar)) answer+=d;
    cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;
  } while ((count > 0)&&(d!=endchar)&&(cpu_time_used<(double)timeout));
  if (cpu_time_used<(double)timeout) return answer;
  SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read_str_until()");
  return "";

}

std::string TCPIPserver::read_str_until(int conn, const std::string& endstring){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::read_str_until()");
	return "";
  }
  clock_t start;//, end;
  double cpu_time_used;
  char d;
  int count=0;
  std::string answer="";
  int n, rv;
  fd_set readfds;
  struct timeval tv;
  bool done=false;

  if (timeout==0) {    
    do {
      count = recv(connections[conn].fd, &d, 1, 0);
      if ((count>0)) answer+=d;
	  done=false;
	  if (answer.size()>=endstring.size()) {
	    done=true;
	    for (size_t i=0; i<endstring.size(); i++) {
		  if (answer[answer.size()-endstring.size()+i]!=endstring[i]) {
		    done=false;
			break;
		  }
	    }
      }
    } while ((count > 0)&&(!done));
    std::string a;
	  for (size_t i=0; i<answer.size()-endstring.size(); i++) {
	      a=a+answer[i];
	  }
	  return a;
  }

  start = clock();
  cpu_time_used=0;
  do {
    FD_ZERO(&readfds);
    FD_SET(connections[conn].fd, &readfds);
    n=connections[conn].fd+1;
    tv.tv_sec=timeout;
    rv=select(n, &readfds, NULL, NULL, &tv);
    //std::cout<<rv<<std::endl;
    if (rv==-1) {
      SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
	  return "";
    } else if (rv==0) {
      SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read_char()");
	  return "";
    } else {
      if (FD_ISSET(connections[conn].fd, &readfds)) {
        count=recv(connections[conn].fd, &d, 1, 0);
      } else {
	    SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::read_char()");
	    return "";
	  }
    }
    if ((count>0)) answer+=d;
	done=false;
    if (answer.size()>=endstring.size()) {
	  done=true;
	  for (size_t i=0; i<endstring.size(); i++) {
	    if (answer[answer.size()-endstring.size()+i]!=endstring[i]) {
  		  done=false;
		  break;
	    }
	  }
    }
    cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;
  } while ((count > 0)&&(!done)&&(cpu_time_used<(double)timeout));
  if (cpu_time_used<(double)timeout) {
	  std::string a;
	  for (size_t i=0; i<answer.size()-endstring.size(); i++) {
	      a=a+answer[i];
	  }
	  return a;
  }
  SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::read_str_until()");
  return "";

}


void TCPIPserver::write(int conn, char* buffer, size_t len){
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::write()");
	return ;
  }
  /*if (send( connections[conn].fd, buffer, len, 0)==-1) {
    SEQUENCER_ERROR(SEQUENCER_NETERROR_SENDFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SENDFAIL_NUM), strerror(errno)), "TCPIPserver::write()");
  };*/
  size_t total=0;     // what has been send so far
  int bytesleft=len; // what is left to send
  int tobesent=mmin(chunksize, bytesleft);
  int n, m, rv;
  fd_set writefds;
  struct timeval tv;
  while (total<len) {
    tobesent=mmin(chunksize, bytesleft);
    if (timeout>0) {
      FD_ZERO(&writefds);
      FD_SET(connections[conn].fd, &writefds);
      m=connections[conn].fd+1;
      tv.tv_sec=timeout;
      rv=select(m, NULL, &writefds, NULL, &tv);
      if (rv==-1) {
        SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::write()");
		return;
      } else if (rv==0) {
        SEQUENCER_ERROR(SEQUENCER_NETERROR_TIMEOUT_NUM, __format(get_errormessage(SEQUENCER_NETERROR_TIMEOUT_NUM), strerror(errno)), "TCPIPserver::write()");
		return;
      } else {
        if (FD_ISSET(connections[conn].fd, &writefds)) {
          n = send(connections[conn].fd, buffer+total, tobesent, 0);
        } else {
			SEQUENCER_ERROR(SEQUENCER_NETERROR_SELECTFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SELECTFAIL_NUM), strerror(errno)), "TCPIPserver::write()");
			return;
		}
      }
    } else {
      n = send(connections[conn].fd, buffer+total, tobesent, 0);
    }
    if (n==-1) {
      SEQUENCER_ERROR(SEQUENCER_NETERROR_SENDFAIL_NUM, __format(get_errormessage(SEQUENCER_NETERROR_SENDFAIL_NUM), strerror(errno)), "TCPIPserver::write()");
	  return;
    }
    total+=n;
    bytesleft-=n;
  }
}

void TCPIPserver::write(int conn, std::string data){
  if (!connection_exists(conn)) {
	SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::write()");
	return;
  }
  write(conn, (char*)data.c_str(), data.size());
}

std::string TCPIPserver::get_client_name(int conn) {
  if (!connection_exists(conn)) {
    SEQUENCER_ERRORN(SEQUENCER_NETERROR_CRANGE_NUM, "TCPIPserver::get_client_name()");
    return "";
  }  
  return __format("%s", inet_ntoa(connections[conn].client.sin_addr));
};

int TCPIPserver::get_socket_status(int sock, int to_secs, int to_msecs)
{

  int sval;
  int ret_val = 1;
  fd_set check_set;
  struct timeval to;



  FD_ZERO(&check_set);
  FD_SET(sock,&check_set);

  to.tv_sec = to_secs;
  to.tv_usec = to_msecs;

  if((sval = select(sock + 1,&check_set,NULL,NULL,&to)) < 0) {
    //printf("Select returned %d %d",sval,errno);
    ret_val = -1;
  }
  if(sval == 0)  {
    //printf("Select timed out.");
    ret_val = 0;
  } else if(sval > 0) {
    //printf("Select shows %d as return val.",sval);
  }

  return(ret_val);

}

bool TCPIPserver::is_online(int conn) {
  return (get_socket_status(connections[conn].fd, 0, 10)>=0);
};


std::string TCPIPserver::get_version() {
  std::string data="";
  #ifdef __WINDOWS__
    data+="driver: WinSock32";
    data+=" supports ver"+__inttostr(wsaData.wVersion & 0xFF)+"."+__inttostr((wsaData.wVersion & 0xFF00)>>8);
    data+=" to ver"+__inttostr(wsaData.wHighVersion & 0xFF)+"."+__inttostr((wsaData.wHighVersion & 0xFF00)>>8)+"\n";
    data+=__format("description: %s\n", wsaData.szDescription);
    data+=__format("system status: %s\n", wsaData.szSystemStatus);
  #else
    data+="driver: Unix Socket Library  'sys/socket.h'\n";
  #endif
  return data;
};


