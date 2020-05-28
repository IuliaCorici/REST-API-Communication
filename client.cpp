// Copyrights 2020, Corici Iulia-Stefania 324CA

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <string>
#include <regex>

// Checks if there is a single word or multiple words
std::string check_if_single_word(std::string words) {
	std::vector<std::string> buffered_words;
	std::istringstream file_virtual(words);
	for (std::string s; file_virtual >> s; ) {
		buffered_words.push_back(s);
	}

	if (buffered_words.size() == 1)
		return "OK";
	return "Just a single word!";
}

// Creates the JSON object using the input data and stores it into a string
std::string get_username_password() {
	std::string username;
	std::string password;
	std::cout << "username=";
	std::getline(std::cin, username);
	std::string single_username = check_if_single_word(username);
	if (single_username == "OK") {
		std::cout << "password=";
		std::getline(std::cin, password);
		std::string single_password = check_if_single_word(password);
		if (single_password == "OK") {
			// Creates the objects using the library functions.
			JSON_Value *root_value = json_value_init_object();
			JSON_Object *root_object = json_value_get_object(root_value);
			char *serialized_string = NULL;
			// Sets the fields using the input data.
			json_object_set_string(root_object, "username", username.c_str());
			json_object_set_string(root_object, "password", password.c_str());
			serialized_string = json_serialize_to_string_pretty(root_value);
			return std::string(serialized_string);
		} else {
			return single_password;
		}
	} else {
		return single_username;
	}
}

// Checks if it's an integer.
bool is_integer(const std::string &s){
    return std::regex_match(s, std::regex("-?[0-9]+"));
}

// Checks if it's a negative integer.
bool is_negative_integer(const std::string &s){
    return std::regex_match(s, std::regex("-[0-9]+"));
}

// Checks if it's a positive integer, a negative one or a string.
std::string check_integer(std::string page_count_string) {
	if (is_integer(page_count_string)) {
		if (is_negative_integer(page_count_string)) {
			return "Page count cannot be negative.";
		} else {
			return "OK";
		}
	} else {
		return "Page count cannot be a string.";
	}
}

// Parses the response and gets the error.
std::string get_error(char *response_to_be_parsed) {
	std::size_t pos = std::string(response_to_be_parsed).find("error\":\"");
	// Looks for keyword "error".
	if (pos == std::string::npos) {
		return "no_error";
	}
  	std::string str3 = std::string(response_to_be_parsed).substr(pos);
	str3 = str3.substr(8);
	pos = std::string(str3).find("\"}");
	// Substring that contains the content of the JSON with the error.
	std::string token = str3.substr(0, pos);

	return token;
}

// Parses the response and gets the token.
std::string get_token(char *token_to_be_parsed) {
	std::size_t pos = std::string(token_to_be_parsed).find("token\":\"");
	// Looks for keyword "token".
	if (pos == std::string::npos) {
		return "no_token";
	}
  	std::string str3 = std::string(token_to_be_parsed).substr(pos);
	str3 = str3.substr(8);
	pos = std::string(str3).find("\"}");
	// Substring that contains the content of the JSON with the token.
	std::string token = str3.substr(0, pos);

	return token;
}

// Checks if the operation is executed without error, by checking the
// length of the content, because it should not be any content if success.
bool successful_operation(std::string cookie_to_be_parsed) {
	// Parsing the response line by line to detect the part
	// including the content length
	std::istringstream file_virtual(cookie_to_be_parsed);
	std::string str, cookie_to; 
	while (std::getline(file_virtual, str)) {
		std::size_t found = str.find("Content-Length: 0");
		if (found != std::string::npos) {
			return true;
		}
	}

	return false;
}
// Gets the content of the response.
std::string get_content(std::string response) {

	std::size_t found = response.find("Connection: keep-alive");
	// Creates a substring only including the list of JSON objects.
	std::string str3 = response.substr(found);
	
	return str3.substr(24);
}
// Checks if the connection is closed.
std::string check_connection_closed(std::string response) {
	std::istringstream file_virtual(response);
	std::string str, cookie_to, str1;

	while (std::getline(file_virtual, str)) {
		std::string str1 = str;
		std::size_t found = str.find("Connection: close");
		if (found != std::string::npos) {
			return "You are not logged in!";
		}
	}
	
	return "OK";
}

