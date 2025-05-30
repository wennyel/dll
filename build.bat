@echo off
echo === Build Script para Cliente Cabal ===

REM Verificar se CMake existe
cmake --version >nul 2>&1
if errorlevel 1 (
    echo CMake nao encontrado. Tentando build com MSBuild...
    goto :msbuild
)

REM Build com CMake
echo Usando CMake para build...
if not exist build mkdir build
cd build
cmake ..
cmake --build . --config Release
if errorlevel 1 (
    echo Erro no build com CMake
    cd ..
    goto :msbuild
)
echo Build com CMake concluido com sucesso!
cd ..
goto :end

:msbuild
echo Tentando build com MSBuild...
REM Procurar por arquivos .sln ou .vcxproj
for %%f in (*.sln) do (
    echo Compilando %%f...
    msbuild "%%f" /p:Configuration=Release /p:Platform=x64
    goto :end
)
for %%f in (*.vcxproj) do (
    echo Compilando %%f...
    msbuild "%%f" /p:Configuration=Release /p:Platform=x64
    goto :end
)
echo Nenhum arquivo de projeto encontrado. Configure manualmente.

:end
echo.
echo Build concluido! Verifique a pasta build/ ou Release/
pause
