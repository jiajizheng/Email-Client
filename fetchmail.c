#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

#define MAX_BUFFER_SIZE 10240



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

    const char *hostname = argv[1];
    const char *username = "tzeng0088@gmail.com";
    const char *password = "vhkycyvvnoxcoejj";

    SSL_library_init();
    SSL_load_error_strings();

    // Create SSL context and BIO
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    BIO *bio = BIO_new_ssl_connect(ssl_ctx);

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
    send_receive_ssl(bio, login_command, login_response, sizeof(login_response));
    printf("Login Response: %s\n", login_response);


    // // Check if login was successful
    // if (strncmp(login_response, "A01 OK", 6) != 0) {
    //     fprintf(stderr, "Error: Login failed.\n");
    //     return EXIT_FAILURE;
    // }

    // Select the INBOX folder
    char select_command[] = "A02 SELECT INBOX\r\n";
    char select_response[MAX_BUFFER_SIZE];
    send_receive_ssl(bio, select_command, select_response, sizeof(select_response));
    printf("Select Response: %s\n", select_response);

    // // Check if folder selection was successful
    // if (strncmp(select_response, "A02 OK", 6) != 0) {
    //     fprintf(stderr, "Error: Failed to select the INBOX folder.\n");
    //     return EXIT_FAILURE;
    // }

    // Fetch the first email
    char fetch_command[] = "A03 FETCH 1 BODY.PEEK[]\r\n";
    char fetch_response[MAX_BUFFER_SIZE];
    send_receive_ssl(bio, fetch_command, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    BIO_read(bio, fetch_response, sizeof(fetch_response));
    // Display the email body
    printf("Email Body:\n%s\n", fetch_response);

    // Clean up
    BIO_free_all(bio);
    SSL_CTX_free(ssl_ctx);

    return EXIT_SUCCESS;
}
