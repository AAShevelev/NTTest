#pragma once
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "../Common/json.hpp"
#include "../Common/Common.hpp"
#include "Core.hpp"

using boost::asio::ip::tcp;

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';

            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";

            if (reqType == Requests::Registration)
            {
                reply = GetCore().RegisterNewUser(j["Message"]);
            }
            else if (reqType == Requests::Buy)
            {
                GetCore().SetUserProposal(j["UserId"], j["Message"], "buy");
                if(!GetCore().hasSellers())
                {
                    reply = "There are no sellers yet. Your buying proposal was added\n";
                }
                else
                {
                    while(GetCore().hasSellers() && GetCore().hasBuyers())
                    {
                        std::cout << GetCore().Buy(j["UserId"]);
                    }
                    reply = "We found a deal for you, check your balance\n";
                }
            }
            else if (reqType == Requests::Sell)
            {
                GetCore().SetUserProposal(j["UserId"], j["Message"], "sell");
                if (!GetCore().hasBuyers())
                {
                   reply = "There are no buyers yet. Your selling proposal was added\n";
                }
                else
                {
                    while(GetCore().hasSellers() && GetCore().hasBuyers())
                    {
                        std::cout << GetCore().Sell(j["UserId"]);
                    }
                    reply = "We found a deal for you, check your balance\n";
                }
            }
            else if (reqType == Requests::Balance)
            {
                reply = GetCore().Balance(j["UserId"]);
            }
            else if (reqType == Requests::Active)
            {
                reply = GetCore().Active();
            }
            else if (reqType == Requests::Reject)
            {
                GetCore().Reject(j["UserId"]);
                reply = "Removed\n";
            }

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

