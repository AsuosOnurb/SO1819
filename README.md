# SO1819 - Projecto de Sistemas Operativos 2018/19

## Construção
* make setup - Prepara a estrutura do projeto
* make \[debug=true\[/false]] - Constrói todos os binários (ma, sv, cv, ag)
* make <ma/sv/cv/ag> \[debug=true\[/false]] - Constrói o binário especificado
* make <ma/sv/cv/ag> run - Constrói (se necessário) o binário especificado e depois executa o mesmo
* make <ma/sv/cv/ag> run-gdb - Constrói (se necessário) o binário especifidado e depois executa o mesmo através do gdb
* make <ma/sv/cv/ag> run-strace - Constrói (se necessário) o binário especificado e depois executa o mesmo através do strace
* make clean - Limpa os ficheiros resultantes de builds anteriormente efetuadas. É necessário depois fazer make setup para restaurar a estrutura do projeto.

## Flags de Construçao
* debug=true\[/false] - Assinala que os binários a construir devem ser construídos com informações de debug (i.e., --ggdb); default: true
