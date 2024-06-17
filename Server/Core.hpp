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
        user aUser(aUserName, newUserId);
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
            return userIt->second.name();
        }
    }

    // Set buying or selling proposal
    bool SetUserProposal(const std::string& aUserId, const std::string& moneyInfo, const std::string& type)
    {
        size_t id = std::stoi(aUserId);
        const auto userIt = mUsers.find(id);
        userIt->second.setStatus(type);

        
        size_t amount = std::stoi(moneyInfo.substr(0, moneyInfo.find(' ')));
        size_t price = std::stoi(moneyInfo.substr(moneyInfo.find(' ') + 1));

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
        std::string prop = userIt->second.status() + " " + std::to_string(userIt->second.amount()) +
            "$ for " + std::to_string(userIt->second.price()) + " rub\n";;
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
        return makeTransaction(buyerIt->second, sellerIt->second, sellerIt->second.price());
    }

    // Sell if someone is buying
    std::string Sell(const std::string& aUserId)
    {
        const auto sellerIt = mUsers.find(std::stoi(aUserId));
        const auto buyerIt = mUsers.find(chooseProposal(buyers, "sell"));
        return makeTransaction(buyerIt->second, sellerIt->second, buyerIt->second.price());
    }

    // Make a transaction
    std::string makeTransaction(user& buyer, user& seller, size_t price)
    {
        int cur = std::min(buyer.amount(), seller.amount());
        int rub = price * cur;

        // Removing currency from proposal
        buyer.reduceAm(cur);
        seller.reduceAm(cur);

        // Change balances
        seller.changeD(-cur);
        seller.changeR(rub);
        buyer.changeD(cur);
        buyer.changeR(-rub);

        // Close the deals
        if (buyer.amount() == 0)
        {
            buyers.erase(buyer.id());
        }
        if (seller.amount() == 0)
        {
            sellers.erase(seller.id());
        }

        buyer.addHistory(buyer.name() + " bought " + std::to_string(cur) + "$ from " +
            seller.name() + " for " + std::to_string(rub) + " RUB\n");

        seller.addHistory(seller.name() + " sold " + std::to_string(cur) + "$ to " +
            buyer.name() + " for " + std::to_string(rub) + " RUB\n");

        quotation += std::to_string(price) + " ";

        return "Making transaction between " + buyer.name() + " and " + seller.name() + "\n";
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
                active += mUsers[it->first].name() + " is selling " +
                    std::to_string(mUsers[it->first].amount()) + "$ for " +
                    std::to_string(mUsers[it->first].price()) + "\n";
            }
        else if (hasBuyers())
            for (auto it = buyers.begin(); it != buyers.end(); ++it)
            {
                active += mUsers[it->first].name() + " is buying " +
                    std::to_string(mUsers[it->first].amount()) + "$ for " +
                    std::to_string(mUsers[it->first].price()) + " RUB\n";
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

    std::string showHistory(const std::string& aUserId)
    {
        return mUsers.find(std::stoi(aUserId))->second.history();
    }

    std::string showQuotation()
    {
        if (quotation.length() == 0)
            return "There were no deals yet\n";
        else
            return quotation;
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

    std::string quotation = "";
};

Core& GetCore()
{
    static Core core;
    return core;
}