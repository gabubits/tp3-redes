# Script que é executado de forma complementar
# ao script do servidor e funciona de forma semelhante.

for filename in arquivos/*.txt; do
    for tam_buffer in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536; do
        for counter in 1 2 3 4 5; do
            ./cliente 127.0.0.1 8080 ${filename:9} $tam_buffer;
            sleep 3
        done;
    done;
done
