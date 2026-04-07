Esse projeto está vinculado à disciplina IF763 ALGEBRA LINEAR AVANÇADA COMPUTACAO - CIn/UFPE:

Aplicação genérica de comparação entre análise direta, inversa e relaxada com entradas representando dados de máquinas e transformações arbitrárias.

Pra rodar no linux, precisa colocar as flags de compilação com a localização do python
```bash
g++ grafico.cpp -o grafico -O2 -std=c++17 $(python3-config --includes) -I$(python3 -c "import numpy; print(numpy.get_include())") $(python3-config --embed --ldflags)
```