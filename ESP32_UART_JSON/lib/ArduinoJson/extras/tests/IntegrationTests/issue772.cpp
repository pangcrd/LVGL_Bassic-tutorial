// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2025, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

// https://github.com/bblanchon/ArduinoJson/issues/772

TEST_CASE("Issue772") {
  JsonDocument doc1;
  JsonDocument doc2;
  DeserializationError err;
  std::string data =
      "{\"state\":{\"reported\":{\"timestamp\":\"2018-07-02T09:40:12Z\","
      "\"mac\":\"2C3AE84FC076\",\"firmwareVersion\":\"v0.2.7-5-gf4d4d78\","
      "\"visibleLight\":261,\"infraRed\":255,\"ultraViolet\":0.02,"
      "\"Temperature\":26.63,\"Pressure\":101145.7,\"Humidity\":54.79883,"
      "\"Vbat\":4.171261,\"soilMoisture\":0,\"ActB\":0}}}";
  err = deserializeJson(doc1, data);
  REQUIRE(err == DeserializationError::Ok);

  data = "";
  serializeMsgPack(doc1, data);
  err = deserializeMsgPack(doc2, data);

  REQUIRE(err == DeserializationError::Ok);
}
