/*
  Name: ssc32.h
  Copyright: (c) 2006 
  Dscription: siehe ssc32.h
*/
#include "ssc32.h"

SSC32::SSC32() {
  connected=false;
  port=0;
  speed=-1;
  retrievedVersion=false;
  sversion="";
  comfile=-1;
}

SSC32::~SSC32() {
  if (connected) disconnect();
}

void SSC32::connect(int p, int s){
  struct termios options;

  if (connected) disconnect();

  /* COM-Port oeffnen */
  char buffer[100];
  sprintf(buffer, "/dev/ttyS%d\0", port);
  comfile = open(buffer, O_RDWR | O_NOCTTY | O_NDELAY);
  //printf("'port=%d'\n", port);
  //printf("'speed=%d'\n", speed);
  //printf("'%s'\n", buffer);
  port=p;
  speed=s;
  if (comfile<0)
    return ; /* Fehler beim Öffnen der Schnittstelle ist aufgetreten */

  fcntl(comfile, F_SETFL, 0);

  /* aktuelle Optionen holen */
  tcgetattr(comfile, &options);
  
  /* baud-Rate setzen */
  switch (speed) {
    case SSC32Speed2400:
      cfsetispeed(&options, B2400);
      cfsetospeed(&options, B2400);
      break;
    case SSC32Speed9600:
      cfsetispeed(&options, B9600);
      cfsetospeed(&options, B9600);
      break;     
    case SSC32Speed38400:
      cfsetispeed(&options, B38400);
      cfsetospeed(&options, B38400);
      break;   
    case SSC32Speed115200:
      cfsetispeed(&options, B115200);
      cfsetospeed(&options, B115200);
      break;
  }
  
  // 8 Datenbits, 1 Stop-Bit, keine Parity (8N1)
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  
  
  /* raw input, 1 second timeout */
  options.c_cflag     |= (CLOCAL | CREAD);
  options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 10;
  
  /* Optionen schreiben */
  tcsetattr(comfile, TCSANOW, &options);
  connected=true;
}

int SSC32::disconnect(){
  int err=close(comfile);
  connected=false;
  retrievedVersion=false;
  sversion="";
  comfile=-1;
  return err;
}

void SSC32::send(char* command) {
  int cnt=0;
  int i;
  
  if (! connected) return; // Abbrechen, da keine Verbindung besteht

  // Zeichen im Kommando-String zählen
  i=0;
  while (command[i]!='\0') {
    i++;
  }
  // Zeichen senden
  write(comfile, command, i);
}

void SSC32::fgetVersion(char* version) {
  char buffer[255];  /* Input buffer */
  char *bufptr;      /* Current char in buffer */
  int  nbytes;       /* Number of bytes read */
  int  tries;        /* Number of tries so far */
  int counter;
  int i;
  
  if (! connected) return; /* Abbruch, da keine Verbindung besteht */

  /* es werden maximal 3 Versuche unternommen. Wird dabei kein Erfolg
     beim Auslesen erziehlt, so wird ein Fehler gemeldet.
  */
  for (tries = 0; tries < 3; tries ++) {
    /* VER-Kommando senden */
    if (write(comfile, "VER\r", 4) < 4)
      continue;

    /* lies Zeichen, bis CR (\r) oder NL (\n) auftritt  */
    bufptr = buffer;
    counter=0;
    while ((nbytes = read(comfile, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0) {
      bufptr += nbytes;
      counter += nbytes;
      if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
        break;
    }

    i=0;
    while ((buffer[i]!='\n')&&(buffer[i]!='\r')) {
      version[i]=buffer[i];
      i++;
    }
    version[i]=0;
    retrievedVersion=true;
    sversion=version;
    return;
  }
  return;
}


void SSC32::resetAll() {
  send("#0P1500\r");
  send("#1P1500\r");
  send("#2P1500\r");
  send("#3P1500\r");
  send("#4P1500\r");
  send("#5P1500\r");
}

std::string SSC32::getVersion() {
  if (retrievedVersion) {
    return sversion; 
  } else {
    char a[255];
    fgetVersion(a);
    return sversion;
  }
}


void SSC32::addTimestep(SSC32ServoData d){
  timeline.push_back(d);
};

void SSC32::sendTimestep(SSC32ServoData d, int duration){
  char command[512];

  //if (duration>0) sprintf(command, "#0P%dS%d #1P%dS%d #2P%dS%d #3P%dS%d #4P%dS%d #5P%dS%d \rT%d \r\0", d.base, d.speedbase, d.shoulder, d.speedshoulder, d.elbow, d.speedelbow, d.wrist, d.speedwrist, d.gripper, d.speedgripper, d.wristR, d.speedwristR, duration);
  //else sprintf(command, "#0P%dS%d #1P%dS%d #2P%dS%d #3P%dS%d #4P%dS%d #5P%dS%d \r\0", d.base, d.speedbase, d.shoulder, d.speedshoulder, d.elbow, d.speedelbow, d.wrist, d.speedwrist, d.gripper, d.speedgripper, d.wristR, d.speedwristR);
  sprintf(command, "#0P%dS%d #1P%dS%d #2P%dS%d #3P%dS%d #4P%dS%d #5P%dS%d \r\0", d.base, d.speedbase, d.shoulder, d.speedshoulder, d.elbow, d.speedelbow, d.wrist, d.speedwrist, d.gripper, d.speedgripper, d.wristR, d.speedwristR);
  //printf("%s\n\n", command);
  send(command);
};

void SSC32::addTimestep(int time, int base, int shoulder, int elbow, int wrist, int wristR, int gripper){
  SSC32ServoData d;
  d.time=time;
  d.base=base;
  d.speedbase=1200;
  d.shoulder=shoulder;
  d.speedshoulder=1200;
  d.elbow=elbow;
  d.speedelbow=1200;
  d.wrist=wrist;
  d.speedwrist=1200;
  d.wristR=wristR;
  d.speedwristR=1200;
  d.gripper=gripper;
  d.speedgripper=1200;
  timeline.push_back(d);
};

void SSC32::clearTimeline(){
  timeline.clear();
};

void SSC32::playTimeline(){
  int count=timeline.size();
  clock_t starttime;
  double ctime;
  int i;

  if (count<=0) return;

  // Initialposition schreiben und 500 ms warten.
  sendTimestep(timeline[0], 500);
  ctime = ((double) (clock() - starttime)) / CLOCKS_PER_SEC;
  while (ctime<500) {
    ctime = ((double) (clock() - starttime)) / CLOCKS_PER_SEC;
  };

  // weitere Timesteps senden
  starttime=clock();
  i=1;
  while (i<count) {
    ctime = ((double) (clock() - starttime)) / CLOCKS_PER_SEC;
    while (ctime<timeline[i].time) {
      ctime = ((double) (clock() - starttime)) / CLOCKS_PER_SEC;
    };
    if (i<count-1) sendTimestep(timeline[i], (int)(timeline[i+1].time*1000)-(int)(timeline[i].time*1000));
    else sendTimestep(timeline[i], 500);
    i++;
  }
};

