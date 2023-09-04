#pragma once

#include "Main.hpp"


class Password {
    private:
        std::string _hashedPassword;
    public:
        Password(std::string password);
        virtual ~Password();
        std::string getHashedPassword() const;

        std::string hashPassword(std::string password);
        bool checkPassword(std::string providedPassword, std::string _hashedPassword);

        class WrongPasswordException : public std::exception {
            public:
                virtual const char* what() const throw();
        };
};