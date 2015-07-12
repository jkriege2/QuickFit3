/**
  * Ansteuerbibliothek für das SSC-32 (ver 2.0) Controlboard zum Lynx6-Arm von
  * Lynxmotion. Dazu wird eine Verbindung zu dem Board über die serielle Schnitt-
  * stelle (= COM-Port) aufgebaut. 
  *
  * Der Timeline-Meachnismus:
  * Eine Timeline enthält die Servopositionen zu verschiedenen Zeitpunkten und ist 
  * somit als Anweisungsfolge für den Arm zu interpretieren. Deswegen sind auch zu
  * jedem Zeitpunkt die Geschwindigkeiten mit abgespeichert. Wird die Timeline abge-
  * spielt, so wird immer zum angegebenen Zeitpunkt der gespeicherte Befehl abgesendet.
  * Die Befehle müssen in chronologischer Reihenfolge abgelegt werden.
  * Mit den addTimestep Befehlen können Zeitschritte hinzugefügt werden. 
  * Mit clearTimeline wird die gespeicherte Timeline gelehrt.
  * Mit playTimeline wird die Timeline abgesielt, also die Befehle an den Lynx6 gesendet.
  
  
  */
#include <stdlib.h>
#include <stdio.h>   
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>   
#include <errno.h>   
#include <termios.h> 
#include <string>
#include <vector>
#include <time.h>


#ifndef SSC32_H
#define SSC32_H

/* Konstanten für die Übertragungsgeschwindigkeit  */
#define SSC32Speed2400 0
#define SSC32Speed9600 1
#define SSC32Speed38400 2
#define SSC32Speed115200 3

typedef struct {
  double time;
  // Positionen
  int base;
  int shoulder;
  int elbow;
  int wrist;
  int wristR;
  int gripper;
  //Geschwindigkeiten
  int speedbase;
  int speedshoulder;
  int speedelbow;
  int speedwrist;
  int speedwristR;
  int speedgripper;
} SSC32ServoData;

class SSC32 {
private:
  bool connected;
  int port;
  int speed;
  bool retrievedVersion;
  std::string sversion;
  int comfile;
  std::vector<SSC32ServoData> timeline;

  /** @param fd          Datei-Objekt
    * @param version     char*-Puffer für die Version der SSC-32-Firmware
    * @return            gibt 0 im Erfolgsfalle und -1 bei einem Fehler zurück
    *
    * liest die Firmware-Version aus dem Board aus.
    */
  void fgetVersion(char* version);
  void sendTimestep(SSC32ServoData d, int duration);
public:
  SSC32();
  ~SSC32();

  std::string getVersion();
  inline int getPort() { return (connected)?port:-1; };
  inline int getSpeed() { return (connected)?speed:-1; };
  inline bool isConnected() { return connected; };


  void addTimestep(SSC32ServoData d);
  void addTimestep(int time, int base, int shoulder, int elbow, int wrist, int wristR, int gripper);
  void addTimestep(int time, int stime, int base, int sbase, int shoulder, int sshoulder, int elbow, int selbow, int wrist, int swrist, int wristR, int swristR, int gripper, int sgripper);
  void clearTimeline();
  void playTimeline();

  /**
    * @param port          COM-Port-Nummer
    * @param speed         Baud-Rate (Übertragungsgeschwindigkeit)
    * @return              gibt ein Datei-Objekt zurück
    *
    * initialisiert die Verbindung zum SSC-32
    * Der erste Parameter gibt die Port-Nummer an (0=COM1, 1=COM2, usw.)
    * Der zweite Parameter gibt die Übertragungsgeschwindigkeit an. Es ist eine
    * der obere vier Konstanten erlaubt, da nur diese Geschwindigkeiten auf dem
    * Board über Jumper eingestellt werden können.
  */
  void connect(int port=0, int speed=3);
  
  /* schließt die Verbindung zum SSC-32  */
  int disconnect();

  
  /** @param fd           Datei-Objekt
    * @param command      ein String mit dem Kommando
    *
    * sendet einen String an das SSC-32
    */
  void send(char* command);
  
  /** @param fd           Datei-Objekt
    *
    * sendet an alle Servos des Lynx6-Armes (Kanäle 0-6) das Signal sich
    * in die Ruhestellung zu bewegen (1.5 ms Pulsbreite)
    */
  void resetAll();

};
#endif
