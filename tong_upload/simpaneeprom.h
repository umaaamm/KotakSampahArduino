 #include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int simpan(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
    EEPROM.commit();
    return i;
}

template <class T> int baca(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
    EEPROM.commit();
    return i;
}
