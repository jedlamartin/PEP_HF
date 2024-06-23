# PEP_HF
Párhuzamos és eseményvezérelt programozás beágyazott rendszerekben (VIMIAC08) tárgy házi feladata. A program egyes részei Linuxon és EFM32 Giant Gecko Starter Kit (EFM32GG-STK3700) mikrokontrolleren futnak, a két rendszer soros porton keresztül kommunikál egymással. 
## Futtatás Linuxon
A Linuxra írt rész fordítása CMake-kel ajánlott, a futtatandó script meghívása során ``` -d ``` csatolóval át kell adni a mikrokontroller elérési útvonalát:
```
./PEP_HF -d/dev/ttyACM0
```
A játékból való kilépés egy ``` x ``` karakter konzolba való bevitele után történik meg. Ezután egy játékosnevet kér a program, hogy a legmagasabb elért pontot menteni tudja.

## Futtatás a mikrokontrolleren
A minkrokontrolleren a játék indítása az egyik nyomógomb lenyomásával indul el, és az irányítás is a nyomógombok segítségével történik. A bal nyomógomb hatására az űrhajó balra 90°-ot, a jobb nyomógomb hatására pedig jobbra 90°-ot fordul. Ha a játéknak vége van, akkor az újraindítás valamelyik nyomógomb újbóli megnyomásával történik.

![image](https://github.com/jedlamartin/PEP_HF/assets/127199529/6f3b8022-f34d-4131-812b-c7e556524db6)
