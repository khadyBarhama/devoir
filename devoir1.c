
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
// Fonction f(n) = 2^n
int f(int n) {
    return (int)pow(2, n);
}

// Fonction g(n) = 2*n
int g(int n) {
    return 2 * n;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage : %s -f n | -g n | -f n -g | -g n -f\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = 0;
    int has_f = 0, has_g = 0;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Erreur : Option -f nécessite un entier comme argument.\n");
                return EXIT_FAILURE;
            }
            n = atoi(argv[i + 1]);
            has_f = 1;
            i++;
        } else if (strcmp(argv[i], "-g") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Erreur : Option -g nécessite un entier comme argument.\n");
                return EXIT_FAILURE;
            }
            n = atoi(argv[i + 1]);
            has_g = 1;
            i++;
        }
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Processus fils
        close(pipe_fd[0]); // Fermer le côté lecture
        int result = 0;

        if (has_f && has_g) {
            if (strcmp(argv[1], "-f") == 0) {
                // Calcul de f(g(n))
                result = f(g(n));
            } else {
                // Calcul de g(f(n))
                result = g(f(n));
            }
        } else if (has_f) {
            // Calcul de f(n)
            result = f(n);
        } else if (has_g) {
            // Calcul de g(n)
            result = g(n);
        }

        write(pipe_fd[1], &result, sizeof(result));
        close(pipe_fd[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Processus parent
        close(pipe_fd[1]); // Fermer le côté écriture
        int result;
        read(pipe_fd[0], &result, sizeof(result));
        close(pipe_fd[0]);

        if (has_f && has_g) {
            if (strcmp(argv[1], "-f") == 0) {
                printf("f(g(%d)) = %d\n", n, result);
            } else {
                printf("g(f(%d)) = %d\n", n, result);
            }
        } else if (has_f) {
            printf("f(%d) = %d\n", n, result);
        } else if (has_g) {
            printf("g(%d) = %d\n", n, result);
        }

        wait(NULL); // Attendre la fin du processus fils
    }

    return EXIT_SUCCESS;
}

