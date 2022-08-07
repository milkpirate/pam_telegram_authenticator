# pragma once

#include <string>

#include <security/pam_modules.h>
#include <spdlog/logger.h>

namespace PamWrapper {
    class PamWrapper {
        public:
            PamWrapper(pam_handle_t *pamHandle);

            std::string getRHost();

            std::string getUser();

            std::string getService();

            template<typename... T>
            std::string readInput(T &&... args);

            template<typename... T>
            std::string readPassword(T &&... args);

            template<typename... T>
            void infoMsg(T &&... args);

            template<typename... T>
            void errorMsg(T &&... args);

            template<typename... T>
            void debug(T &&... args);

            template<typename... T>
            void info(T &&... args);

            template<typename... T>
            void notice(T &&... args);

            template<typename... T>
            void warn(T &&... args);

            template<typename... T>
            void error(T &&... args);
            //        void debug(const std::string& msg);
            //        void info(const std::string& msg);
            //        void notice(const std::string& msg);
            //        void warn(const std::string& msg);
            //        void error(const std::string& msg);

        private:
            template<typename... T>
            void _log(int priority, fmt::format_string<T...> fmt, T &&... args);

            std::string _getItem(const int &item);

            pam_handle_t *_pamHandle;
        };
}
