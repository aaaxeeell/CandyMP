# Práctica de Programación 2 - UAB, Grado Ingeniería Informática, 107890

# Ayuda primera ejecución

Instrucciones para una primera prueba de humo. Sistemas testados:

* Linux
* Windows
* MacOS

Todas las rutas relativas a la carpeta raíz del proyecto.

## Windows/Mac

1. Instalar Visual Studio community.
2. Abrir el fichero `visual_studio/0.\ Windows\ Desktop/P2_Lab.sln` con Visual Studio.
5. Pulsar el boton play de la barra superior (también disponible el script `run.bat` una vez compilado el proyecto).

El ejecutable se genera en `visual_studio\0. Windows Desktop\Program\P2_Lab.exe`.
Se recomiendo usar PowerShell para ejecutarlo y usar la entrada/salida por línea de comandos (`std::cin`/`std::cout`).

## Ubuntu/Debian

1. `sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-gfx-dev libpng-dev libfreetype-dev`
2. make clean; make
3. `bash run.sh`

El binario se genera en `./build/p2_project` y puede ejecutarse directamente.

## Fedora

1. `sudo dnf install gcc gcc-c++ make cmake libsdl2-devel libsdl2-ttf-devel libsdl2-mixer-devel libsdl2-image-devel libsdl2-gfx-devel libpng-devel libfreetype-devel`
2. make clean; make
3. `bash run.sh`

El binario se genera en `./build/p2_project` y puede ejecutarse directamente.
