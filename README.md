# PEP_HF
Párhuzamos és eseményvezérelt programozás beágyazott rendszerekben (VIMIAC08) tárgy házi feladata. A program egyes részei Linuxon és EFM32 Giant Gecko Starter Kit (EFM32GG-STK3700) mikrokontrolleren futnak, a két rendszer soros porton keresztül kommunikál egymással. 
## Futtatás Linuxon
A Linuxra írt rész fordítása CMake-kel ajánlott, a futtatandó script meghívása során ``` -d ``` csatolóval át kell adni a mikrokontroller elérési útvonalát:
```
./PEP_HF -d/dev/ttyACM0
```
