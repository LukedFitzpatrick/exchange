#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

enum Side
{
    BUY,
    SELL
};

enum Type
{
    GFD,
    FAK
};

struct Order
{
    Order(int id, int price, int volume, Side side, Type type)
        : mId(id)
        , mPrice(price)
        , mVolume(volume)
        , mSide(side)
        , mType(type)
    {}

    // for now, assume only one product so no feedcode needed
    int mId;
    int mPrice;
    int mVolume;
    Side mSide;
    Type mType;
    
    void Show()
    {
        if(mSide == BUY)
        {
            std::cout<<"(B "<<mVolume<<")\t"<<mPrice<<std::endl;
        }
        else
        {
            std::cout<<"\t"<<mPrice<<"\t(S "<<mVolume<<")"<<std::endl;
        }
    }

    void VerboseShow()
    {
        std::string side = (mSide==BUY) ? "BUY" : "SELL";
        std::cout << side << " " << mVolume << "@" << mPrice <<std::endl;
    }
};

// sorts in the order that offers would be filled
bool SortOrdersAscendingByPrice(const Order& lhs, const Order& rhs)
{
    return lhs.mPrice < rhs.mPrice;
}

// sorts in the order that bids would be filled
bool SortOrdersDescendingByPrice(const Order& lhs, const Order& rhs)
{
    return rhs.mPrice < lhs.mPrice;
}

struct OrderBook
{
    // TODO std::deque?
    
    // bids want to buy
    std::vector<Order> mBids = {};

    // offers want to sell
    std::vector<Order> mOffers = {};

    void HandleNewOrder(Order o)
    {
        std::cout<<"[OrderBook] Received order: ";
        o.VerboseShow();
        
        bool fullyTraded = TryToFillOrder(o);
        if(!fullyTraded)
        {
            AddOrder(o);
        }
    }

    // returns true when an order fully trades, false
    // otherwise. Updates the order volume when it partially trades.
    bool TryToFillOrder(Order& o)
    {
        if(o.mSide == BUY && !mOffers.empty())
        {
            // if(mOffers[0].mPrice <= o.mPrice)
            // {
            //     std::cout<<"Found a trade with:";
            //     mOffers[0].Show();
            // }
        }
        else if(o.mSide == SELL && !mBids.empty())
        {
            while(!mBids.empty() && mBids[0].mPrice >= o.mPrice)
            {
                // we have a trade!

                // the aggressor has less volume, total fill
                if(mBids[0].mVolume >= o.mVolume)
                {
                    // TODO broadcast messages here to participants
                    std::cout<<"[OrderBook] Order fully traded @ "<<mBids[0].mPrice <<std::endl;
                    mBids[0].mVolume -= o.mVolume;
                    if(mBids[0].mVolume == 0)
                    {
                        // TODO Inefficient call here! Maybe store in reverse order?
                        mBids.erase(mBids.begin());
                    }
                    return true;
                }
                // the aggressor has more volume, partial fill
                else
                {
                    std::cout<<"[OrderBook] Order partially traded @ "<<mBids[0].mPrice << std::endl;;
                    o.mVolume -= mBids[0].mVolume;
                    mBids.erase(mBids.begin());
                }
            }
            return false;
        }
        
        return false;
    }
    
    void AddOrder(const Order& o)
    {
        if(o.mSide == BUY)
        {
            mBids.push_back(o);
            std::stable_sort(mBids.begin(), mBids.end(), SortOrdersDescendingByPrice);
        }
        else
        {
            mOffers.push_back(o);
            std::stable_sort(mOffers.begin(), mOffers.end(), SortOrdersAscendingByPrice);
        }
    }

    void Show()
    {
        auto reversedOffers = mOffers;
        std::reverse(reversedOffers.begin(), reversedOffers.end());
        for(auto& order : reversedOffers)
        {
            order.Show();
        }
        
        for(auto& order : mBids)
        {
            order.Show();
        }
    }
};

// wrap the evil platform specific code in here
void ClearScreen()
{
    system("clear");
}

int main()
{
    OrderBook book;
    int i = 0;

    std::vector<Order> orders =
        {
            Order(i++, 109, 10, BUY, GFD),
            Order(i++, 108, 10, BUY, GFD),
            Order(i++, 107, 10, BUY, GFD),
            Order(i++, 106, 10, BUY, GFD),
            Order(i++, 120, 5, SELL, GFD),
            Order(i++, 108, 5, SELL, GFD),
            Order(i++, 100, 29, SELL, GFD)
        };


    for(Order& order : orders)
    {
        std::cout<<"===================================="<<std::endl;
        book.HandleNewOrder(order);
        book.Show();
        std::cout<<"====================================\n\n"<<std::endl;

    }
}
