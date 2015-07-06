#pragma once

#include "Messages.hpp"
#include "IpAddrPort.hpp"
#include "Controller.hpp"
#include "ControllerManager.hpp"
#include "KeyValueStore.hpp"
#include "HttpDownload.hpp"
#include "HttpGet.hpp"

#include <string>
#include <memory>


// The function to run the game with the provided options
typedef void ( * RunFuncPtr ) ( const IpAddrPort& address, const Serializable& config );

class ConsoleUi;

// Function that computes the delay from the latency
inline int computeDelay ( double latency )
{
    return ( int ) ceil ( latency / ( 1000.0 / 60 ) );
}


class MainUi
    : private Controller::Owner
    , private ControllerManager::Owner
    , private HttpDownload::Owner
    , private HttpGet::Owner
{
public:

    InitialConfig initialConfig;

    std::string sessionMessage;

    std::string sessionError;


    void initialize();

    void main ( RunFuncPtr run );

    void update ( bool isStartup = false );

    void display ( const std::string& message, bool replace = true );

    bool connected ( const InitialConfig& initialConfig, const PingStats& pingStats );

    void spectate ( const SpectateConfig& spectateConfig );

    bool confirm ( const std::string& question );


    void setMaxRealDelay ( uint8_t delay );

    void setDefaultRollback ( uint8_t rollback );

    const KeyValueStore& getConfig() const { return config; }

    const NetplayConfig& getNetplayConfig() const { return netplayConfig; }


    static void *getConsoleWindow();

    static std::string formatStats ( const PingStats& pingStats );

private:

    std::shared_ptr<ConsoleUi> ui;

    KeyValueStore config;

    IpAddrPort address;

    NetplayConfig netplayConfig;

    Controller *currentController = 0;

    uint32_t mappedKey = 0;

    std::shared_ptr<HttpGet> httpGet;

    std::shared_ptr<HttpDownload> httpDl;

    uint32_t serverIdx = 0;

    Version latestVersion;

    bool downloadCompleted = false;

    std::string tmpDir;

    void netplay ( RunFuncPtr run );
    void spectate ( RunFuncPtr run );
    void broadcast ( RunFuncPtr run );
    void offline ( RunFuncPtr run );
    void controls();
    void settings();

    bool areYouSure();

    bool gameMode ( bool below );
    bool offlineGameMode();

    void doneMapping ( Controller *controller, uint32_t key ) override;

    void attachedJoystick ( Controller *controller ) override {};
    void detachedJoystick ( Controller *controller ) override;

    void saveConfig();
    void loadConfig();

    void saveMappings ( const Controller& controller );
    void loadMappings ( Controller& controller );

    void alertUser();

    std::string formatPlayer ( const SpectateConfig& spectateConfig, uint8_t player ) const;

    void httpResponse ( HttpGet *httpGet, int code, const std::string& data, uint32_t remainingBytes ) override;
    void httpFailed ( HttpGet *httpGet ) override;
    void httpProgress ( HttpGet *httpGet, uint32_t receivedBytes, uint32_t totalBytes ) override {}

    void downloadComplete ( HttpDownload *httpDl ) override;
    void downloadFailed ( HttpDownload *httpDl ) override;
    void downloadProgress ( HttpDownload *httpDl, uint32_t downloadedBytes, uint32_t totalBytes ) override;

    void updateTo ( const std::string& version );

    bool configure ( const PingStats& pingStats );

};