// chat_client2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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

// просто переменная типа сокет для будущей проверки - а создасться ли сокет
    SOCKET ConnectSocket = INVALID_SOCKET;

    const char* sendBuffer = "Hello from client";

    // буфер приема сообщения от сервера
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





    // функция ищет параметры, 1 - айпи или имя, 2 - порт, 3 - указатель на структуру аддринфо с параметрами коннекта, 
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    // getaddrinfo тоже возвращает int, если = 0, то всё окей, если != 0, то ошибка
    if (result != 0)
    {
        cout << "getaddrinfo failed, result = " << result << endl;
        WSACleanup();
        return 1;
    }


    // а теперь создаем сокет
    // функция сокет принимает аргументы с семейством протоколок, типом сокета и самим протоколом, всё это указывали выше 
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) 
    {
        cout << "Socket creation failed" << endl;
        MyShutDown(ConnectSocket, addrResult);
        return 1;
    }

    // Теперь заставляем сокет стучаться на сервер


    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) 

    {         
        cout << "Unable to connection to server" << endl;
        // выполняем полную зачистку из-за ошибки
        ConnectSocket = INVALID_SOCKET;
        MyShutDown(ConnectSocket, addrResult);
        return 1;
    }

    // отправляем что-нибудь на сервер
    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);

    // функция send возвращает кол-во переданных данных, либо SOCKET_ERROR

    if (result == SOCKET_ERROR)
    {
        cout << "Send failed, error: " << result << endl;
        MyShutDown(ConnectSocket, addrResult);
        return 1;

    }

    cout << "Sent: " << result << " bytes" << endl;

    // выключаем отправку через наш сокет, но прием работает
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR)
    {
        cout << "Shutdown error: " << result << endl;
        MyShutDown(ConnectSocket, addrResult);
        return 1;
    }

    
    
    do
    {
        ZeroMemory(recvBuffer, sizeof(recvBuffer));
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        // result тут возвращает либо положительно число (сколько байт принято), либо отриц число - ошибка
        // если 0 - закрыли коннект с другой стороны
        if (result > 0) 
        {
            cout << "Received" << result << " bytes" << endl; // сколько получили
            cout << "Received data" << recvBuffer << endl; // что получили
        }

        else if (result == 0)
        {
            cout << "Connection Closed" << endl;
        }

        else 
        {
            cout << "recv failed with error: " << endl;
        }
            

    } while (result > 0);

    MyShutDown(ConnectSocket, addrResult);;
    getchar();
    return 0;

}

