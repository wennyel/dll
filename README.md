# Cliente Cabal - GameHook DLL

## Descrição
Projeto de DLL para hook de jogo Cabal Online.

## Estrutura do Projeto
```
├── src/           # Código fonte (.cpp, .c)
├── include/       # Headers (.h, .hpp)
├── lib/           # Bibliotecas (.dll, .lib)
├── bin/           # Executáveis
├── build/         # Arquivos de build
└── docs/          # Documentação
```

## Compilação

### Usando CMake (Recomendado)
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Usando Visual Studio
1. Abra o projeto no Visual Studio
2. Configure para Release/x64
3. Build Solution (Ctrl+Shift+B)

## Dependências
- fmodex.dll
- keystone.dll
- Visual Studio 2019 ou superior
- CMake 3.10+ (opcional)

## Problemas Conhecidos
- Verifique compatibilidade x86/x64
- Certifique-se que todas as DLLs estão no PATH
- Configure corretamente as bibliotecas de runtime

## Contribuição
1. Fork o projeto
2. Crie uma branch para sua feature
3. Commit suas mudanças
4. Push para a branch
5. Abra um Pull Request
