#include "RankingView.h"
#include <algorithm>
#include <sstream>

void RankingView::setRanking(const std::vector<PlayerResult>& results)
{
    allResults = results;
    offset = 0;
}

void RankingView::scrollUp()
{
    offset = std::max(0, offset - pageSize);
}

void RankingView::scrollDown()
{
    if (offset + pageSize < static_cast<int>(allResults.size()))
    {
        offset += pageSize;
    }
}

void RankingView::resetScroll()
{
    offset = 0;
}

std::string RankingView::buildText() const
{
    std::vector<PlayerResult> solo;
    std::vector<PlayerResult> duo;

    for (const auto& r : allResults)
    {
        if (r.mode == "DUO")
            duo.push_back(r);
        else
            solo.push_back(r);
    }

    std::ostringstream oss;

    oss << "=== SOLO ===\n";
    if (solo.empty())
    {
        oss << "Нет результатов\n";
    }
    else
    {
        int soloOffset = std::min(offset, std::max(0, static_cast<int>(solo.size()) - pageSize));
        int limit = std::min(pageSize, static_cast<int>(solo.size()) - soloOffset);
        for (int i = 0; i < limit; ++i)
        {
            const auto& r = solo[soloOffset + i];
            oss << (soloOffset + i + 1) << ". " << r.playerName << " | " << r.score << "/"
                << r.maxScore << " | " << r.timestamp << "\n";
        }
    }

    oss << "\n=== DUO ===\n";
    if (duo.empty())
    {
        oss << "Нет результатов\n";
    }
    else
    {
        int duoOffset = std::min(offset, std::max(0, static_cast<int>(duo.size()) - pageSize));
        int limit = std::min(pageSize, static_cast<int>(duo.size()) - duoOffset);
        for (int i = 0; i < limit; ++i)
        {
            const auto& r = duo[duoOffset + i];
            oss << (duoOffset + i + 1) << ". " << r.playerName << " | " << r.score << "/"
                << r.maxScore << " | " << r.timestamp << "\n";
        }
    }

    return oss.str();
}
