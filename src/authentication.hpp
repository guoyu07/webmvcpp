#ifndef WEBMVCPP_AUTHENTICATION_H
#define WEBMVCPP_AUTHENTICATION_H

namespace webmvcpp
{
    struct basic_authentication {
        std::string get_realm(const std::string &user, const std::string &password) 
        {
            std::string loginAndPassword = user + ":" + password;
            return utils::base64_encode((unsigned char *)loginAndPassword.c_str(), loginAndPassword.length());
        }

        bool get_username(const std::string &realm, std::string &userName)
        {
            std::string loginAndPassword = utils::base64_decode(realm);
            std::vector<std::string> splittedRealm = utils::split_string(loginAndPassword, ':');
            if (splittedRealm.size() != 2)
                return false;

            std::string u = splittedRealm[0];
            if (u.length() == 0)
                return false;

            userName = u;

            return true;
        }
    };

    struct digest_authentication {

        std::string calc_digest(const std::string &auth, const std::string &user, const std::string &realm, const std::string &pass, const std::string &method, const std::string &uri)
        {
            const char *nonce = "06a1a23153d48e59f", *cnonce = "1z2c2v7w9v", *qop = "auth";

            std::string ha = MD5::calc(method + ":" + uri);
            std::string ha1 = MD5::calc(user + ":" + realm + ":" + pass);
            std::string result = MD5::calc(ha1 + ":" + nonce + ":" + "00000001" + ":" + cnonce + ":" + qop + ":" + ha);

            return result;
        }

    };
};

#endif // WEBMVCPP_AUTHENTICATION_H
