#include "PlayerManager.h"

void PlayerManager::addPlayer(const std::string& name)
{
    players_.push_back({name, 0});
}

void PlayerManager::clear()
{
    players_.clear();
    currentIndex_ = 0;
}

const std::vector<Player>& PlayerManager::players() const
{
    return players_;
}

Player& PlayerManager::current()
{
    return players_.at(currentIndex_);
}

bool PlayerManager::next()
{
    if (currentIndex_ + 1 < players_.size())
    {
        ++currentIndex_;
        return true;
    }
    return false;
}

bool PlayerManager::empty() const
{
    return players_.empty();
}