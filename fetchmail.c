#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <sys/types.h>
#include <sys/time.h>
#define MAX_BUFFER_SIZE 10240

void receive_with_timeout(BIO *bio, char *total_buffer, char *recv_buffer, size_t buffer_size) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(BIO_get_fd(bio, NULL), &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    while (select(BIO_get_fd(bio, NULL) + 1, &read_fds, NULL, NULL, &timeout) > 0) {
        BIO_read(bio, recv_buffer, 10240);
        strcat(total_buffer, recv_buffer);
    }
}

// Helper function to send and receive 
void send_receive_ssl(BIO *bio, const char *send_cmd, char *recv_buffer, size_t buffer_size) {
    // Send command
    BIO_puts(bio, send_cmd);

    // Receive response
    BIO_read(bio, recv_buffer, buffer_size);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Throwaway Gmail account created for this project, password is the app password
    const char *hostname = argv[1];
    const char *username = "tzeng0088@gmail.com";
    const char *password = "vhkycyvvnoxcoejj";

    SSL_library_init();
    SSL_load_error_strings();

    // Create SSL context and BIO
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    BIO *bio = BIO_new_ssl_connect(ssl_ctx);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);

    // Set up connection
    BIO_set_conn_hostname(bio, hostname);
    BIO_set_conn_port(bio, "imaps");

    // Open connection
    if (BIO_do_connect(bio) <= 0) {
        fprintf(stderr, "Error: Failed to connect to the server.\n");
        return EXIT_FAILURE;
    }

    // Perform TLS handshake
    if (BIO_do_handshake(bio) <= 0) {
        fprintf(stderr, "Error: Failed TLS handshake.\n");
        return EXIT_FAILURE;
    }

    // Log in to the IMAP server
    char login_command[MAX_BUFFER_SIZE];
    sprintf(login_command, "A01 LOGIN %s {%lu}\r\n%s\r\n", username, strlen(password), password);
    char login_response[MAX_BUFFER_SIZE];
    char total_login[102400];
    BIO_puts(bio, login_command);

    // Use select to check if data is available to be read

    receive_with_timeout(bio, total_login, login_response, 10240);
    
    //send_receive_ssl(bio, login_command, login_response, sizeof(login_response));
    


    

    printf("login response:\n%s\n", total_login);


    

    // Select the INBOX folder
    char select_command[] = "A02 SELECT INBOX\r\n";
    char select_response[MAX_BUFFER_SIZE];
    char total_select[102400];
    BIO_puts(bio, select_command);
    receive_with_timeout(bio, total_select, select_response, 10240);

    printf("Select Response: %s\n", select_response);

    // Fetch the first email
    char fetch_command[] = "A03 FETCH 1 BODY.PEEK[]\r\n";
    char fetch_response[MAX_BUFFER_SIZE];
    char total_fetch[102400];
    BIO_puts(bio, fetch_command);
    receive_with_timeout(bio, total_fetch, fetch_response, 10240);
    printf("Email body: %s\n", total_fetch);
    
    // // BIO_read does not always read the full response so I have to call it multiple times. Also, since SSL BIO is blocking, 
    // // if there is no new data to be read, BIO_read will not actually return and will just keep waiting for a response. 
    // // Therefore, I will just keep sending fetch commands so there will always be something to read.
    // // The amount of times that I need to call send_receive_ssl is arbitrary but it just needs to be long enough to not miss out on any details.
    // for (int i = 0; i < 20; i++) {
    //     //BIO_read(bio, fetch_response, 10240);
    //     send_receive_ssl(bio, fetch_command, fetch_response, 10240);
    //     strcat(combined, fetch_response);
    // }
    // //printf("combined:\n%s\n", combined);

    // // This code is for manipulating the strings to extract the email body, starting at "From:"
    // // I noticed that the email contents began and ended at the parts with double dashes (--) followed by random numbers and letters
    // char* from_pos;
    // from_pos = strstr(combined, "From");
    // char* first_dash = strstr(from_pos, "--");
    // char* second_dash = strstr(first_dash + 2, "--");
    // size_t body_length = second_dash - from_pos - 2;
    // char* email_body = malloc(body_length + 1);
    // strncpy(email_body, from_pos, body_length);
    // email_body[body_length] = '\0';

    // // Display the email body
    // printf("Email Body:\n%s\n", email_body);

    
    

    // Clean up
    BIO_free_all(bio);
    SSL_CTX_free(ssl_ctx);

    return EXIT_SUCCESS;
}
