$SDL2_DIR = ""

md -Force -Path bin > $null

c++ -I"`"$($SDL2_DIR)\include\SDL2`"" `
    -I"`"$($SDL2_DIR)\include`"" `
    "main.cpp" `
    -o "bin/main.exe" `
    -L"`"$($SDL2_DIR)\lib\x64`"" `
    -m64 -lSDL2main -lSDL2 -lopengl32