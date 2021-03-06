# artnet-to-serial

![Coloured LED strips connected to power suply](docs/ledstrips.jpg "artnet-to-serial usage example with LED strips")

## Required commands to receive Art-Net packets from remote hosts
```bash
# Linux
sudo ifconfig eth0:0 2.168.1.100 broadcast 2.255.255.255 up

# Windows
netsh interface ipv4 add address "Ethernet" 2.168.1.4 255.0.0.0
```

## Block diagram
![Block diagram: Lighting control Software to artnet-to-serial to Arduino to DMX board and TLC5940 board. DMX board to DMX lights. TLC5940 to LED strip controllers to LED strips.](docs/diagram.png?raw=true "artnet-to-serial block diagram")

## Screenshot
![screenshot of the program](screenshot.png)

## Windows Development Dependencies
- Microsoft Visual Studio 2017
- [Boost binaries for Windows](https://www.boost.org/users/download/)
  - [Installation Instructions](https://www.boost.org/doc/libs/1_67_0/more/getting_started/windows.html#build-from-the-visual-studio-ide)
- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

## LED tape connector pinout
```
1 - +12V
2 - Red / Single Colour
3 - Green
4 - Cool White / Other Colour
5 - Blue
6 - White / Warm White
```

- LED tape: **Male** connector
- Power: **Female** connector
- Extension cord: Male to Female
