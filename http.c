#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// Porta padrão HTTP. Precisa de root.
#define PORT 80

void render_html(int socket, char *page)
{
  FILE *html;
  char buff[5096];
  html = fopen(page, "r");
  fread(buff, 5096, 5096, (FILE*)html);
  // Padrão HTTP: <versão> <status>\nheaders\n\n<body>
  char ok[] = "HTTP/1.1 200\nContent-type: text/html; charset=utf-8\n\n";

  // Concatenar strings dá :poop:, usar dois sends não
  send(socket, ok, strlen(ok) , 0 );
  send(socket, buff, strlen(buff) , 0 );
}

int main(int argc, char const *argv[])
{
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  // Para erritos 404
  char E404[] =  "HTTP/1.1 404\n\nPage Not found";

  // Arquivo de Log
  FILE *fp;

  // criando file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Opções do socket
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  if (bind(server_fd, (struct sockaddr *)&address, 
	sizeof(address))<0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Fique recebendo conexões até alguém parar o processo.
  while(1) {
    fp = fopen("./cacique.log", "a");

    // Abrir nova socket e aguardar uma conexão.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
	    (socklen_t*)&addrlen))<0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // Ler REQUEST e escrever no Log
    read( new_socket , buffer, 1024);
    fprintf(fp, "%s\n", buffer);

    /* Parsear a REQUEST
    Esse C é um pesadelo. Primeiro, arranca a primeira linha da REQUEST.
    Protocolo HTTP tem por padrão a primeira linha ser <método> <URI> <versão>
    Depois quebra por espaço. O do meio é o que interessa.
    */
    char *primeiraLinha = strtok(buffer, "\n");
    char *words;
    char *rest = primeiraLinha;

    for(int i = 0; i < 2; i++) {
      words = strtok_r(rest, " ", &rest);
      // Quero a URL
      if (i != 1) continue; 

      // Devolver RESPONSE
      if( strcmp(words, "/") == 0 ) {
        render_html(new_socket, "./index.html");
        break;
      }

      if( strcmp(words, "/contato") == 0 ) {
        render_html(new_socket,"./contato.html");
        break;
      }

      // Não achei nada...devolve 404
      send(new_socket , E404, strlen(E404) , 0 );
    }


    // Fechar conexão e log
    close(new_socket);
    fclose(fp);
  }

  return 0;
}
