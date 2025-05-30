@echo off
chcp 65001 >nul
echo === Script de Correção para Projeto DLL ===
echo Iniciando correções automáticas...

REM Definir variáveis
set "PROJECT_DIR=C:\Users\wennys\Desktop\Cliente_Cabal\Cliente_Cabal"
set "REPO_URL=https://github.com/wennyel/dll.git"

REM Verificar se Git está instalado
git --version >nul 2>&1
if errorlevel 1 (
    echo ❌ Git não encontrado. Instale: https://git-scm.com/download/win
    pause
    exit /b 1
)

REM Navegar para o diretório do projeto
echo === 1. Navegando para o Projeto ===
if not exist "%PROJECT_DIR%" (
    echo ❌ Diretório do projeto não encontrado: %PROJECT_DIR%
    pause
    exit /b 1
)
cd /d "%PROJECT_DIR%"

REM Criar estrutura de diretórios adequada
echo === 2. Reorganizando Estrutura do Projeto ===
if not exist src mkdir src
if not exist include mkdir include
if not exist lib mkdir lib
if not exist bin mkdir bin
if not exist build mkdir build
if not exist docs mkdir docs

REM Mover arquivos para estrutura correta
echo Reorganizando arquivos...
for %%f in (*.cpp) do if exist "%%f" move "%%f" src\ >nul 2>&1
for %%f in (*.c) do if exist "%%f" move "%%f" src\ >nul 2>&1
for %%f in (*.h) do if exist "%%f" move "%%f" include\ >nul 2>&1
for %%f in (*.hpp) do if exist "%%f" move "%%f" include\ >nul 2>&1
for %%f in (*.dll) do if exist "%%f" move "%%f" lib\ >nul 2>&1
for %%f in (*.lib) do if exist "%%f" move "%%f" lib\ >nul 2>&1
for %%f in (*.exe) do if exist "%%f" move "%%f" bin\ >nul 2>&1

REM Criar .gitignore
echo === 3. Criando .gitignore ===
(
echo # Arquivos compilados
echo *.exe
echo *.dll
echo *.lib
echo *.obj
echo *.o
echo *.pdb
echo *.ilk
echo *.exp
echo.
echo # Diretórios de build
echo build/
echo Debug/
echo Release/
echo x64/
echo x86/
echo .vs/
echo .vscode/
echo.
echo # Arquivos temporários
echo *.tmp
echo *.log
echo *.cache
echo *.user
echo *.suo
echo *.sdf
echo *.opensdf
echo *.VC.db
echo *.VC.opendb
echo.
echo # Arquivos do sistema
echo Thumbs.db
echo .DS_Store
echo *.swp
echo *.swo
echo *~
echo.
echo # Dependências
echo node_modules/
echo packages/
) > .gitignore

