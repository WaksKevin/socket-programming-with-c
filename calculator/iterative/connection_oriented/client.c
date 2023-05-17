#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// function prototypes
int getuserinput(char buffer[BUFSIZ], char operator[2], char fdigit[9], char sdigit[9]); // function to get user input and format based on application protocol

int main()
{
    int r, sockfd;
    char send_buffer[BUFSIZ], recv_buffer[BUFSIZ], operator[2], fdigit[9], sdigit[9];
    struct addrinfo hints, *server;

    putchar('\n');

    /* ***************************** CONFIGURE REMOTE ADDRESS ********************************* */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo(0, "8080", &hints, &server);

    if (r != 0)
    {
        puts("⛔ Failed to set address of remote server. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ Address of remote server set successfully!");

    /* ************************ CREATE SOCKET *********************** */

    sockfd = socket(server->ai_family,
                    server->ai_socktype,
                    server->ai_protocol);

    if (sockfd == -1)
    {
        puts("⛔ Failed to create client socket. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ Client socket created successfully!");

    /* ******************************* CONNECT TO THE SERVER ********************************* */

    r = connect(sockfd,
                server->ai_addr,
                server->ai_addrlen);
    if (r == -1)
    {
        puts("⛔ Client socket failed to connect to the server. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ Client socket connected to remote server successfully!");
    puts("\n--------------------------------------------------------");

    /* ******************************* GET USER INPUT *********************************** */

    r = getuserinput(send_buffer, operator, fdigit, sdigit);

    if (r == -1)
    {
        puts("⛔ Failed to get user input. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ User input got successfully!");

    /* ******************************* FORMAT INPUT INTO STRING *********************************** */

    strcpy(send_buffer, fdigit);
    strcat(send_buffer, "@@@"); // separator indicator
    strcat(send_buffer, operator);
    strcat(send_buffer, "@@@"); // separator indicator
    strcat(send_buffer, sdigit);
    strcat(send_buffer, "$$$"); // finish with the terminator indicator/

    putchar('\n');

    /* ************************* SEND STRING ******************************* */

    r = send(sockfd, send_buffer, strlen(send_buffer), 0);

    if (r < 1)
    {
        printf("⛔ Failed to send mesage. Exiting program...\n\n");
        exit(EXIT_FAILURE);
    }

    puts("📤 User Input sent to the Server");

    /* *************************** RECEIVE SERVER RESPONSE ********************************* */

    r = recv(sockfd, recv_buffer, BUFSIZ, 0);

    if (r < 1)
    {
        puts("⛔ Failed! Received 0 bytes of data. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    recv_buffer[r] = '\0'; // terminate string
    printf("📨 Received %d bytes of data from the Server\n", r);
    printf("Server responded with '%s'.\n", recv_buffer);
    puts("\n--------------------------------------------------------");

    /* ******************************** CLOSE RESOURCES ************************************ */

    freeaddrinfo(server); // free allocated address memory
    close(sockfd);        // close socket
    putchar('\n');

    return EXIT_SUCCESS;
}

int getuserinput(char buffer[BUFSIZ], char operator[2], char fdigit[9], char sdigit[9])
{

    int trial = 1;
    char confirm;

    while (trial <= 3) // user can try three times to ensure that he/she has entered correct details
    {
        printf("Which operation do you want to perform? (Choose either +, -, * or /): ");
        scanf("%s", operator);

        printf("Enter the first digit: ");
        scanf("%s", fdigit);
        printf("Enter the second digit: ");
        scanf("%s", sdigit);
        printf("You want do '%s %s %s'. Is this correct? (Answer 'y' or 'n'): ", fdigit, operator, sdigit);
        scanf(" %c", &confirm);

        if (confirm == 'y')
        {
            return 0;
        }
        else if (confirm == 'n')
        {
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached!");
            }
            putchar('\n');
            trial++;
        }
        else
        {
            puts("Invalid input (Allowed values are 'y' or 'n')");
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached!");
            }
            putchar('\n');
            trial++;
        }
    }

    return -1;
}
