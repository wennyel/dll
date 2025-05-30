@echo off
echo === Limpeza de Arquivos Temporários ===

REM Remover diretórios de build
if exist build rmdir /s /q build
if exist Debug rmdir /s /q Debug
if exist Release rmdir /s /q Release
if exist x64 rmdir /s /q x64
if exist x86 rmdir /s /q x86
if exist .vs rmdir /s /q .vs

REM Remover arquivos temporários
del /q *.obj *.pdb *.ilk *.exp *.user *.suo *.sdf *.opensdf *.VC.db *.VC.opendb 2>nul

echo Limpeza concluída!
pause
