// Daemon de supervisión de daemon Apache en función - Katerin Amaya - 201020127

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

int main(void) {
  
  pid_t pid, sid;
  int apachePIDfile, apachePIDread, apachePID, daemonLogFileDesc;
  char buf[5], filename[255];        
      
  // Se hace fork del proceso padre 
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  //se cambia la mascara del fichero
  umask(0);
          
  
  //Se abren los ficheros de logs del demonio 
  daemonLogFileDesc = open ("/home/katerin/Escritorio/log", O_WRONLY | O_CREAT, 0600);
  if (daemonLogFileDesc == -1) {
    perror ("Error en la apertura del log");
    exit (EXIT_FAILURE);
  }
  
  
  //Creacion de un identificador de sesión unico para el proceso que ha quedado "huerfano"
  sid = setsid();
  if (sid < 0) {
    perror("nuevo SID ha fallado");
    exit(EXIT_FAILURE);
  }
    
  //cambio de directorio
  if ((chdir("/")) < 0) {
    perror("Fallo en cambio de directorio");
    exit(EXIT_FAILURE);
  }
    
  //Se cierran los descriptores
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
    
 
  /************Codigo del demonio**************/
  
  time_t t;
  struct tm *tlocal;
  char tiempo[100];
    
  apachePIDfile = open("/var/run/apache2.pid", O_RDONLY, 0600);
  if (apachePIDfile == -1) {
    perror("Error en la apertura del fichero");
    exit(EXIT_FAILURE);
  }
  apachePIDread = read (apachePIDfile, buf, sizeof(buf));
    
  while (1) {
    apachePID = atoi (buf);
    snprintf(filename, sizeof(filename), "/proc/%d/cmdline", apachePID);
  
    if ((open (filename, O_RDONLY, 0600)) == -1) {
      perror ("No puedo abrir el fichero en proc");
      exit(EXIT_FAILURE);
    } else {
      
      t=time(NULL);
      tlocal=localtime(&t);
      strftime(tiempo, 100, "Apache running: %d/%m/%Y - %H:%M:%S\n", tlocal);
      write (daemonLogFileDesc, tiempo, 38);
      sleep(20); 
    }
  }
  exit(EXIT_SUCCESS);
}