#include "JwtVerifier.hpp"
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include "utils/Logger.hpp"

JwtVerifier::JwtVerifier(const std::string& secret) : _secret(secret) {}

bool JwtVerifier::verify(const std::string& token, JwtClaims& outClaims) {
	try {
		auto decoded = jwt::decode(token);

		auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256{ _secret});

		verifier.verify(decoded);

		//outClaims.userId = decoded.get_payload_claim("sub").as_integer();

		std::string subStr = decoded.get_payload_claim("sub").as_string();
		outClaims.userId = std::stoll(subStr);

		return true;
	}
	catch (const std::exception& e) {
		LOG_ERROR("JWT Verification Failed : {}", e.what());
		return false;
	}
}