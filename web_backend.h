#ifndef WEB_BACKEND_H_INCLUDED
#define WEB_BACKEND_H_INCLUDED

#include <string>
#include <unistd.h>  ///_SC_NPROCESSORS_ONLN
#include <limits.h>  ///PATH_MAX
#include <cstring>
#include <vector>
#include <fcgiapp.h>

class web_backend
{
    public:
    ///PATH_MAX уже содержит терминальный нуль
    std::string path; ///полный путь
    char *ip_addr="127.0.0.1:9000";
    std::string html_relative_path{"/html"};
    std::vector<char*> files;

    web_backend()
    {
        char buf[PATH_MAX]; ///PATH_MAX уже содержит терминальный нуль
        get_full_path("bin", buf);
        path.append(buf);
        path=path+html_relative_path;
        files=get_all_string_from_path((char*)path.c_str());
        create_fastcgi_threads(ip_addr);
    }
    ~web_backend()
    {
        for(char *str: files)
        {
            free(str);
        }
    }
    private:
    int THREAD_COUNT=sysconf(_SC_NPROCESSORS_ONLN);
    void get_full_path(char *relatve_path, char *absolute_path);
    int create_fastcgi_threads(char *ip_addr);
    std::vector<char*> get_all_string_from_path(char* html_path);

    static std::string urlencode(const std::string &s);
    static std::string urlDecode(std::string &SRC);
    static int threadFunction(int socketId, char* html_path, std::vector<char*> *files);
    static void generate_pages_from_uri(FCGX_Request *request, char *uri, char* html_path, std::vector<char*> *files);
    static int add_all_path_for_send(FCGX_Request *request, char *uri, char *html_path, std::vector<char*> *files);
};

#endif // WEB_BACKEND_H_INCLUDED
