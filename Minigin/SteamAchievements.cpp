//
// Created by frane on 3/18/2026.
//

#include "SteamAchievements.h"

#include <iostream>

CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements):
 m_iAppID( 0 ),
 m_bInitialized( false )
#if USE_STEAMWORKS
,
 m_CallbackUserStatsStored( this, &CSteamAchievements::OnUserStatsStored ),
 m_CallbackAchievementStored( this, &CSteamAchievements::OnAchievementStored )
#endif
{
    m_pAchievements = Achievements;
    m_iNumAchievements = NumAchievements;
#if USE_STEAMWORKS
    m_iAppID = SteamUtils()->GetAppID();
    m_bInitialized = Initialize();
#endif
}

CSteamAchievements::~CSteamAchievements() {

}

bool CSteamAchievements::Initialize()
{
#if USE_STEAMWORKS
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
#endif

    return true;
}

bool CSteamAchievements::SetAchievement(const char* ID)
{
#if USE_STEAMWORKS
    // Have we received a call back from Steam yet?
    if (m_bInitialized)
    {
        SteamUserStats()->SetAchievement(ID);
        return SteamUserStats()->StoreStats();
    }
    // If not then we can't set achievements yet
#endif
    return false;
}

#if USE_STEAMWORKS
void CSteamAchievements::OnUserStatsStored( UserStatsStored_t *pCallback )
{
#if USE_STEAMWORKS
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
#endif
}

void CSteamAchievements::OnAchievementStored( UserAchievementStored_t *pCallback )
{
#if USE_STEAMWORKS
    // we may get callbacks for other games' stats arriving, ignore them
    if ( uint64(m_iAppID) == pCallback->m_nGameID )
    {
        std::cout << ( "Stored Achievement for Steam\n" );
    }
#endif
}

#endif
