//
// Created by frane on 3/18/2026.
//

#include "SteamAchievements.h"

#include <iostream>

#if USE_STEAMWORKS
CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements):
 m_iAppID( 0 ),
 m_bInitialized( false ),
 m_CallbackUserStatsStored( this, &CSteamAchievements::OnUserStatsStored ),
 m_CallbackAchievementStored( this, &CSteamAchievements::OnAchievementStored )
{
    m_iAppID = SteamUtils()->GetAppID();
    m_pAchievements = Achievements;
    m_iNumAchievements = NumAchievements;
    m_bInitialized = Initialize();
}

CSteamAchievements::~CSteamAchievements() {

}

bool CSteamAchievements::Initialize()
{
    // Is Steam loaded? If not we can't get stats.
    if ( NULL == SteamUserStats() || NULL == SteamUser() )
    {
        return false;
    }
    // Is the user logged on?  If not we can't access stats.
    if ( !SteamUser()->BLoggedOn() )
    {
        return false;
    }

    return true;
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
    // Have we received a call back from Steam yet?
    if (m_bInitialized)
    {
        SteamUserStats()->SetAchievement(ID);
        return SteamUserStats()->StoreStats();
    }
    // If not then we can't set achievements yet
    return false;
}

void CSteamAchievements::OnUserStatsStored( UserStatsStored_t *pCallback )
{
    // we may get callbacks for other games' stats arriving, ignore them
    if ( uint64(m_iAppID) == pCallback->m_nGameID )
    {
        if ( k_EResultOK == pCallback->m_eResult )
        {
            std::cout << ( "Stored stats for Steam\n" );
        }
        else
        {
            char buffer[128];
            _snprintf_s( buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult );
            std::cout << ( buffer ) << std::endl;
        }
    }
}

void CSteamAchievements::OnAchievementStored( UserAchievementStored_t *pCallback )
{
    // we may get callbacks for other games' stats arriving, ignore them
    if ( uint64(m_iAppID) == pCallback->m_nGameID )
    {
        std::cout << ( "Stored Achievement for Steam\n" );
    }
}
#endif