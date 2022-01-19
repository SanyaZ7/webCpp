
#include "web_backend.h"
#include <mutex>
#include <cstring>
#include <fstream>

#include <iostream>
#include <filesystem>


void web_backend::get_full_path(char *relatve_path, char *absolute_path)
{
    char *res = realpath(relatve_path, absolute_path);
    if (res) {
        int len1=strlen(relatve_path);
        int len2=strlen(absolute_path);
        absolute_path[len2-len1-1]=0;
    }
}

int web_backend::threadFunction(int socketId, char* html_path, std::vector<char*> *files)
{
    int rc;
    FCGX_Request request;
    if(FCGX_InitRequest(&request, socketId, 0) != 0)
    {
        //ошибка при инициализации структуры запроса
        fprintf(stderr, "Can not init request\n");
        return 0;
    }
    while(1)
    {
        static std::mutex accept_mutex;
        //попробовать получить новый запрос
        printf("Try to accept new request\n");
        accept_mutex.lock();
        rc = FCGX_Accept_r(&request);
        accept_mutex.unlock();
        if(rc < 0)
        {
            //ошибка при получении запроса
            fprintf(stderr, "Can not accept new request\n"); break;
        }
        if(rc==0)
        {
            //получить значение переменной
            char *uri = FCGX_GetParam("DOCUMENT_URI", request.envp);
            generate_pages_from_uri(&request, uri, html_path, files);
        }
        FCGX_Finish_r(&request);
    }
    return 0;
}

#include <thread>
int web_backend::create_fastcgi_threads(char *ip_addr)
{
    FCGX_Init();
     //открываем новый сокет
    int socketId = FCGX_OpenSocket(ip_addr, 20); ///20 - глубина очереди
     if(socketId < 0)
    {
        fprintf(stderr, "Failed to open socket with FCGX_OpenSocket\n");
        return -1;
    }
    std::thread thr[THREAD_COUNT];
    for(int i=0; i<THREAD_COUNT; ++i)
    {
        thr[i]=std::thread(threadFunction, socketId, (char*) path.c_str(), &files);
    }
    for(int i=0; i<THREAD_COUNT; ++i)
    {
        thr[i].join();
    }
    return 0;
}

int web_backend::add_all_path_for_send(FCGX_Request *request, char *uri, char *html_path, std::vector<char*> *files)
{
    for(char *str: *files)
    {
        printf("file=%s\n", str);
        ///страница по умолчанию
        char *str_temp="/index.html"; ///временный указатель для index.html
        if(!strcmp(uri, "/"))
        {
            goto start;
        }
        str_temp=str;
        if(!strcmp(uri, str_temp))
        {
            start:
            std::string filename{html_path};
            filename+=str_temp;

            std::ifstream istrm(filename, std::ios::binary);
            if (istrm.is_open())
            {
                std::stringstream extension;
                extension<<std::filesystem::path(filename.c_str()).extension();
                if(extension.str()!=".mstch")
                {
                    istrm.seekg (0, istrm.end);
                    std::streampos size = istrm.tellg();
                    istrm.seekg (0, istrm.beg);
                    char *memblock = new char [size];
                    istrm.read (memblock, size);
                    istrm.close();
                    FCGX_PutStr(memblock, size, request->out);
                    delete[] memblock;
                    return 0;
                }
                else
                {
                    ///обработка динамических страниц с шаблонами
                }
            }
            else std::cout << "Unable to open file";
        }
    }
    return 0;
}

void web_backend::generate_pages_from_uri(FCGX_Request *request, char *uri, char* html_path, std::vector<char*> *files)
{
    FCGX_PutS("\n", request->out);
    add_all_path_for_send(request, uri, html_path, files);
    ///здесь может быть функция для обработки post/get запросов
}

std::vector<char*> web_backend::get_all_string_from_path(char* html_path)
{
    std::vector <char*> vc;
    const std::filesystem::path sandbox(html_path);
    namespace fs = std::filesystem;
    if(fs::exists(sandbox))
    {
        for(std::filesystem::directory_entry const& dir_entry: std::filesystem::recursive_directory_iterator{sandbox})
        {
            if(fs::is_regular_file(dir_entry))
            {
                vc.emplace_back(strdup(dir_entry.path().c_str()+strlen(html_path)));
            }
        }
    }
    return vc;
}

std::string web_backend::urlencode(const std::string &s)
{
    static const char lookup[]= "0123456789abcdef";
    std::stringstream e;
    for(int i=0, ix=s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if ( (48 <= c && c <= 57) ||//0-9
             (65 <= c && c <= 90) ||//abc...xyz
             (97 <= c && c <= 122) || //ABC...XYZ
             (c=='-' || c=='_' || c=='.' || c=='~')
        )
        {
            e << c;
        }
        else
        {
            e << '%';
            e << lookup[ (c&0xF0)>>4 ];
            e << lookup[ (c&0x0F) ];
        }
    }
    return e.str();
}

std::string web_backend::urlDecode(std::string &SRC) {
    std::string ret;
    char ch;
    int ii;
    for (unsigned long i=0; i<SRC.length(); i++) {
        if (int(SRC[i])==37) {
            sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else {
            ret+=SRC[i];
        }
    }
    return (ret);
}

