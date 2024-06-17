#include <iostream>
#include <boost/asio.hpp>
#include "../Common/json.hpp"
#include "../Common/Common.hpp"

using boost::asio::ip::tcp;

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const std::string& aMessage = "")
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;
    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);

        while (true)
        {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                "1) Set or change proposal\n"
                "2) Balance\n"
                "3) Show active proposals\n"
                "4) Reject proposal\n"
                "0) Exit\n"
                << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    std::cout << "Buying or selling?:\n"
                        "1) Buying\n"
                        "2) Selling\n"
                        "3) Return\n";
                    short option;
                    std::cin >> option;
                    if (option != 2 and option != 1) break;
                    std::cout << "Enter amount and price\n";
                    std::string am, pr;
                    std::cin >> am >> pr;
                    if(option == 1)
                    {
                        SendMessage(s, my_id, Requests::Buy, am + " " + pr);
                    }
                    else if(option == 2)
                    {
                        SendMessage(s, my_id, Requests::Sell, am + " " + pr);
                    }
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    SendMessage(s, my_id, Requests::Balance);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 3:
                {
                    SendMessage(s, my_id, Requests::Active);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 4:
                {
                    SendMessage(s, my_id, Requests::Reject);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 0:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}