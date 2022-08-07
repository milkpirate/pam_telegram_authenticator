#include "PamWrapper.hpp"

#include <security/pam_ext.h>
#include <syslog.h>

namespace PamWrapper {
    PamWrapper::PamWrapper(pam_handle_t *pamHandle) :
        _pamHandle(pamHandle)
    {
        pam_info(_pamHandle, "constr");
    }

    template<typename... T>
    std::string PamWrapper::readInput(T &&... args) {
        char *str;
        pam_prompt(_pamHandle, PAM_PROMPT_ECHO_ON, &str, fmt::format(std::forward<T>(args)...).c_str());
        return std::string(static_cast<const char *>(str));
    }

    template<typename... T>
    std::string PamWrapper::readPassword(T &&... args) {
        char *str;
        pam_prompt(_pamHandle, PAM_PROMPT_ECHO_OFF, &str, fmt::format(std::forward<T>(args)...).c_str());
        return std::string(static_cast<const char *>(str));
    }

    template<typename... T>
    void PamWrapper::infoMsg(T &&... args) {
        pam_info(_pamHandle, fmt::format(std::forward<T>(args)...).c_str());
    }

    template<typename... T>
    void PamWrapper::errorMsg(T &&... args) {
        pam_error(_pamHandle, fmt::format(std::forward<T>(args)...).c_str());
    }

    std::string PamWrapper::_getItem(const int &item) {
        const void *str;
        pam_get_item(_pamHandle, item, &str);
        return std::string(static_cast<const char *>(str));
    }

    std::string PamWrapper::getUser() {
        return _getItem(PAM_USER);
    }

    std::string PamWrapper::getService() {
        return _getItem(PAM_SERVICE);
    }

    std::string PamWrapper::getRHost() {
        return _getItem(PAM_RHOST);
    }

    template<typename... T>
    void PamWrapper::_log(int priority, fmt::format_string<T...> fmt, T &&... args) {
        pam_syslog(_pamHandle, priority, fmt::format(fmt, std::forward<T>(args)...));
    }

    //void PamWrapper::_log(int priority, const char* msg) {
    //    pam_syslog(_pamHandle, priority, msg);
    //}

    //template <typename... T>
    //FMT_NODISCARD FMT_INLINE auto format(format_string<T...> fmt, T&&... args) -> std::string {
    //    return vformat(fmt, fmt::make_format_args(args...));
    //}

    //void func(T t, Args... args) // recursive variadic function
    //{
    //    std::cout << t <<std::endl ;
    //
    //    func(args...) ;
    //}

    template<typename... T>
    void PamWrapper::debug(T &&... args) {
        _log(LOG_DEBUG, std::forward<T>(args)...);
    }

    template<typename... T>
    void PamWrapper::info(T &&... args) {
        _log(LOG_INFO, std::forward<T>(args)...);
    }

    template<typename... T>
    void PamWrapper::notice(T &&... args) {
        _log(LOG_NOTICE, std::forward<T>(args)...);
    }

    template<typename... T>
    void PamWrapper::warn(T &&... args) {
        _log(LOG_WARNING, std::forward<T>(args)...);
    }

    template<typename... T>
    void PamWrapper::error(T &&... args) {
        _log(LOG_ERR, std::forward<T>(args)...);
    }
}