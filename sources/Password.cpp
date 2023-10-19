#include <Password.hpp>

Password::Password(std::string password) {
	this->_hashedPassword = hashPassword(password);
}

Password::~Password() {}

std::string Password::getHashedPassword() const {
    return _hashedPassword;
}

std::string Password::hashPassword(std::string password) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned const char* data = reinterpret_cast<const unsigned char*>(password.c_str());
	SHA256(data, password.size(), hash);

	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return (ss.str());
}

bool Password::checkPassword(std::string providedPassword, std::string _hashedPassword) {
    std::string pw_hash = hashPassword(providedPassword);
    if (pw_hash != _hashedPassword)
        return false;
    return true;
}

const char *Password::WrongPasswordException::what() const throw() {
    std::cerr << "Wrong password" << std::endl;
}