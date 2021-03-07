# neopixel twinkle

## set

```
all/neopixel/twinkle 
{
  "colour":"red",
  "operation":"set"
}
```

## setrgb

```
all/neopixel/twinkle 
{
  "r":45,
  "g":255,
  "b":238,
  "a":1,
  "operation":"setrgb"
}
```

## twinkle
```
mosquitto_pub -t all/neopixel/twinkle -m  '{"operation":"twinkle","duration":2000, "delay": 3}'
```

delay and duration are in milliseconds

```
{
  "operation":"twinkle",
  "duration":2000,
  "delay": 3
}
```

## build instructions

The Wifi SSID and password needs to be set in the `homie/config.json` file, which needs to be "Upload Filesystem Image'd"

example config.json:

```

{
    "name": "test",
    "device_stats_interval": 60,
    "wifi": {
      "ssid": "Folly",
      "password": "passphrase"
    },
    "mqtt": {
      "host": "mqtt",
      "port": 1883
    },
    "ota": {
      "enabled": true
    }
  }
```