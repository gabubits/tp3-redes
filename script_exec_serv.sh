# Script automatico que executara,
# para cada arquivo, uma conexao do servidor de
# variando os tamanhos de bytes do buffer e,
# para cada valor de tam_buffer, executando 5 vezes
# para analises posteriores.
for filename in arquivos/*.txt; do
    for tam_buffer in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536; do
        for counter in 1 2 3 4 5; do
            ./servidor 8080 $tam_buffer;
            sleep 2
        done;
    done;
done
