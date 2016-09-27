// Virtual Serial Port Emulator API header
// Author: Volodymyr Ter (http://www.eterlogic.com)

#ifndef _VSPE_API_H_
#define _VSPE_API_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
* Activate VSPE API using activation key
* \return result
*/
bool vspe_activate(const char* key);

/*
 * Initialize VSPE core
 * \return result
 */
bool vspe_initialize();

/*
 * Load configuration file
 * \param name 
 * \return result
 */
bool vspe_loadConfiguration(const char* name);


/*
* Save configuration
* \param name 
* \return result
*/
bool vspe_saveConfiguration(const char* name);


/*
* Create device
* \param name Device name. For example "Connector", "Splitter", "Pair" etc.
* \param initString device initialization string
* \return deviceId
*/
int vspe_createDevice(const char* name, const char* initString);

/*
* Destroy device by deviceId
* \param deviceId
* \return result
*/
bool vspe_destroyDevice(int deviceId);


/*
* Get VSPE devices count
* \return result
*/
int vspe_getDevicesCount();


/*
* Get VSPE deviceId by device index
* \param idx device index
* \return deviceId
*/
int vspe_getDeviceIdByIdx(int idx);

/*
* Get VSPE deviceId by COM port index
* \param ComPortIdx
* \return deviceId (-1 if not found).
*/
int vspe_getDeviceIdByComPortIndex(int ComPortIdx);

/*
* Get device information
* \param deviceId
* \param name [out] device name
* \param initStirng [out] device initString
* \param ok [out] device state (1 = good)
* \param used [out] device clients count (0 - not used)
* \return result
*/
bool vspe_getDeviceInfo(int deviceId, const char** name, const char** initString, int* ok, int* used);

/*
* Reinitialize device by deviceId
* \param deviceId
* \return result
*/
bool vspe_reinitializeDevice(int deviceId);

/*
* Destroy all devices
* \return result
*/
bool vspe_destroyAllDevices();


/*
 * Start emulation
 * \return result
 */
bool vspe_startEmulation();

/*
 * Stop emulation
 * \return result
 */
bool vspe_stopEmulation();

/*
 * Release VSPE core
 */
void vspe_release();

/*
* Get VSPE API version information
* \return result
*/
const char* vspe_getVersionInformation();

#ifdef __cplusplus
}
#endif

#endif
