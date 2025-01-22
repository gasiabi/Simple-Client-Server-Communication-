// Klient
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 8000
#define DEFAULT_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char *server_ip = (argc >= 3) ? argv[2] : DEFAULT_ADDR;
    int port = (argc >= 2) ? atoi(argv[1]) : DEFAULT_PORT;

    printf("Łączenie z serwerem %s:%d...\n", server_ip, port);

    // Tworzenie gniazda
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Błąd: nie udało się utworzyć gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Błąd: nieprawidłowy adres IP");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Łączenie z serwerem
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd: nie udało się połączyć z serwerem");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Połączono z serwerem.\n");

    while (1) {
        // Wprowadzenie wiadomości
        printf("Twoja wiadomość: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // Wysłanie wiadomości
        if (write(sock, buffer, strlen(buffer)) < 0) {
            perror("Błąd: nie udało się wysłać wiadomości");
            break;
        }

        // Jeśli wiadomość to "quit", zakończ
        if (strcmp(buffer, "quit") == 0) {
            printf("Zakończono komunikację.\n");
            shutdown(sock, 1);
            break;
        }

        // Oczekiwanie na odpowiedź od serwera
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            printf("Serwer: %s\n", buffer);
        } else if (bytes_read == 0) {
            printf("Serwer zakończył połączenie.\n");
            shutdown(sock, 2);
            close(sock);
            break;
        } else {
            perror("Błąd: nie udało się odebrać wiadomości");
            break;
        }
    }

    close(sock);
    return 0;
}