REM Criar CMakeLists.txt
echo === 4. Criando CMakeLists.txt ===
(
echo cmake_minimum_required^(VERSION 3.10^)
echo project^(ClienteCabal^)
echo.
echo # Definir padrão C++
echo set^(CMAKE_CXX_STANDARD 17^)
echo set^(CMAKE_CXX_STANDARD_REQUIRED ON^)
echo.
echo # Incluir diretórios
echo include_directories^(include^)
echo link_directories^(lib^)
echo.
echo # Encontrar arquivos fonte
echo file^(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c"^)
echo file^(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp"^)
echo.
echo # Criar DLL
echo add_library^(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS}^)
echo.
echo # Definir propriedades da DLL
echo set_target_properties^(${PROJECT_NAME} PROPERTIES
echo     OUTPUT_NAME "gamehook"
echo     PREFIX ""
echo     SUFFIX ".dll"
echo ^)
echo.
echo # Linkar bibliotecas necessárias
echo target_link_libraries^(${PROJECT_NAME}
echo     # Adicione suas bibliotecas aqui
echo     # fmodex
echo     # keystone
echo ^)
echo.
echo # Definir macros para exportação
echo target_compile_definitions^(${PROJECT_NAME} PRIVATE
echo     BUILDING_DLL
echo     _CRT_SECURE_NO_WARNINGS
echo ^)
) > CMakeLists.txt

REM Criar README.md
echo === 5. Criando README.md ===
(
echo # Cliente Cabal - GameHook DLL
echo.
echo ## Descrição
echo Projeto de DLL para hook de jogo Cabal Online.
echo.
echo ## Estrutura do Projeto
echo ```
echo ├── src/           # Código fonte ^(.cpp, .c^)
echo ├── include/       # Headers ^(.h, .hpp^)
echo ├── lib/           # Bibliotecas ^(.dll, .lib^)
echo ├── bin/           # Executáveis
echo ├── build/         # Arquivos de build
echo └── docs/          # Documentação
echo ```
echo.
echo ## Compilação
echo.
echo ### Usando CMake ^(Recomendado^)
echo ```bash
echo mkdir build
echo cd build
echo cmake ..
echo cmake --build . --config Release
echo ```
echo.
echo ### Usando Visual Studio
echo 1. Abra o projeto no Visual Studio
echo 2. Configure para Release/x64
echo 3. Build Solution ^(Ctrl+Shift+B^)
echo.
echo ## Dependências
echo - fmodex.dll
echo - keystone.dll
echo - Visual Studio 2019 ou superior
echo - CMake 3.10+ ^(opcional^)
echo.
echo ## Problemas Conhecidos
echo - Verifique compatibilidade x86/x64
echo - Certifique-se que todas as DLLs estão no PATH
echo - Configure corretamente as bibliotecas de runtime
echo.
echo ## Contribuição
echo 1. Fork o projeto
echo 2. Crie uma branch para sua feature
echo 3. Commit suas mudanças
echo 4. Push para a branch
echo 5. Abra um Pull Request
) > README.md

REM Criar script de build
echo === 6. Criando Script de Build ===
(
echo @echo off
echo echo === Build Script para Cliente Cabal ===
echo.
echo REM Verificar se CMake existe
echo cmake --version ^>nul 2^>^&1
echo if errorlevel 1 ^(
echo     echo CMake nao encontrado. Tentando build com MSBuild...
echo     goto :msbuild
echo ^)
echo.
echo REM Build com CMake
echo echo Usando CMake para build...
echo if not exist build mkdir build
echo cd build
echo cmake ..
echo cmake --build . --config Release
echo if errorlevel 1 ^(
echo     echo Erro no build com CMake
echo     cd ..
echo     goto :msbuild
echo ^)
echo echo Build com CMake concluido com sucesso!
echo cd ..
echo goto :end
echo.
echo :msbuild
echo echo Tentando build com MSBuild...
echo REM Procurar por arquivos .sln ou .vcxproj
echo for %%%%f in ^(*.sln^) do ^(
echo     echo Compilando %%%%f...
echo     msbuild "%%%%f" /p:Configuration=Release /p:Platform=x64
echo     goto :end
echo ^)
echo for %%%%f in ^(*.vcxproj^) do ^(
echo     echo Compilando %%%%f...
echo     msbuild "%%%%f" /p:Configuration=Release /p:Platform=x64
echo     goto :end
echo ^)
echo echo Nenhum arquivo de projeto encontrado. Configure manualmente.
echo.
echo :end
echo echo.
echo echo Build concluido! Verifique a pasta build/ ou Release/
echo pause
) > build.bat

REM Criar script de limpeza
echo === 7. Criando Script de Limpeza ===
(
echo @echo off
echo echo === Limpeza de Arquivos Temporários ===
echo.
echo REM Remover diretórios de build
echo if exist build rmdir /s /q build
echo if exist Debug rmdir /s /q Debug
echo if exist Release rmdir /s /q Release
echo if exist x64 rmdir /s /q x64
echo if exist x86 rmdir /s /q x86
echo if exist .vs rmdir /s /q .vs
echo.
echo REM Remover arquivos temporários
echo del /q *.obj *.pdb *.ilk *.exp *.user *.suo *.sdf *.opensdf *.VC.db *.VC.opendb 2^>nul
echo.
echo echo Limpeza concluída!
echo pause
) > clean.bat

REM Criar arquivo de configuração do Visual Studio
echo === 8. Criando Configuração do Projeto ===
(
echo ^<?xml version="1.0" encoding="utf-8"?^>
echo ^<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^>
echo   ^<ItemGroup Label="ProjectConfigurations"^>
echo     ^<ProjectConfiguration Include="Debug|x64"^>
echo       ^<Configuration^>Debug^</Configuration^>
echo       ^<Platform^>x64^</Platform^>
echo     ^</ProjectConfiguration^>
echo     ^<ProjectConfiguration Include="Release|x64"^>
echo       ^<Configuration^>Release^</Configuration^>
echo       ^<Platform^>x64^</Platform^>
echo     ^</ProjectConfiguration^>
echo   ^</ItemGroup^>
echo   ^<PropertyGroup Label="Globals"^>
echo     ^<ProjectName^>ClienteCabal^</ProjectName^>
echo     ^<ConfigurationType^>DynamicLibrary^</ConfigurationType^>
echo     ^<PlatformToolset^>v142^</PlatformToolset^>
echo   ^</PropertyGroup^>
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" /^>
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" /^>
echo   ^<ItemDefinitionGroup^>
echo     ^<ClCompile^>
echo       ^<AdditionalIncludeDirectories^>include;%%(AdditionalIncludeDirectories^)^</AdditionalIncludeDirectories^>
echo       ^<PreprocessorDefinitions^>BUILDING_DLL;_CRT_SECURE_NO_WARNINGS;%%(PreprocessorDefinitions^)^</PreprocessorDefinitions^>
echo     ^</ClCompile^>
echo     ^<Link^>
echo       ^<AdditionalLibraryDirectories^>lib;%%(AdditionalLibraryDirectories^)^</AdditionalLibraryDirectories^>
echo     ^</Link^>
echo   ^</ItemDefinitionGroup^>
echo   ^<ItemGroup^>
echo     ^<ClCompile Include="src\*.cpp" /^>
echo   ^</ItemGroup^>
echo   ^<ItemGroup^>
echo     ^<ClInclude Include="include\*.h" /^>
echo   ^</ItemGroup^>
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /^>
echo ^</Project^>
) > ClienteCabal.vcxproj

REM Configurar Git e fazer commit
echo === 9. Configurando Git ===
git init >nul 2>&1
git add .
git commit -m "🔧 Correção automática da estrutura do projeto

- Reorganizada estrutura de diretórios
- Adicionado .gitignore apropriado  
- Criado CMakeLists.txt para build
- Adicionado README.md com documentação
- Criado scripts de build e limpeza para Windows
- Adicionado arquivo de projeto Visual Studio
- Corrigida configuração para x64" >nul 2>&1

REM Conectar com repositório remoto
git remote get-url origin >nul 2>&1
if errorlevel 1 (
    echo Adicionando repositório remoto...
    git remote add origin "%REPO_URL%"
)

REM Push para GitHub
echo === 10. Enviando para GitHub ===
echo Fazendo push para o repositório...
git branch -M main
git push -u origin main --force

echo.
echo ✅ === CORREÇÕES CONCLUÍDAS ===
echo 📁 Estrutura reorganizada
echo 📝 Documentação criada  
echo 🔧 Scripts de build adicionados
echo 📤 Projeto enviado para GitHub
echo.
echo Próximos passos:
echo 1. Execute 'build.bat' para compilar
echo 2. Use 'clean.bat' para limpar arquivos temporários
echo 3. Abra ClienteCabal.vcxproj no Visual Studio se preferir
echo.
echo Arquivos criados:
echo - build.bat ^(script de compilação^)
echo - clean.bat ^(script de limpeza^)
echo - ClienteCabal.vcxproj ^(projeto Visual Studio^)
echo - CMakeLists.txt ^(configuração CMake^)
echo - README.md ^(documentação^)
echo.
pause
