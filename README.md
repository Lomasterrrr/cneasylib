# Cneasylib
Набор уже готовых функция для работы с сетью на си.
- Не требует никаких сторонних библиотек
- Кросплотформленая
- Можно использывать в любом проекте просто подключих заголовок
- Доступны примеры

Пока всего бета.

## IP (IPv4, IPv6)
``` C
// Получает первый ip домена
char* get_ip_address(const char *host, const char* port, int use_cache);

// Получает все ip домена
char** get_ip_addresses(const char* host, const char *port, int use_cache);

// Тестовый вывод функции get_ip_address
int debug_print_ip_address(char *ip, int time);

// Тестовый вывод функции get_ip_addresses
int debug_print_ip_addresses(char **ips, int time);

// Чистит память после использывания get_ip_addresses
void free_ips(char **ips);
```

``` C
const char* ip_temp = get_ip_address("google.com", "80", 0);

if (ip_temp != NULL)
{
    printf("IP is: %s\n", ip_temp);
}
```

``` C
char **ips = get_ip_addresses("google.com", "80", 0);
char **temp;

if (ips == NULL)
{
    fprintf(stderr, "error: getting IP addresses");
}

for (temp = ips; *temp != NULL; temp++)
{
    printf("%s\n", *temp);
} 

free_ips(ips);
```


## GET (HTTP, HTTPS)
``` C
// Отправить HTTP/GET запрос и записать ответ в переменную
int http_get(const char *host, const char *port, const char *path, char* response);

// Отправить HTTPs/GET запрос и записать ответ в переменную
int https_get(const char *host, const char *port, const char *path, char *response);

// Получить время ответа сайта, с помощью GET запроса
double get_get_response_time(const char* host, int http_or_https);
```

``` C
char responce[MAXBUFLEN];
if (http_get("google.com", "80", "/", responce) != EOF)
{
    printf("%s\n",responce);
}
```

``` C
char responce[MAXBUFLEN];
if (https_get("google.com", "443", "/", responce) != EOF)
{
    printf("%s\n",responce);
}
```

## POST
``` C
// Отправить POST запрос, и записать ответ в переменную
int post(const char* host, int port, char* path, char* content_type, char* request_body, char* response);

// Отправить тестовый POST запрос, и вывести на экран
int debug_post(const char* host);
```

``` C
const char* host = "google.com";
int port = 80;

char* path = "/api";
char* content_type = "";
char* request_body = "test";

char responce[BUFFER_SIZE];

post(host, port, content_type, request_body, request_body, responce);
printf("%s", responce);
```

## CODE
``` C
// Вернуть код ответа сайта
int get_responce_code(const char* host, int port, const char* path);
```

``` C
int code = get_responce_code("google.com", 80, "/");
if (code != EOF)
{
    printf("Responce code is: %d", code);
} 
```

## Компиляция
```
gcc -c <filename> -lssl -lcrypto
gcc -o <program_name> <filename.o> -lssl -lcrypto
```

