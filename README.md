# Cacique HTTP Server

O melhor servidor Web da minha rua.

## Rodando o projeto

Compilar:
```sh
gcc http.c -o http -std=gnu99 -Wall
```

Rodar o server (sudo é necessário, porta 80 :smile:) :

```sh
sudo ./http
```

Testar com `curl`: `./test.sh`

No browser: `http://localhost`

Cheque o `cacique.log` para verificar requisições! Só funciona para duas URLs.

---
Daniel Corrêa <dnlcorrea@gmail.com>
