#pragma once

#include <string>

class Channel {
    private:
        bool _isInviteOnly;
        bool _hasPassword;
        bool _hasUsersLimit;

        size_t _userLimit;
    public:
        Channel(std::string name, std::string password);
        virtual ~Channel(void);
};