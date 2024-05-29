#define WIN32_LEAN_AND_MEAN // Оптимизация Windows.h, исключение ненужных структур и определений

#include <iostream> // Стандартная библиотека для ввода/вывода
#include <Windows.h> // Библиотека Windows API
#include <WinSock2.h> // Библиотека Windows Sockets
#include <WS2tcpip.h> // Библиотека для работы с адресами и сокетами

using namespace std; // Использование имен из пространства имен std

int main() // Главная функция программы
{
    SetConsoleCP(1251); // Устанавливает кодировку консоли на кириллицу
    setlocale(LC_ALL, "Russian"); // Устанавливает локаль на русский язык

    WSADATA wsaData; // Создает структуру данных для инициализации WinSock
    ADDRINFO hints; // Создает структуру данных для хранения информации об адресе
    ADDRINFO* addrResult; // Создает указатель на структуру данных для хранения результатов поиска адреса
    SOCKET ClientSocket = INVALID_SOCKET; // Создает сокет клиента со значением INVALID_SOCKET

    const char* sendBuffer = "Hello from Client"; // Создает буфер данных для отправки
    char recvBuffer[512]; // Создает буфер данных для приема

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализирует WinSock версии 2.2
    if (result != 0) { // Если инициализация не удалась
        cout << "WSAStartup failed" << endl; // Выводит сообщение об ошибке
        return 1; // Возвращает код ошибки
    }

    ZeroMemory(&hints, sizeof(hints)); // Заполняет структуру данных нулями
    hints.ai_family = AF_INET; // Устанавливает тип адреса на IPv4
    hints.ai_socktype = SOCK_STREAM; // Устанавливает тип сокета на поток данных
    hints.ai_protocol = IPPROTO_TCP; // Устанавливает протокол сокета на TCP

    result = getaddrinfo("localhost", "788", &hints, &addrResult); // Получает информацию об адресе сервера
    if (result != 0) { // Если поиск адреса не удался
        cout << "getaddrinfo failed" << endl; // Выводит сообщение об ошибке
        return 1; // Возвращает код ошибки
    }

    ClientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создает сокет клиента
    if (ClientSocket == INVALID_SOCKET) { // Если создание сокета не удалось
        cout << "socket creation with error" << endl; // Выводит сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождает память, занимаемую структурой данных
        return 1; // Возвращает код ошибки
    }

    result = connect(ClientSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Устанавливает соединение с сервером
    if (result == SOCKET_ERROR) { // Если соединение не удалось
        cout << "Unable to connect server" << endl; // Выводит сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождает память
        WSACleanup(); // Очищает ресурсы WinSock
        return 1; // Возвращает код ошибки
    }

    result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), MSG_DONTROUTE); // Отправляет данные на сервер
    if (result == SOCKET_ERROR) { // Если отправка данных не удалась
        cout << "send Failed" << endl; // Выводит сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождает память
        WSACleanup(); // Очищает ресурсы WinSock
        return 1; // Возвращает код ошибки
    }

    cout << "Sent " << result << " bytes" << endl; // Выводит количество отправленных байт

    const char* secondSendBuffer = "Second message from Client"; // Создает буфер для второго сообщения
    result = send(ClientSocket, secondSendBuffer, (int)strlen(secondSendBuffer), MSG_DONTROUTE); // Отправляет второе сообщение
    if (result == SOCKET_ERROR) { // Если отправка не удалась
        cout << "send Failed" << endl; // Выводит сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождает память
        WSACleanup(); // Очищает ресурсы WinSock
        return 1; // Возвращает код ошибки
    }
    cout << "Sent " << result << " bytes" << endl; // Выводит количество отправленных байт

    result = shutdown(ClientSocket, SD_SEND); // Закрывает отправку данных на сокете
    if (result == SOCKET_ERROR) { // Если закрытие не удалось
        cout << "shutdown error" << endl; // Выводит сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождает память
        WSACleanup(); // Очищает ресурсы WinSock
        return 1; // Возвращает код ошибки
    }

    do { // Цикл для получения данных с сокета
        ZeroMemory(recvBuffer, 512); // Обнуляет буфер для приема данных
        result = recv(ClientSocket, recvBuffer, 512, 0); // Получает данные с сокета
        if (result > 0) { // Если данные получены
            cout << "Получено " << result << " байт" << endl; // Выводит количество полученных байт
            cout << "Получено: " << recvBuffer << endl; // Выводит полученные данные
        }
        else if (result == 0) { // Если соединение закрыто
            cout << "Connection closed" << endl; // Выводит сообщение о закрытии
        }
        else { // Если произошла ошибка при получении данных
            cout << "Сообщение не получено" << endl; // Выводит сообщение об ошибке
        }
    } while (result > 0); // Продолжает цикл, пока данные поступают

    closesocket(ClientSocket); // Закрывает сокет
    freeaddrinfo(addrResult); // Освобождает память, занимаемую структурой данных
    WSACleanup(); // Очищает ресурсы WinSock
    return 0; // Возвращает код успешного завершения
}