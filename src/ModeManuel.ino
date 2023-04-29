#include "FS.h"
#include "SPIFFS.h"

setup(){
    writeWood(1, 100, 100);
    readWood(1);
    modifyWood(1, 200, 200);
}

if (!SPIFFS.begin(true))
{
    Serial.println("An error has occurred while mounting SPIFFS");
    return wood;
}

Wood_t readWood(int id)
{
    Wood_t wood;

    File file = fs.open("/wood.json", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return wood;
    }

    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return wood;
    }

    JsonObject root = doc.as<JsonObject>();
    for (JsonPair keyValue : root)
    {
        JsonObject woodObj = keyValue.value().as<JsonObject>();
        if (woodObj["code"].as<int>() == id)
        {
            strncpy(wood.name, woodObj["name"].as<const char *>(), sizeof(wood.name) - 1);
            wood.code = woodObj["code"].as<int>();
            wood.sawSpeed = woodObj["sawSpeed"].as<int>();
            wood.feedRate = woodObj["feedRate"].as<int>();
            break;
        }
        else
        {
            strncpy(wood.name, "NULL", sizeof(wood.name) - 1);
        }
    }

    file.close();
    return wood;
}

void writeWood(int id, int sawSpeed, int feedRate)
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open("/wood.json", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    JsonObject newWood;
    newWood["name"] = woodType[id];
    newWood["code"] = id;
    newWood["sawSpeed"] = sawSpeed;
    newWood["feedRate"] = feedRate;

    JsonObject root = doc.as<JsonObject>();
    root[String(id)] = newWood;

    String modifiedData;
    serializeJson(doc, modifiedData);

    file.close();

    File writeFile = SPIFFS.open("/wood.json", "w");
    if (!writeFile)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    writeFile.print(modifiedData);
    writeFile.close();
}

void updateWood(int id, int sawSpeed, int feedRate)
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open("/wood.json", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    JsonObject root = doc.as<JsonObject>();
    JsonObject woodObj = root[String(id)];

    // Update the properties of the JsonObject
    woodObj["name"] = woodType[id];
    woodObj["sawSpeed"] = sawSpeed;
    woodObj["feedRate"] = feedRate;

    // Serialize the modified JsonDocument back to a string
    String modifiedData;
    serializeJson(doc, modifiedData);

    file.close();

    // Save the modified JSON data back to the file
    File writeFile = SPIFFS.open("/wood.json", "w");
    if (!writeFile)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    writeFile.print(modifiedData);
    writeFile.close();
}