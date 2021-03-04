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

1 create a `src/secrets.h` containing `#define SECRET_SSID` and `#define SECRET_PASSWORD`