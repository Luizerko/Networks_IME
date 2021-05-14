/* Por Prof. Daniel Batista <batista@ime.usp.br>
 * Em 4/4/2021
 * 
 * Um código simples de um servidor de eco a ser usado como base para
 * o EP1. Ele recebe uma linha de um cliente e devolve a mesma linha.
 * Teste ele assim depois de compilar:
 * 
 * ./ep1-servidor-exemplo 8000
 * 
 * Com este comando o servidor ficará escutando por conexões na porta
 * 8000 TCP (Se você quiser fazer o servidor escutar em uma porta
 * menor que 1024 você precisará ser root ou ter as permissões
 * necessáfias para rodar o código com 'sudo').
 *
 * Depois conecte no servidor via telnet. Rode em outro terminal:
 * 
 * telnet 127.0.0.1 8000
 * 
 * Escreva sequências de caracteres seguidas de ENTER. Você verá que o
 * telnet exibe a mesma linha em seguida. Esta repetição da linha é
 * enviada pelo servidor. O servidor também exibe no terminal onde ele
 * estiver rodando as linhas enviadas pelos clientes.
 * 
 * Obs.: Você pode conectar no servidor remotamente também. Basta
 * saber o endereço IP remoto da máquina onde o servidor está rodando
 * e não pode haver nenhum firewall no meio do caminho bloqueando
 * conexões na porta escolhida.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
    /* Os sockets. Um que será o socket que vai escutar pelas conexões
     * e o outro que vai ser o socket específico de cada conexão */
    int listenfd, connfd;
    /* Informações sobre o socket (endereço e porta) ficam nesta struct */
    struct sockaddr_in servaddr;
    /* Retorno da função fork para saber quem é o processo filho e
     * quem é o processo pai */
    pid_t childpid;
    /* Armazena linhas recebidas do cliente */
    unsigned char recvline[MAXLINE + 1];
    /* Armazena o tamanho da string lida do cliente */
    ssize_t n;
   
    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

    /* Criação de um socket. É como se fosse um descritor de arquivo.
     * É possível fazer operações como read, write e close. Neste caso o
     * socket criado é um socket IPv4 (por causa do AF_INET), que vai
     * usar TCP (por causa do SOCK_STREAM), já que o MQTT funciona sobre
     * TCP, e será usado para uma aplicação convencional sobre a Internet
     * (por causa do número 0) */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    /* Agora é necessário informar os endereços associados a este
     * socket. É necessário informar o endereço / interface e a porta,
     * pois mais adiante o socket ficará esperando conexões nesta porta
     * e neste(s) endereços. Para isso é necessário preencher a struct
     * servaddr. É necessário colocar lá o tipo de socket (No nosso
     * caso AF_INET porque é IPv4), em qual endereço / interface serão
     * esperadas conexões (Neste caso em qualquer uma -- INADDR_ANY) e
     * qual a porta. Neste caso será a porta que foi passada como
     * argumento no shell (atoi(argv[1]))
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

    /* Como este código é o código de um servidor, o socket será um
     * socket passivo. Para isto é necessário chamar a função listen
     * que define que este é um socket de servidor que ficará esperando
     * por conexões nos endereços definidos na função bind. */
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }

    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
    /* O servidor no final das contas é um loop infinito de espera por
     * conexões e processamento de cada uma individualmente */
	for (;;) {
        /* O socket inicial que foi criado é o socket que vai aguardar
         * pela conexão na porta especificada. Mas pode ser que existam
         * diversos clientes conectando no servidor. Por isso deve-se
         * utilizar a função accept. Esta função vai retirar uma conexão
         * da fila de conexões que foram aceitas no socket listenfd e
         * vai criar um socket específico para esta conexão. O descritor
         * deste novo socket é o retorno da função accept. */
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }
      
        /* Agora o servidor precisa tratar este cliente de forma
         * separada. Para isto é criado um processo filho usando a
         * função fork. O processo vai ser uma cópia deste. Depois da
         * função fork, os dois processos (pai e filho) estarão no mesmo
         * ponto do código, mas cada um terá um PID diferente. Assim é
         * possível diferenciar o que cada processo terá que fazer. O
         * filho tem que processar a requisição do cliente. O pai tem
         * que voltar no loop para continuar aceitando novas conexões.
         * Se o retorno da função fork for zero, é porque está no
         * processo filho. */
        if ( (childpid = fork()) == 0) {
            /**** PROCESSO FILHO ****/
            printf("[Uma conexão aberta]\n");
            /* Já que está no processo filho, não precisa mais do socket
             * listenfd. Só o processo pai precisa deste socket. */
            close(listenfd);
         
            /* Agora pode ler do socket e escrever no socket. Isto tem
             * que ser feito em sincronia com o cliente. Não faz sentido
             * ler sem ter o que ler. Ou seja, neste caso está sendo
             * considerado que o cliente vai enviar algo para o servidor.
             * O servidor vai processar o que tiver sido enviado e vai
             * enviar uma resposta para o cliente (Que precisará estar
             * esperando por esta resposta) 
             */

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 INÍCIO                        */
            /* ========================================================= */
            /* ========================================================= */
            /* TODO: É esta parte do código que terá que ser modificada
             * para que este servidor consiga interpretar comandos MQTT  */            

            //Lê os primeiros pacotes enviados pelo socket do cliente.
            if((n=read(connfd, recvline, MAXLINE)) > 0) {
                recvline[n] = 0;

                //Verifica os 4 bits mais significativos do primeiro byte para garantir
                //que fora devidamente recebido um pacote com flag de conexão. Em caso
                //afirmativo, um sinal de connack será enviado - sinal esse acompanhado
                //de mais 1 byte indicando a quantidade de bytes restantes a serem
                //enviados bytes e seguido dos outros 53 bytes típicos com flags - em
                //nosso caso, todas zeradas.
                unsigned int control_packet = recvline[0] >> 4;
                if (control_packet == 1) {
                    recvline[0] = 32;
                    recvline[1] = 53;
                    for (int i = 0; i < recvline[1]; i++) {
                        recvline[i+2] = 0;
                    }
                    
                    write(connfd, recvline, 55);
                }
                else {
                    perror("connect :(");
                    exit(1);
                }

                for (int i = 0; i < MAXLINE+1; i++) {
                    recvline[i] = 0;
                }

                //O socket do cliente é lido novamente para indicar o próximo comando
                //através dos 4 bits mais significativos do primeiro byte. No caso do
                //control_packet ser igual a 3, sabemos que o cliente será um cliente de
                //publicação. No caso do control_packet ser igual a 8, teremos um cliente
                //de inscrição.
                n = read(connfd, recvline, MAXLINE);
                control_packet = recvline[0] >> 4;
                
                if (control_packet == 3) {

                    //Tratamento dos pacotes recebidos para devida interpretação do tópico
                    //escolhido e da mensagem a ser enviada àquele tópico. Antes de prosseguir,
                    //no entanto, devemos primeiro verificar se o tamanho da mensagem é maior 
                    //do que 127 devido ao protocolo MQTT que adiciona um byte ao tamanho da
                    //mensagem nesse caso.
                    unsigned int topic_len;
                    char* topic;
                    unsigned char* message;

                    if(recvline[1] <= 127) {
                        topic_len = recvline[2]*256;
                        topic_len += recvline[3];
                        topic = malloc((topic_len+1)*sizeof(unsigned char));
                        for (int i = 4; i < 4 + topic_len; i++) {
                            topic[i-4] = recvline[i];
                        }
                        topic[topic_len] = 0;

                        unsigned int tam = n - 2 - (4 + topic_len) + 1 + 1;
                        message = malloc(tam*sizeof(unsigned char));

                        for (unsigned int i = 4 + topic_len + 1; i < n; i++) {
                            message[i-(4 + topic_len + 1)] = recvline[i];
                        }

                    }

                    else {
                        topic_len = recvline[3]*256;
                        topic_len += recvline[4];
                        topic = malloc((topic_len+1)*sizeof(unsigned char));
                        for (int i = 5; i < 5 + topic_len; i++) {
                            topic[i-5] = recvline[i];
                        }
                        topic[topic_len] = 0;

                        unsigned int tam = n - 2 - (5 + topic_len) + 1 + 1;
                        message = malloc(tam*sizeof(unsigned char));

                        for (unsigned int i = 5 + topic_len + 1; i < n; i++) {
                            message[i-(5 + topic_len + 1)] = recvline[i];
                        }

                    }

                    //Escrita da mensagem do cliente de publicação no arquivo que
                    //representa o tópico escolhido.
                    FILE* f;
                    if (access(topic, F_OK) != 0) {
                        f = fopen(topic, "w");
                        fclose(f);
                    }
                    else {
                        f = fopen(topic, "a");
                    }
                    fprintf(f, "%s\n", message);
                    fclose(f);

                }

                else if (control_packet == 8) {
                    
                    //Tratamento dos pacotes recebidos para devida interpretaçãoe
                    //processamento do identificador da mensagem de inscrição - o que
                    //verificará que escreveremos para o mesmo cliente que nos enviou a
                    //requisição de inscrição -, e do tópico ao qual o cliente
                    //inscreveu-se.
                    unsigned int message_id = recvline[2]*256;
                    message_id += recvline[3];

                    unsigned int topic_len = recvline[5]*256;
                    topic_len += recvline[6];
                    char* topic = malloc((topic_len+1)*sizeof(char));
                    for (int i = 7; i < 7 + topic_len; i++) {
                        topic[i-7] = recvline[i];
                    }
                    topic[topic_len] = 0;
                    
                    //Envio de 1 byte de resposta suback para o cliente que se inscreveu. 
                    //Junto com a resposta de suback, também são enviados 1 byte contendo
                    //a quantidade restante de bytes na mensagem, 2 bytes com o
                    //identificador de mensagem anteriormente lido para verificação,
                    //e 2 bytes nulos de propriedades e flags outras que não serão
                    //utilizadas por esse servidor.
                    recvline[0] = 144;
                    recvline[1] = 4;
                    recvline[2] = message_id >> 8;
                    recvline[3] = message_id & 255;
                    recvline[4] = 0;
                    recvline[5] = 0;
                    write(connfd, recvline, 6);

                    //Trata do arquivo do tópico. Em caso de não existência prévia do
                    //tópico, um novo arquivo é criado para representar aquele tópico.
                    //Com o arquivo de tópio em mãos, o programa verifica o momento da
                    //última atualização do mesmo para depois entrar num laço principal.
                    //Ness laço, o programa verifica se alguma mensagem foi mandada pelo
                    //cliente, podendo essa mensagem ser de dois tipos: uma requisição de
                    //desconexão - em caso de saída voluntária do cliente - ou uma
                    //requisição de ping - teste de conexão em caso de inatividade por
                    //parte do servidor. Se nenhuma mensagem foi enviada pelo cliente
                    //em até 0.1 segundos quando da leitura do socket pelo servidor,
                    //um comando de timeout foi configurado para que o servidor continue
                    //sua principal função de averiguar edições no arquivo do tópico para
                    //envio da útlima linha editada para o cliente. 
                    FILE* f;
                    if (access(topic, F_OK) != 0) {
                        f = fopen(topic, "w");
                        fclose(f);
                    }

                    time_t old_time;
                    struct stat file_stat;
                    char* path = malloc((strlen(topic)+3)*sizeof(char));
                    path[0] = '.';
                    path[1] = '/';
                    strcat(path, topic);
                    path[strlen(topic)+2] = '\0';
                    int err = stat(path, &file_stat);
                    if (err != 0) {
                        perror("stat sub :(\n");
                        exit(2);
                    }
                    old_time = file_stat.st_mtime;

                    //Configuração de timeout de 0.1 segundos para comandos de leitura
                    //envolvendo o scoket do cliente de 0.1 segundos.
                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 100000;
                    setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

                    while(1) {
                        int err = stat(path, &file_stat);
                        if (err != 0) {
                            perror("stat sub laco :(\n");
                            exit(3);
                        }

                        if ((n = read(connfd, recvline, MAXLINE)) > 0) {
                            //Verifica requisição de desconexão. Em caso positivo o
                            //programa mata o processo, encerrando a conexão com o
                            //cliente.
                            if ((recvline[0] >> 4) == 14)
                                break;

                            //Verifica requisição de ping. Em caso positivo, o servidor
                            //manda uma 1 byte com o sinal de resposta de ping e 1 outro
                            //byte indicando que não resta mais nenhum pacote a ser
                            //enviado. 
                            else if ((recvline[0] >> 4) == 12) {
                                recvline[0] = 208;
                                recvline[1] = 0;
                                write(connfd, recvline, 2);
                            }
                        }

                        //Verifica se o arquivo referente ao tópico ao qual o cliente
                        //está inscrito teve alguma alteração. Em caso positivo, a última
                        //linha do arquivo é devidamente passada para o cliente através
                        //de uma mensagem de publicação. Essa mensagem de publicação é
                        //composta de 1 primeiro byte de controle que sinaliza uma
                        //uma mensagem de publicação, 1 segundo byte que contabiliza
                        //o número de pacotes restantes a serem enviados, 2 bytes com
                        //o tamanho do tópico ao qual a mensagem será enviada, topic_len
                        //bytes com o nome do tópico efetivamente e strlen(line)-1 bytes
                        //com a mensagem a ser passada para o cliente - (-1) para
                        //desconsiderar o '\n' da string.
                        if (file_stat.st_mtime != old_time) {
                            old_time = file_stat.st_mtime;
                            f = fopen(topic, "r+");
                            char* line = NULL;
                            size_t len = 0;
                            ssize_t read;
                            while((read = getline(&line, &len, f)) != -1);
                            recvline[0] = 48;
                            if ((2 + topic_len + 1 + strlen(line)-1) <= 127) {
                                recvline[1] = 2 + topic_len + 1 + strlen(line)-1;
                                recvline[2] = topic_len >> 8;
                                recvline[3] = topic_len & 255;
                                for (int i = 4; i < 4 + topic_len; i++) {
                                    recvline[i] = topic[i-4];
                                }
                                recvline[4+topic_len] = 0;
                                for (int i = 4+topic_len+1; i < 4+topic_len+1+strlen(line)-1; i++) {
                                    recvline[i] = line[i-(4+topic_len+1)];
                                }
                                write(connfd, recvline, 4+topic_len+1+strlen(line)-1);
                            }
                            else {
                                if(((2 + topic_len + 1 + strlen(line)-1) & 255) <= 127)
                                    recvline[1] = ((2 + topic_len + 1 + strlen(line)-1) & 255) + 128;
                                else
                                    recvline[1] = ((2 + topic_len + 1 + strlen(line)-1) & 255);
                                recvline[2] = (2 + topic_len + 1 + strlen(line)-1)/128;
                                recvline[3] = topic_len >> 8;
                                recvline[4] = topic_len & 255;
                                for (int i = 5; i < 5 + topic_len; i++) {
                                    recvline[i] = topic[i-5];
                                }
                                recvline[5+topic_len] = 0;
                                for (int i = 5+topic_len+1; i < 5+topic_len+1+strlen(line)-1; i++) {
                                    recvline[i] = line[i-(5+topic_len+1)];
                                }
                                write(connfd, recvline, 5+topic_len+1+strlen(line)-1);
                            }
                            fclose(f);
                        }

                        //sleep(0.1);
                    }
                }

            }
            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
            /* ========================================================= */

            /* Após ter feito toda a troca de informação com o cliente,
             * pode finalizar o processo filho */
            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho) */
            close(connfd);
    }
    exit(0);
}
