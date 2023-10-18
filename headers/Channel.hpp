#pragma once

#include "Main.hpp"

class Client;

class Channel {
    private:
		std::string _name;
		std::map<std::string, Client *> _clients;
		//Password _password;
		std::string _password;
		std::string _mode;

		std::string _topic;
		std::string _creator;
		std::set<std::string> op_clients; //KICK, INVITE, MODE(i : invite only, t: topic, k: password, o: give/take op, l: client limit)
		std::set<std::string> invited_clients;

		bool invite_only;
		bool restrict_topic;
		bool has_password;
		bool has_clientlimit;

		size_t client_limit;
		size_t client_count;
	public:
		//Channel(std::string name, Client &creator, Password &password);
		Channel(std::string name, Client &creator, std::string password);
		virtual ~Channel(void);

		void 				setName(std::string name);
		void 				setTopic(std::string topic);
		void 				setCreator(std::string creator);
		void 				setModes(std::string password);
		void				setClientLimit(size_t limit);
		std::string const	&getName(void) const;
		std::string const	&getTopic(void) const;
		std::string const	&getCreator(void) const;
		std::string const	&getModes(void) const;
		std::map<std::string, Client *> &getClients(void);
		size_t				getClientLimit(void) const;

		void 				addMode(std::string mode, std::string arg);
		void 				removeMode(std::string mode);
		int 				addClient(Client &client);
		void 				removeClient(Client &client);
		void 				addOp(Client &client);
		void 				removeOp(Client &client);
		void				addInvited(std::string username);

		bool 				isInviteOnly(void) const;
		bool 				isRestrictedTopic(void) const;
		bool 				hasPassword(void) const;
		bool 				hasClientLimit(void) const;
		bool 				isOp(Client &client) const;
		bool 				isInvited(Client &client) const;
		bool 				isClientInChannel(Client &client) const;
		bool 				isFull(void) const;
};

std::ostream&	operator<<(std::ostream& os, Channel& channel);