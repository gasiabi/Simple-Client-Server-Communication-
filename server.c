// Serwer
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
#define BACKLOG 5

int main(int argc, char *argv[]) {
    int server_fd, newsock, port, pid;
    struct sockaddr_in server_addr, cl_addr;
    socklen_t clen;

    // Ustawienie portu
    port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    printf("Serwer uruchomiony na porcie %d.\n", port);

    // Tworzenie gniazda
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Błąd: nie udało się utworzyć gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Przypisanie adresu do gniazda
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Błąd: nie udało się przypisać adresu do gniazda");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Rozpoczęcie nasłuchiwania
    if (listen(server_fd, BACKLOG) == -1) {
        perror("Błąd: nie udało się uruchomić nasłuchiwania");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Serwer nasłuchuje na porcie %d...\n", port);

    while (1) {
        clen = sizeof(cl_addr);
        // Akceptowanie połączenia
        newsock = accept(server_fd, (struct sockaddr *)&cl_addr, &clen);
        if (newsock < 0) {
            perror("server: błąd w funkcji accept()");
            continue;
        } else {
            switch (pid = fork()) {
            case 0:
                // Dziecko -- obsługuje dopiero co nawiązane połączenie
                close(server_fd);

                char buffer[1024];
                int bytes_read;

                printf("Połączono z klientem.\n");

                while (1) {
                    memset(buffer, 0, sizeof(buffer));
                    bytes_read = read(newsock, buffer, sizeof(buffer) - 1);
                    if (bytes_read > 0) {
                        printf("Klient: %s\n", buffer);

                        // Jeśli klient wysyła "quit", zamknij połączenie
                        if (strcmp(buffer, "quit\n") == 0) {
                            printf("Klient zakończył połączenie.\n");
                            break;
                        }

                        // Odpowiedz klientowi (echo)
                        write(newsock, buffer, strlen(buffer));
                    } else if (bytes_read == 0) {
                        printf("Klient zakończył połączenie.\n");
                        break;
                    } else {
                        perror("Błąd: nie udało się odczytać danych od klienta");
                        break;
                    }
                }

                shutdown(newsock, 2);
                close(newsock);
                exit(0);

            case -1:
                perror("błąd podczas fork()");
                break;

            default:
                // Rodzic
                close(newsock); // Rodzic wraca do oczekiwania na nowe połączenia
                break;
            }
        }
    }

    close(server_fd);
    return 0;
}