// Gets the cookie from the response.
std::string get_cookie(std::string cookie_to_be_parsed) {
	// Parsing the response line by line to make sure
	// only the cookie line is returned.
	std::istringstream file_virtual(cookie_to_be_parsed);
	std::string str, cookie_to;

	while (std::getline(file_virtual, str)) {
		std::size_t found = str.find("Set-Cookie");
		if (found != std::string::npos) {
			std::size_t pos = str.find("connect");
			std::string str3 = str.substr(pos);
			pos = str3.find(";");
			// Substring with the cookie-content.
			str3 = str3.substr(0, pos);
			cookie_to = str3;
			break;
		}
	}

	return cookie_to;
}
// Prints errors or "SUCCESS" message.
void print_output(char* response) {
	if (!successful_operation(std::string(response))) {
		puts(get_error(response).c_str());
	} else {
		puts("SUCCESS");
	}
}
// Prints the content or the errors.
void print_output_with_content(char* response) {
	char *copy_response = strdup(response);
	std::string response_to = std::string(response);
	std::string is_connected = check_connection_closed(response_to);
	if (is_connected == "OK") {
		// The user is connected but there might be errors.
		std::string check_error = get_error(response);
		if (check_error == "no_error") {
			puts(get_content(std::string(copy_response)).c_str());
		} else {
			puts(check_error.c_str());
		}
	} else {
		puts(is_connected.c_str());
	}
}
// Creates a JSON object from the input data and returns the string version of it. 
char* get_added_book(std::string title, std::string author, std::string publisher,
					std::string genre, int page_count) {
	// Creates the objects using the library functions.
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	char *serialized_string = NULL;
	// Sets the fields using the input data.			
	json_object_set_string(root_object, "title", title.c_str());
	json_object_set_string(root_object, "author", author.c_str());
	json_object_set_string(root_object, "genre", genre.c_str());
	json_object_set_number(root_object, "page_count", page_count);
	json_object_set_string(root_object, "publisher", publisher.c_str());
	serialized_string = json_serialize_to_string_pretty(root_value);

	return serialized_string;
}

