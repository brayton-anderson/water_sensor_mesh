#include "MeshModule.h"
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

class NAU7802Sensor : public TelemetrySensor
{
  private:
    NAU7802 nau7802;

  protected:
    virtual void setup() override;
    const char *nau7802ConfigFileName = "/prefs/nau7802.dat";
    bool saveCalibrationData();
    bool loadCalibrationData();

  public:
    NAU7802Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    void tare();
    void calibrate(float weight);
    AdminMessageHandleResult handleAdminMessage(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_AdminMessage *request,
                                                water_sensor_mesh_AdminMessage *response) override;
};

#endif