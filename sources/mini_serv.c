// given
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#define BUFFER_SIZE 4096

typedef struct s_client
{
	int sockfd;
	char *msg;
	int id;
} t_client;

// given
int extract_message(char **buf, char **msg)
{
	char *newbuf;
	int i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

// given
char *str_join(char *buf, char *add)
{
	char *newbuf;
	int len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

int main(int argc, char **argv)
{
	//given
	struct sockaddr_in address;
	int sockfd;

	int nbclients = 0;
	fd_set readfds;
	fd_set writefds;
	t_client clients[100];
	socklen_t addrlen;
	int maxsockfd = 0;

	int id = 0;
	if (argc == 1)
	{
		printf("Wrong number of arguments\n");	
		exit(1);
	}

	int port = atoi(argv[1]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Fatal error\n");
		exit(1);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	address.sin_port = htons(port);

	addrlen = sizeof(address);

	if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Fatal error\n");
		close(sockfd);
		exit(1);
	}

	maxsockfd = sockfd;

	//given
	if (listen(sockfd, 10) < 0)
	{
		perror("Fatal error\n");
		exit(1);
	}

	char buffer[BUFFER_SIZE]; // TCP buffer size limits

	while (1)
	{
		// Clear the socket set
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		// Add the socket to the set
		FD_SET(sockfd, &readfds);
		// Add the clients to the set
		for (int i = 0; i < nbclients; i++)
		{
			FD_SET(clients[i].sockfd, &writefds);
			if (clients[i].sockfd > maxsockfd)
				maxsockfd = clients[i].sockfd;
		}

		if (select(maxsockfd + 1, &readfds, &writefds, NULL, NULL) < 0)
		{
			perror("Fatal error\n");
			exit(1);
		}
		// Check if there is a new connection
		if (FD_ISSET(sockfd, &readfds))
		{
			int newsockfd = accept(sockfd, (struct sockaddr *)&address, &addrlen);
			if (newsockfd < 0)
			{
				perror("Fatal error\n");
				exit(1);
			}
			// Add the new client to the list
			fcntl(newsockfd, F_SETFL, O_NONBLOCK);
			clients[nbclients] = (struct s_client){newsockfd, NULL, id++};
			// Warn clients already on the server that a new client arrived
			for (int i = 0; i < nbclients; i++)
			{
				// Send the new client to the other clients
				memset(buffer, 0, strlen(buffer));
				sprintf(buffer, "server: client %d just arrived\n", clients[nbclients].id);
				if (send(clients[i].sockfd, buffer, strlen(buffer), 0) < 0)
				{
					perror("Fatal error\n");
					exit(1);
				}
			}
			nbclients++;
			continue;
		}
		
		for (int i = 0; i < nbclients; i++)
		{
			if (FD_ISSET(clients[i].sockfd, &writefds))
			{
				memset(buffer, 0, sizeof(buffer));
				int ret = recv(clients[i].sockfd, buffer, sizeof(buffer) - 1, 0);
				if (ret == 0)
				{
					close(clients[i].sockfd);
					memset(buffer, 0, BUFFER_SIZE);
					sprintf(buffer, "server: client %d just left\n", clients[i].id);
					for (int j = i; j < nbclients - 1; j++)
					{
						clients[j] = clients[j + 1];
					}
					nbclients--;
					for (int j = 0; j < nbclients; j++)
					{
						if (send(clients[j].sockfd, buffer, strlen(buffer), 0) < 0)
						{
							perror("Fatal error\n");
							exit(1);
						}
					}
				}
				else if (ret > 0)
				{
					clients[i].msg = str_join(clients[i].msg, buffer);
					if (clients[i].msg && strstr(clients[i].msg, "\n") == NULL)
						continue;
					memset(buffer, 0, BUFFER_SIZE);
					char *msg;
					char buffermsg[BUFFER_SIZE + 20];
					while (extract_message(&clients[i].msg, &msg) > 0)
					{
						sprintf(buffermsg, "client %d: %s", clients[i].id, msg);
						for (int j = 0; j < nbclients; j++)
						{
							if (j != i)
							{
								if (send(clients[j].sockfd, buffermsg, strlen(buffermsg), 0) < 0)
								{
									perror("Fatal error\n");
									exit(1);
								}
							}
						}
						free(msg);
						msg = NULL;
					}
					free(clients[i].msg);
					clients[i].msg = NULL;
				}
			}
		}
	}
}