int main(int argc, char *argv[]) {
    char *message;
    char *response;
    int sockfd;
	char cookie[BUFLEN];
	char token[BUFLEN];
	memset(token, 0, BUFLEN);
	memset(cookie, 0, BUFLEN);
	// Hard-coding the host.
    char* host = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
    // Opens the connection.
    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

    while (1) {
    	
		std::string command;
		std::getline(std::cin, command);

		if (command == "exit") {
				break;

		} else if (command == "register") {
			// For the register command.
			std::string serialized_string = get_username_password();
			// Checks if the username and the password are single words.
			if (std::string(serialized_string) != "Just a single word!") {
				// Proceeds the post request.
				message = compute_post_request(host, "/api/v1/tema/auth/register", 
						NULL, (char*) serialized_string.c_str(), 2, NULL, 0, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				// Reassures that the connection is not closed, and gets a response.
				if (strlen(response) == 0) {
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
				}

				print_output(response);
			} else {
				puts(serialized_string.c_str());
			}
			puts("");

		} else if (command == "login") {
			// For the login command.
			std::string serialized_string = get_username_password();
			// Checks if the username and the password are single words.
			if (serialized_string != "Just a single word!") {
				// Proceeds the post request.
				message = compute_post_request(host, "/api/v1/tema/auth/login", NULL,
						(char*) serialized_string.c_str(), 2, NULL, 0, NULL);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				// Reassures that the connection is not closed, and gets a response.
				if (strlen(response) == 0) {
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
				}
				// Checks if there is any error.
				if (!successful_operation(std::string(response))) {
					puts(get_error(response).c_str());
				} else {
					puts("SUCCESS");
					std::string response_parse(response);
					std::string cookie_to = get_cookie(response_parse);
					memset(cookie, 0, BUFLEN);
					memcpy(cookie, cookie_to.c_str(), cookie_to.size());
				}
			} else {
				puts(serialized_string.c_str());
			}
			puts("");
			
		} else if (command == "enter_library") {
			// For the enter_library command.
			// Proceeds the get request.
			message = compute_get_request(host, "/api/v1/tema/library/access",
					  NULL, (char*) cookie, 1, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
			// Reassures that the connection is not closed, and gets a response.
			if (strlen(response) == 0) {
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
			}
			// Gets the token.
			std::string token_to = get_token(response);
			if (get_token(response) != "no_token") {
				puts("SUCCESS");
			    memset(token, 0, BUFLEN);
			    memcpy(token, token_to.c_str(), token_to.size());
			} else {
				puts(get_error(response).c_str());
			}
			puts("");

			
		} else if (command == "get_books") {
			// For the get_request command.
			// Proceeds the get request.
			message = compute_get_request(host, "/api/v1/tema/library/books",
					  NULL, (char*) cookie, 1, token);
			send_to_server(sockfd, message);
			response = receive_from_server(sockfd);
			// Reassures that the connection is not closed, and gets a response.
			if (strlen(response) == 0) {
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
			}

			print_output_with_content(response);
			puts("");

		} else if (command == "get_book") {
			// For the get_book command.
			std::string id_string;
			std::cout << "id=";
            std::getline(std::cin, id_string);
			std::string maybe_integer = check_integer(id_string);
			if (maybe_integer == "OK") {
				// Proceeds the get request.
				message = compute_get_request(host, "/api/v1/tema/library/books/",
						  (char*) id_string.c_str(), (char*) cookie, 1, token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				// Reassures that the connection is not closed, and gets a response.
				if (strlen(response) == 0) {
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
				}
				
				print_output_with_content(response);
			} else {
				puts(maybe_integer.c_str());
			}
			puts("");

		} else if (command == "add_book") {
		
			// For the add_book command.
			std::string title;
			std::string author;
			std::string publisher;
			std::string genre;
			std::string page_count_string;
			
            std::cout << "title=";
			std::getline(std::cin, title);
			std::cout << "author=";
			std::getline(std::cin, author);
			std::cout << "genre=";
			std::getline(std::cin, genre);
			std::cout << "publisher=";
			std::getline(std::cin, publisher);
			std::cout << "page_count=";
			std::getline(std::cin, page_count_string);
			
			std::string maybe_integer = check_integer(page_count_string);
			if (maybe_integer == "OK") {
			
				int page_count = std::stoi(page_count_string);
				char *serialized_string = get_added_book(title, author, publisher,
											genre, page_count);
            	// Proceeds the post request.
				message = compute_post_request(host, "/api/v1/tema/library/books",
						  NULL, serialized_string, 2, (char*) cookie, 0, token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				// Reassures that the connection is not closed, and gets a response.
				if (strlen(response) == 0) {
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
				}
				print_output(response);

			} else {
				puts(maybe_integer.c_str());
			}
			puts("");

		} else if (command == "delete_book") {
			// For the delete_book command.
			std::string id_string;
			std::cout << "id=";
            std::getline(std::cin, id_string);

			std::string maybe_integer = check_integer(id_string);
			if (maybe_integer == "OK") {
				// Proceeds the selete request.
				message = compute_delete_request(host, "/api/v1/tema/library/books/",
					      (char*) id_string.c_str(), (char*) cookie, 1, token);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
				// Reassures that the connection is not closed, and gets a response.
				if (strlen(response) == 0) {
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
					send_to_server(sockfd, message);
					response = receive_from_server(sockfd);
				}
				print_output(response);

			} else {
				puts(maybe_integer.c_str());
			}
			puts("");

		} else if (command == "logout") {
			// For the logout command.
			// Proceeds the get request.
			message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL,
			 		 (char*) cookie, 1, NULL);
    		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
			// Reassures that the connection is not closed, and gets a response.
			if (strlen(response) == 0) {
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
				send_to_server(sockfd, message);
				response = receive_from_server(sockfd);
			}
			// Checks for errors.
			if (!successful_operation(std::string(response))) {
				puts(get_error(response).c_str());
			} else {
				puts("SUCCESS");
				memset(cookie, 0, BUFLEN);
				memset(token, 0 , BUFLEN);
			}
			puts("");

		} else {
			// Error for any other command.
			puts("INVALID COMMAND!");
			puts("");
		}
    }
    return 0;
}