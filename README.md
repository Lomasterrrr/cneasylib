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

## GET (HTTP, HTTPS)
``` C
// Отправить HTTP/GET запрос и записать ответ в переменную
int http_get(const char *host, const char *port, const char *path, char* response);

// Отправить HTTPs/GET запрос и записать ответ в переменную
int https_get(const char *host, const char *port, const char *path, char *response);

// Получить время ответа сайта, с помощью GET запроса
double get_get_response_time(const char* host, int http_or_https);
```

## POST
``` C
// Отправить POST запрос, и записать ответ в переменную
int post(const char* host, int port, char* path, char* content_type, char* request_body, char* response);

// Отправить тестовый POST запрос, и вывести на экран
int debug_post(const char* host);
```

## CODE
``` C
// Вернуть код ответа сайта
int get_responce_code(const char* host, int port, const char* path);
```
