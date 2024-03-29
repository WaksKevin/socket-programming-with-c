#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

typedef enum
{
    CANNOT_OPEN_FILE = 30,
    DUPLICATE_SERIAL = 31,
    DUPLICATE_REGNO = 32,
    DETAILS_SAVED_SUCCESSFULLY = 33
} Response;

Response add_student_record(char student_details[4][20]);

int main()
{
    int r, sockfd, j, k;
    char recv_buffer[BUFSIZ], send_buffer[BUFSIZ], client[BUFSIZ];
    char student_details[4][20];
    struct addrinfo hints, *host;
    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof client_address;
    time_t connect_time, disconnect_time;

    putchar('\n');

    // configure remote address

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4 connection
    hints.ai_socktype = SOCK_DGRAM; // UDP connection
    hints.ai_flags = AI_PASSIVE;    // listen to any interface

    r = getaddrinfo("127.0.0.1", "8080", &hints, &host);

    if (r != 0)
    {
        puts("❌ Server Malfunction! Failed to configure host address details. Exiting program...");
        exit(EXIT_FAILURE);
    }

    puts("Configured host server address details successfully!");

    // create socket

    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);

    if (sockfd == -1)
    {
        puts("❌ Server Malfunction! Failed to create host server socket. Exiting program...");
        exit(EXIT_FAILURE);
    }

    puts("Host server socket created successfully!");

    // bind socket to address

    r = bind(sockfd, host->ai_addr, host->ai_addrlen);

    if (r == -1)
    {
        puts("❌ Server Malfunction! Failed to bind host to socket. Exiting server program...");
        exit(EXIT_FAILURE);
    }

    puts("Socket bound to address successfully!");
    puts("UDP server is waiting...");

    while (1)
    {
        // receive student details

        r = recvfrom(sockfd, recv_buffer, BUFSIZ, 0, &client_address, &clientaddr_len);

        if (r < 1)
        {
            puts("❌ Received 0 bytes of data");
        }
        else
        {
            getnameinfo(&client_address, clientaddr_len, client, BUFSIZ,0,0,NI_NUMERICHOST); // get client ip
            connect_time = time(NULL); // get client connecting time
            printf("Client %s sent data to the server at \033[34m%s\033[0m", client, ctime(&connect_time));
            
            recv_buffer[r] = '\0'; // terminate the incoming string
            printf("📨 Received %d bytes of data\n", r);
            puts("⌛ Processing data...");

            // extract student details

            j = 0;
            k = 0;
            for (int i = 0; i < (strlen(recv_buffer) - 1); i++)
            {
                // remove the @@@ separators
                if (recv_buffer[i] == '@' && recv_buffer[i + 1] == '@' && recv_buffer[i + 2] == '@')
                {
                    i = i + 2;
                    student_details[j][k] = '\0';
                    j++;
                    k = 0;
                }
                // stopping before the $$$ terminator
                else if (recv_buffer[i] == '$' && recv_buffer[i + 1] == '$' && recv_buffer[i + 2] == '$')
                {
                    student_details[j][k] = '\0';
                    break;
                }
                else
                {
                    student_details[j][k] = recv_buffer[i];
                    k++;
                }
            }

            // add record

            r = add_student_record(student_details);
        }

        // formulate response

        switch (r)
        {
        case CANNOT_OPEN_FILE:
            // message displayed in color red
            strcpy(send_buffer, "\033[31mCould not open file student_details.txt\033[0m");
            break;
        case DUPLICATE_SERIAL:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mDuplicate Serial no. Failed to add record\033[0m");
            break;
        case DUPLICATE_REGNO:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mDuplicate Registration no. Failed to add record\033[0m");
            break;
        case DETAILS_SAVED_SUCCESSFULLY:
            // message displayed in color green
            strcpy(send_buffer, "\033[32mRecord added successfully\033[0m");
            break;
        default:
            // message displayed in color red
            strcpy(send_buffer, "\033[31mError saving record.\033[0m");
            break;
        }
        printf("...%s\n", send_buffer);

        // send response back to client

        r = sendto(sockfd, send_buffer, strlen(send_buffer), 0, &client_address, clientaddr_len);
        if (r < 1)
        {
            puts("❌ Server Malfunction! Failed to send response back to the client");
            exit(EXIT_FAILURE);
        }

        disconnect_time = time(NULL);
        printf("📤 Response sent to client %s at \033[34m%s\033[0m", client, ctime(&disconnect_time));
        puts("\n--------------------------------------------------------");
    }

    // close up

    freeaddrinfo(host);
    close(sockfd);

    return EXIT_SUCCESS;
}

Response add_student_record(char student_details[4][20])
{
    enum user_details
    {
        SERIAL,
        REGNO,
        FNAME,
        LNAME
    };

    FILE *fh;
    char line[100];
    char *token;
    char delimiter[] = "            ";

    fh = fopen("student_details.txt", "a+");
    if (fh == NULL)
        return CANNOT_OPEN_FILE;

    fseek(fh, 0, SEEK_END); // Move the file pointer to the end of the file

    switch (ftell(fh))
    {
        // Check if the file is empty
    case 0:
        fprintf(fh, "SERIAL         REGISTRATION           FULL NAME\n");
        fprintf(fh, "-----------------------------------------------\n");
        break;

    default:
        fseek(fh, -1, SEEK_END); // Move the file pointer to the last character
        if (fgetc(fh) != '\n')
            fprintf(fh, "\n"); // Add a newline if the last character is not a newline

        rewind(fh);
        while (fgets(line, sizeof(line), fh) != NULL)
        {
            if (line[strlen(line) - 1] == '\n')
                line[strlen(line) - 1] = '\0';

            token = strtok(line, delimiter);
            if (token != NULL)
            {
                if (strcmp(student_details[SERIAL], token) == 0)
                {
                    fclose(fh);
                    return DUPLICATE_SERIAL;
                }

                token = strtok(NULL, delimiter); // Move to the next token (REGNO)
                if (token != NULL && strcmp(student_details[REGNO], token) == 0)
                {
                    fclose(fh);
                    return DUPLICATE_REGNO;
                }
            }
        }
        break;
    }

    // Write the record to the file
    fprintf(fh, "%s            %s            %s %s\n", student_details[SERIAL], student_details[REGNO], student_details[FNAME], student_details[LNAME]);

    fclose(fh);

    return DETAILS_SAVED_SUCCESSFULLY;
}
