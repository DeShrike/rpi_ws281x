# rpi_ws281x

Based on https://github.com/danjperron/rpi_ws281x

See original readme: README_original.md

## Compiling

```console
git clone https://github.com/DeShrike/rpi_ws281x
cd rpi_ws281x
make
sudo ./test1
sudo ./test2
sudo ./test3
```

Before running `make` you might want to change a few settings at the top of the test programs, like `LED_COUNT` and `GPIO_PIN`.

See original README for more instructions.

## Connecting to Raspberry Pi

- Connect the ground wire (usually black) to pin 6.
- Connect the power wire (usually red) to pin 1 (3v3).
- Connect the data wire to pin 12 (GPIO18).
