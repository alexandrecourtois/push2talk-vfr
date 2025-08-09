#ifdef TARGET_XP11
#define XPLM300 = 1
#else
#define XPLM400 = 1
#endif

#include <chrono>
#include <zmq.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMWeather.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <XPLMProcessing.h>
#include <cstring>
#include <reqrsp.h>
#include <zmq_server.h>
#include <enums.h>
#include <lang.h>
#include <fr_FR.h>

XPLMDataRef dref_tailnum        = nullptr;
XPLMDataRef dref_visibility     = nullptr;
XPLMDataRef dref_qnh            = nullptr;
XPLMDataRef dref_windspeed      = nullptr;
XPLMDataRef dref_winddir        = nullptr;
XPLMDataRef dref_latitude       = nullptr;
XPLMDataRef dref_longitude      = nullptr;
XPLMDataRef dref_com1_freq      = nullptr;
XPLMDataRef dref_com2_freq      = nullptr;
XPLMDataRef dref_com1_vol       = nullptr;
XPLMDataRef dref_com2_vol       = nullptr;
XPLMDataRef dref_com1_on        = nullptr;
XPLMDataRef dref_com2_on        = nullptr;
XPLMDataRef dref_com1_power     = nullptr;
XPLMDataRef dref_com2_power     = nullptr;
XPLMDataRef dref_altitude       = nullptr;
XPLMDataRef dref_battery_on     = nullptr;
XPLMDataRef dref_region_rain    = nullptr;
XPLMDataRef dref_sim_paused     = nullptr;
XPLMDataRef dref_local_time     = nullptr;

Rsp_Dataframe dataframe;
zmq::context_t context(1);
ZMQ_Server* pserver;

// Fonction de rappel qui sera appelée périodiquement
float callback_dataframe(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon) {
    dataframe.acquire([]() {
        Req pserver_req = pserver->getLastRequest();
        
        XPLMGetDatab(dref_tailnum, dataframe.tailnum, 0, sizeof(dataframe.tailnum));
        dataframe.visibility    = XPLMGetDataf(dref_visibility) * 1609.34f;
        dataframe.qnh           = XPLMGetDataf(dref_qnh);
        dataframe.windspeed     = XPLMGetDataf(dref_windspeed);
        dataframe.winddir       = XPLMGetDataf(dref_winddir);
        dataframe.latitude      = XPLMGetDataf(dref_latitude);
        dataframe.longitude     = XPLMGetDataf(dref_longitude);
        dataframe.com1_freq     = XPLMGetDatai(dref_com1_freq);
        dataframe.com2_freq     = XPLMGetDatai(dref_com2_freq);
        dataframe.com1_vol      = XPLMGetDataf(dref_com1_vol);
        dataframe.com2_vol      = XPLMGetDataf(dref_com2_vol);
        dataframe.com1_active   = XPLMGetDatai(dref_com1_on);
        dataframe.com2_active   = XPLMGetDatai(dref_com2_on);
        dataframe.com1_power    = XPLMGetDatai(dref_com1_power);
        dataframe.com2_power    = XPLMGetDatai(dref_com2_power);
        dataframe.altitude      = XPLMGetDataf(dref_altitude) * 3.28084f;
        dataframe.region_rain   = XPLMGetDataf(dref_region_rain);
        dataframe.sim_paused    = XPLMGetDatai(dref_sim_paused);
        dataframe.local_time_sec = XPLMGetDataf(dref_local_time);
        XPLMGetDatavi(dref_battery_on, dataframe.battery_on, 0, sizeof(dataframe.battery_on));

    #ifndef TARGET_XP11
        if (pserver_req.type == Request::UPD_AIRPORT) {
            XPLMWeatherInfo_t w;

            memset(&w, 0, sizeof(XPLMWeatherInfo_t));
            w.structSize = sizeof(XPLMWeatherInfo_t);

            XPLMGetWeatherAtLocation(pserver_req.argD_1, pserver_req.argD_2, pserver_req.argD_3, &w);

            //dataframe.lock();
            dataframe.infreq_visibility = w.visibility;
            dataframe.infreq_qnh = w.pressure_alt;
            dataframe.infreq_windspeed = w.wind_spd_alt;
            dataframe.infreq_winddir = w.wind_dir_alt;
            //dataframe.unlock();
        }
    #endif
    });


    return 0.5; // Retourne 2.0 pour rappeler cette fonction toutes les 2 secondes
}

// Fonction de démarrage du plugin
PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    strcpy(outName, "push2talk-vfr");
    strcpy(outSig, "xplane.plugin.push2talk-vfr");
    strcpy(outDesc, "Push2Talk VFR Plugin");

    XPLMGetVersions(&dataframe.xp_major, &dataframe.xp_minor, NULL);

    dref_tailnum        = XPLMFindDataRef("sim/aircraft/view/acf_tailnum");
    dref_visibility     = XPLMFindDataRef("sim/weather/aircraft/visibility_reported_sm");
    dref_qnh            = XPLMFindDataRef("sim/weather/aircraft/barometer_current_pas");
    dref_windspeed      = XPLMFindDataRef("sim/weather/aircraft/wind_speed_kts");
    dref_winddir        = XPLMFindDataRef("sim/weather/aircraft/wind_direction_degt");
    dref_latitude       = XPLMFindDataRef("sim/flightmodel/position/latitude");
    dref_longitude      = XPLMFindDataRef("sim/flightmodel/position/longitude");
    dref_com1_freq      = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
    dref_com2_freq      = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_frequency_hz_833");
    dref_com1_vol       = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_volume_com1");
    dref_com2_vol       = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_volume_com2");
    dref_com1_on        = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_selection_com1");
    dref_com2_on        = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_selection_com2");
    dref_com1_power     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
    dref_com2_power     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
    dref_altitude       = XPLMFindDataRef("sim/flightmodel2/position/y_agl");
    dref_battery_on     = XPLMFindDataRef("sim/cockpit2/electrical/battery_on");
    dref_region_rain    = XPLMFindDataRef("sim/weather/region/rain_percent");
    dref_sim_paused     = XPLMFindDataRef("sim/time/paused");
    dref_local_time     = XPLMFindDataRef("sim/time/local_time_sec");

    Lang::setLang<fr_FR>();
    pserver = new ZMQ_Server(context, TCP_Server("*:5555"), dataframe);
    pserver->start();

    XPLMRegisterFlightLoopCallback(callback_dataframe, -1, NULL);

    return 1;
}

PLUGIN_API void XPluginStop() {
    XPLMUnregisterFlightLoopCallback(callback_dataframe, 0);
    dataframe.acquire([]() {
        dataframe.server_down = true;
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    pserver->stop();
    delete pserver;
}

PLUGIN_API int XPluginEnable() {
    return 1;
}

PLUGIN_API void XPluginDisable() {}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam) {}
