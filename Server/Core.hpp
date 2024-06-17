#pragma once
#include <string>
#include <map>
#include "user.hpp"

class Core
{
public:
    // "–егистрирует" нового пользовател€ и возвращает его ID.
    std::string RegisterNewUser(const std::string& aUserName)
    {
        size_t newUserId = mUsers.size();
        user aUser(aUserName);
        aUser.id_ = newUserId;
        mUsers[newUserId] = aUser;

        return std::to_string(newUserId);
    }

    // «апрос имени клиента по ID
    std::string GetUserName(const std::string& aUserId)
    {
        const auto userIt = mUsers.find(std::stoi(aUserId));
        if (userIt == mUsers.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second.name_;
        }
    }

    // Set buying or selling proposal
    bool SetUserProposal(const std::string& aUserId, const std::string& moneyInfo, const std::string& type)
    {
        size_t id = std::stoi(aUserId);
        const auto userIt = mUsers.find(id);
        userIt->second.status_ = type;

        std::stringstream  stream(moneyInfo);
        std::string word;
        stream >> word;
        size_t amount = std::stoi(word);
        stream >> word;
        size_t price = std::stoi(word);

        userIt->second.setProposal(type, amount, price);

        if (type == "buy")
        {
            if (sellers.count(id))
            {
                sellers.erase(id);
            }
            buyers[id] = price;
            return true;
        }
        else if (type == "sell")
        {
            if (buyers.count(id))
            {
                buyers.erase(id);
            }
            sellers[id] = price;
            return true;
        }
        else
        {
            return false;
        }
    }

    // Show a proposal
    string UserProposal(const std::string& aUserId)
    {
        const auto userIt = mUsers.find(std::stoi(aUserId));
        std::string prop = userIt->second.status_ + " " + std::to_string(userIt->second.amount_) + 
            "$ for " + std::to_string(userIt->second.price_) + " rub\n";;
        return prop;
    }

    //choose best proposal
    //type must be "buy" or "sell" 
    size_t chooseProposal(const map<size_t, size_t>& props, const std::string& type)
    {
        auto mit = props.begin(); 
        if (type == "buy")
        {
            size_t min = props.begin()->second;
            for (auto it = props.begin(); it != props.end(); ++it)
            {
                if (it->second < min)
                {
                    min = it->second;
                    mit = it;
                }
            }
        }
        else if (type == "sell")
        {
            size_t max = props.begin()->second;
            for (auto it = props.begin(); it != props.end(); ++it)
            {
                if (it->second > max)
                {
                    max = it->second;
                    mit = it;
                }
            }
        }
        return mit->first;
    }

    // Buy if someone is selling
    std::string Buy(const std::string& aUserId)
    {
        const auto buyerIt = mUsers.find(std::stoi(aUserId));
        const auto sellerIt = mUsers.find(chooseProposal(sellers, "buy"));
        return makeTransaction(buyerIt->second, sellerIt->second, sellerIt->second.price_);
    }

    // Sell if someone is buying
    std::string Sell(const std::string& aUserId)
    {
        const auto sellerIt = mUsers.find(std::stoi(aUserId));
        const auto buyerIt = mUsers.find(chooseProposal(buyers, "sell"));
        return makeTransaction(buyerIt->second, sellerIt->second, buyerIt->second.price_);
    }

    // Make a transaction
    std::string makeTransaction(user& buyer, user& seller, size_t prise)
    {
        int cur = std::min(buyer.amount_, seller.amount_);
        int rub = prise * cur;

        // Removing currency from proposal
        buyer.amount_ -= cur;
        seller.amount_ -= cur;

        // Change balances
        seller.dollars_ -= cur;
        seller.rubbles_ += rub;
        buyer.dollars_ += cur;
        buyer.rubbles_ -= rub;

        // Close the deals
        if (buyer.amount_ == 0)
        {
            buyers.erase(buyer.id_);
        }
        if (seller.amount_ == 0)
        {
            sellers.erase(seller.id_);
        }

        return "Making transaction between " + buyer.name_ + " and " + seller.name_ + "\n";
    }

    // User's balance
    std::string Balance(const std::string& aUserId)
    {
        return mUsers.find(std::stoi(aUserId))->second.showBalance();
    }

    // Show active proposals
    std::string Active()
    {
        std::string active = "";
        if(hasSellers())
            for (auto it = sellers.begin(); it != sellers.end(); ++it)
            {
                active += mUsers[it->first].name_ + " is selling " + 
                    std::to_string(mUsers[it->first].amount_) + "$ for " +
                    std::to_string(mUsers[it->first].price_) + "\n";
            }
        else if (hasBuyers())
            for (auto it = buyers.begin(); it != buyers.end(); ++it)
            {
                active += mUsers[it->first].name_ + " is buying " +
                    std::to_string(mUsers[it->first].amount_) + "$ for " +
                    std::to_string(mUsers[it->first].price_) + " RUB\n";
            }
        else
        {
            active = "No active proposals\n";
        }
        return active;
    }

    // Reject proposal
    void Reject(const std::string& aUserId)
    {
        size_t id = std::stoi(aUserId);

        if(sellers.count(id))
        {
            sellers.erase(id);
        }
        if (buyers.count(id))
        {
            buyers.erase(id);
        }
    
        mUsers.find(id)->second.setProposal("None", 0, 0);
    }

    // Check for sellers or buyers
    bool hasSellers()
    {
        return sellers.size() > 0;
    }
    bool hasBuyers()
    {
        return buyers.size() > 0;
    }

private:
    // <UserId, user>
    std::map<size_t, user> mUsers;
    // List of buyer's IDs and prises
    std::map<size_t, size_t> buyers;
    // List of seller's IDs and prises
    std::map<size_t, size_t> sellers;
};

Core& GetCore()
{
    static Core core;
    return core;
}