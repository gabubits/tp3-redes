import random, string, os, sys

if not len(sys.argv):
    print("Argumentos invalidos!") 

else:

    # Cria a pasta onde serao salvos os arquivos gerados
    try:
        os.mkdir("arquivos")
    except:
        pass

    caminho = os.getcwd()

    # Gera arquivos de texto de tamanhos diferentes, passados
    # por argumentos na chamada na linha de comando.
    for i in sys.argv[1:]:
        
        tam_mb = int(i)
        tam_bytes = tam_mb * 1024 * 1024

        with open(f'{caminho}/arquivos/arquivo{tam_mb}MB.txt', 'w') as arq:
            # Escreve dados no arquivo até atingir o tamanho desejado
            while arq.tell() < tam_bytes:
                arq.write(''.join(random.choice(string.ascii_letters + string.digits)) * 
                        min(1024, tam_bytes - arq.tell()))

        print(f"Arquivo gerado com sucesso: 'arquivo{tam_mb}MB.txt'")
