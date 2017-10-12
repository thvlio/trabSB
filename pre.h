/*      PRE.H: funções relativas ao preprocessamento        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void preReadLine (std::string&, std::ifstream&, std::vector<Label>&);
void appendNextLine (std::string&, std::stringstream&, std::ifstream&, std::vector<Label>&);
void equCommand (std::string&, std::stringstream&, std::vector<Label>&, std::string&);
void ifCommand (std::string&, std::stringstream&, std::ifstream&);
void preParser (std::string&, std::ifstream&, std::vector<Label>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
preReadLine: le uma linha do arquivo na etapa de preprocessamento e passa tudo para caixa alta, ignora comentários e ignora espaços em branco no começo e no final da linha. depois, procura por rotulos numa linha e substitui pela definicao se encontrar
entrada: stream do arquivo de entrada e lista de rotulos definidos
saida: nenhuma (string com a linha lida e alterada dada por referência)
*/
void preReadLine (std::string &line, std::ifstream &asmFile, std::vector<Label> &labelList)  {
    
    // le uma linha do arquivo
    getline(asmFile, line);
    
    // transforma a string p caixa alta
    std::transform (line.begin(), line.end(), line.begin(), ::toupper);
    
    // ignora os comentarios, se houver algum (e retira os espaços em branco)
    std::stringstream lineStream (line);
    getline(lineStream, line, ';');
    
    // retira os espaços em branco, caso existam, no final da linha
    while (line.back() == ' ' || line.back() == '\t' || line.back() == '\n' || line.back() == '\v' || line.back() == '\f' || line.back() == '\r')
        line.pop_back();
    
    // retira os espaços em branco, caso existam, no começo da linha
    while (line.front() == ' ' || line.front() == '\t' || line.front() == '\n' || line.front() == '\v' || line.front() == '\f' || line.front() == '\r')
        line = line.substr(1);
    
    // itera na lista de rotulos definidos procurando por um desses rotulos na linha
    for (int i = 0; i < labelList.size(); ++i) {
        
        // procura pelo nome do rotulo na linha
        std::size_t pos = line.find(labelList[i].name);
        
        // se encontrar em alguma posicao, substitui pelo numero associado ao rotulo
        if (pos != std::string::npos) {
            line.replace (pos, labelList[i].name.size(), std::to_string(labelList[i].value));
        }
    }
    
}



/*
appendNextLine: le a proxima linha, anexa na atual e retorna a linha composta
entrada: a linha atual, a stream da linha atual, a stream do arquivo e a lista de rotulos
saida: nenhuma (linha e stream da linha alteradas por referencia)
*/
void appendNextLine (std::string &line, std::stringstream &lineStream, std::ifstream &asmFile, std::vector<Label> &labelList) {
    
    // le e anexa à linha atual a proxima linha
    std::string nextLine;
    preReadLine (nextLine, asmFile, labelList);
    line = line + " " + nextLine;
    
    // e corrige a stream da linha
    lineStream.str(nextLine); // salva a linha anexada como stream
    lineStream.clear(); // limpa as flags de estado
    
}



/*
equCommand: associa um valor ao rotulo
entrada: linha atual, stream da linha atual, lista de rotulos e nome do rotulo
saida: nenhuma (lista de rotulos alterada por referencia)
*/
void equCommand (std::string &line, std::stringstream &lineStream, std::vector<Label> &labelList, std::string &token) {
    
    // le o valor a ser substituido
    int value;
    lineStream >> value;
    
    token.pop_back(); // apaga o ':'
    Label label (token, value); // cria o rotulo com o valor associado
    labelList.push_back(label); // coloca o rotulo na lista de rotulos definidos
    
    // esvazia a string p nao salvar a linha do equ no codigo
    line.clear();
    
}



/*
ifCommand: se o valor do if for 1, compila a linha abaixo, senao esvazia a linha
entrada: linha atual, stream da linha atual e stream do arquivo de entrada
saida: nenhuma (linha alterada por referencia)
*/
void ifCommand (std::string &line, std::stringstream &lineStream, std::ifstream &asmFile) {
    
    // le o numero seguinte (a busca ja trocou o rotulo por um valor)
    int value;
    lineStream >> value;

    if (value != 1)
        getline (asmFile, line); // le a proxima linha do arquivo (que vai ser descartada logo em seguida)
    line.clear(); // limpa a linha atual, ja que era um if
    
}



/*
preParser: processa uma linha do arquivo fonte
entrada: linha atual, stream do arquivo de entrada e lista de rotulos
saida: nenhuma (linha lida devolvida por referência)
*/
void preParser (std::string &line, std::ifstream &asmFile, std::vector<Label> &labelList) {
    
    // le uma linha, corrige algumas coisas e procura na linha por rotulos que ja tenham sido definidos por equs
    preReadLine (line, asmFile, labelList);
    
    // cria um stream para a leitura de tokens
    std::stringstream lineStream (line);
    
    // pede para o scanner do preprocessamento um token
    std::string token;
    scanner (token, lineStream);
    
    // se o ultimo caracter for ':', entao ta definindo um rotulo
    if (token.back() == ':') {
        
        // pega o token seguinte
        std::string token2;
        scanner (token2, lineStream);
        
        // se token2 estiver vazio, anexa a proxima linha
        if (token2.empty()) {
            appendNextLine (line, lineStream, asmFile, labelList);
            scanner (token2, lineStream); // pega o token correto
        }
        
        // se for equ, salva o valor associado ao rotulo na lista
        if (token2 == "EQU")
            equCommand (line, lineStream, labelList, token);
        
    } else if (token == "IF")
        ifCommand (line, lineStream, asmFile);
}



/*
preProcessFile: faz a passagem de preprocessamento no arquivo, que inclui:
    - passa tudo para caixa alta
    - ignora comentarios
    - avalia EQU e IF
    - (detectar erros)
entrada: nome do arquivo de entrada '.asm'
saida: nome do arquivo de saida '.pre'
*/
void preProcessFile (std::string inFileName, std::string preFileName) {
    
    std::ifstream asmFile (inFileName);
    std::ofstream preFile (preFileName);
    
    std::vector<Label> labelList;

    while (!asmFile.eof()) {
        
        // chama o parser especifico do preprocessamento        
        std::string line;
        preParser(line, asmFile, labelList);
            
        // se a linha nao retornar vazia, copia no arquivo '.pre'        
        if (!line.empty())
            preFile << line << "\n";
    }
    
    asmFile.close();
    preFile.close();
    
    return;
}