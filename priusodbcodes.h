#ifndef PRIUSODBCODES_H
#define PRIUSODBCODES_H

namespace QPriusCan {
enum PriusDataODBCode {
  SterringId = 0x25,
  BrakesId = 0x30,
  FrontWheelsSpeedId = 0xB1,
  RearWheelsSpeedId = 0xB3,
  IceId = 0x348,
  IceRpmId = 0x3C8,
  SpeedId = 0x3CA,
  BatterySocId = 0x3CB,
  BatteryVolyageId = 0x3CD,
  EvModeId = 0x529,
  EngineTemperatureId = 0x52C,
  ShiftLevelId = 0x540,
  LightId = 0x57F,
  GasGaugeId = 0x5A4,
  DoorsId = 0x5B6

};
enum PriusErrorODBCode {};
}
#endif // PRIUSODBCODES_H
