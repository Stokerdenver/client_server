// chat_server2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define WIN32_LEAN_AND_MEAN

#include <iostream>
using namespace std;
#include <windows.h>
#include <winSock2.h>
#include <WS2tcpip.h>

void MyShutDown(SOCKET Socket, ADDRINFO* addrResult)
{
    closesocket(Socket);
    freeaddrinfo(addrResult);
    WSACleanup();
}

int main()
{
    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;

    // клиентский сокет, который мы порождаем при подключении к нам клиента
    SOCKET ClientSocket = INVALID_SOCKET;

    // создаем сокет, слушающий соединение
    SOCKET ListenSocket = INVALID_SOCKET;

    const char* sendBuffer = "Hello from server";

    // буфер приема сообщения от клиента
    char recvBuffer[512];

    int result;

    // возвращает 0, если всё окей, иначе возвращает не 0
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        cout << "WSAStartup failed, result = " << result << endl;
        return 1;
    }

    // пишем, что хотим передать в структуре hints
    // поля структуры нужно занулить для нормальной работы getaddrinfo (если поле 1, то оно учитывается, поэтому зануляем)
    ZeroMemory(&hints, sizeof(hints));
    // sizeof(hints) можно пользоваться для автоматического определения длины структуры

    //задаем протоколы
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    // хотим получать информацию пассивно
    hints.ai_flags = AI_PASSIVE;


    // функция ищет параметры, 1 - айпи или имя, 2 - порт, 3 - указатель на структуру аддринфо с параметрами коннекта, 
    result = getaddrinfo(NULL, "666", &hints, &addrResult);
    // getaddrinfo тоже возвращает int, если = 0, то всё окей, если != 0, то ошибка
    if (result != 0)
    {
        cout << "getaddrinfo failed, result = " << result << endl;
        WSACleanup();
        return 1;
    }


    // а теперь создаем сокет
    // функция сокет принимает аргументы с семейством протоколов, типом сокета и самим протоколом, всё это указывали выше 
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed" << endl;
        MyShutDown(ListenSocket, addrResult);
        return 1;
    }

    // Теперь заставляем сокет стучаться на сервер


    result == bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR)

    {
        cout << "Binding socket failed" << endl;
        // выполняем полную зачистку из-за ошибки
        ListenSocket = INVALID_SOCKET;
        MyShutDown(ListenSocket, addrResult);
        return 1;
    }

    // Устанавливаем режим прослушивания сокета

    result = listen(ListenSocket, SOMAXCONN);
    
    if (result == SOCKET_ERROR)
    {
        cout << "Listening failed" << endl;
        MyShutDown(ListenSocket, addrResult);
        return 1;
    }


    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == SOCKET_ERROR)
    {
        cout << "Accepting failed" << endl;
        MyShutDown(ClientSocket, addrResult);
        return 1;
    }

    // здесь должна быть обработка подключения следующих клиентов

    closesocket(ListenSocket);

    do
    {
        ZeroMemory(recvBuffer, sizeof(recvBuffer));
        result = recv(ClientSocket, recvBuffer, 512, 0);
        // result тут возвращает либо положительно число (сколько байт принято), либо отриц число - ошибка
        // если 0 - закрыли коннект с другой стороны
        if (result > 0)
        {
            cout << "Received" << result << " bytes" << endl; // сколько получили
            cout << "Received data" << recvBuffer << endl; // что получили

            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR)
            {
                cout << "Failed to send data to client" << endl;
                MyShutDown(ClientSocket, addrResult);
                return 1;
            }
        }

        else if (result == 0)
        {
            cout << "Connection Closing..." << endl;
        }

        else
        {
            cout << "recv failed with error: " << endl;
            MyShutDown(ClientSocket, addrResult);
            return 1;

        }


    } while (result > 0);

    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR)
    {
        cout << "Shutdown client error" << endl;
        MyShutDown(ClientSocket, addrResult);
        return 1;
    }



    MyShutDown(ClientSocket, addrResult);
    getchar();
    return 0;

}

