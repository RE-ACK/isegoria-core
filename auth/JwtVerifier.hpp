#pragma once

#include <string>

struct JwtClaims {
	int64_t userId;
};

class JwtVerifier {
public:
	explicit JwtVerifier(const std::string& secret);
	bool verify(const std::string& token, JwtClaims& outClaims);

private:
	std::string _secret;
};