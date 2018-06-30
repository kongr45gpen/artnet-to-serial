# artnet-to-serial

## Interface configuration commands

```bash
sudo ifconfig eth0:0 2.168.1.100 broadcast 2.255.255.255 up
netsh interface ipv4 add address "Ethernet" 2.168.1.4 255.0.0.0
```

## Windows Development Dependencies
- Microsoft Visual Studio 2017
- [Boost binaries for Windows](https://www.boost.org/users/download/)
  - [Installation Instructions](https://www.boost.org/doc/libs/1_67_0/more/getting_started/windows.html#build-from-the-visual-studio-ide)
- [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
