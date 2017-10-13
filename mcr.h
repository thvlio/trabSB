/*      PRE.H: funções relativas ao preprocessamento        */



/*      DECLARAÇÕES DAS FUNÇÕES      */
void createMacro (std::string&, std::ifstream&, std::string&, std::vector<Macro>&, int&);
void mcrSearchAndReplace (std::string&, std::string&, std::vector<Macro>&, int&);
void mcrParser (std::string&, std::ifstream&, std::vector<Macro>&, int&, int&);
int expandMacros (std::string, std::string, std::vector<int>&);



/*      DEFINIÇÕES DAS FUNÇÕES      */

/*
createMacro: le a definicao de uma macro e guarda na lista de macros
entrada: linha atual, stream do arquivo .pre, nome da macro, lista de macros e contador de linha
saida: nenhuma (lista de macros e contador de linhas alterados por referencia)
*/
void createMacro (std::string &line, std::ifstream &preFile, std::string &token, std::vector<Macro> &macroList, int &lineCounter) {
    
    // cria uma string p guardar a definicao da macro
    std::string definition;
    
    // indica se achou end indicando o final da macro
    int reachedEnd = 0;
    
    // linha inicial da macro
    int initLine = lineCounter+1;
    int numLines = 0;
    
    // laco que vai armazenando todas as linhas ate o final da macro
    while (!reachedEnd) {
        
        // a proxima linha eh lida
        std::string auxLine;
        getline (preFile, auxLine);
        lineCounter++;
        
        if (auxLine != "END") {
            // se nao for o final, anexa na definicao
            definition = definition + auxLine + '\n';
            numLines++;
        } else {
            // se for o final, ajusta a flag e tira o '\n' do final da definicao
            reachedEnd = 1;
            definition.pop_back();
        }
    }
    
    // armazena a macro na lista de macros
    token.pop_back();
    Macro macro (token, definition, initLine, numLines);
    macroList.push_back(macro);
    
    // nao escreve a linha que define o rotulo da macro
    line.clear();
    
}



/*
mcrSearchAndReplace: procura o token na lista de macros e substitui a macro no codigo
entrada: linha atual, nome da macro sendo chamada, lista de macros, contador de linhas e flag indicando se uma macro foi chamada
saida: nenhuma (linha atual e flag de macro alterada por referencia)
*/
void mcrSearchAndReplace (std::string &line, std::string &token, std::vector<Macro> &macroList, int &macroCall) {
    
    int found = 0;
    macroCall = -1;
    for (int i = 0; ((i < macroList.size()) && (!found)); ++i) {
        if (token == macroList[i].name) {
            // se encontrar, escreve a definicao no lugar da linha atual
            line = macroList[i].definition;
            found = 1;
            macroCall = i;
        }
    }
}



/*
mcrParser: parser do processamento de macros. procura definicoes e chamadas de macros nas linhas
entrada: linha atual, stream do arquivo .pre, lista de macros, contador de linhas e flag indicando se uma macro foi chamada
saida: nenhuma (linha atual, contador de linhas e flag de macro alterados por referencia)
*/
void mcrParser (std::string &line, std::ifstream &preFile, std::vector<Macro> &macroList, int &lineCounter, int &macroCall) {
    
    // le uma linha do arquivo
    getline(preFile, line);
    
    // cria o stream para a linha e le um token
    std::stringstream lineStream (line);
    std::string token;
    lineStream >> token;
    
    // se o ultimo char for ':', esta definindo um rotulo
    if (token.back() == ':') {
        
        // le o token seguinte
        std::string token2;
        lineStream >> token2;
        
        // se for uma diretiva de macro, cria uma macro nova na lista
        if (token2 == "MACRO")
            createMacro (line, preFile, token, macroList, lineCounter);
    
    // se nao for definicao de rotulo, eh uma linha que pode ou nao estar chamando uma macro
    } else
        mcrSearchAndReplace (line, token, macroList, macroCall);
    
}



/*
expandMacros: faz a passagem para expandir macros no arquivo, que inclui:
    - substitui as macros
    - (detectar erros)
entrada: nome do arquivo de entrada '.pre', nome do arquivo de saida '.mcr' e dicionario de linhas
saida: inteiro representando a ocorrencia de erro
*/
int expandMacros (std::string preFileName, std::string mcrFileName, std::vector<int> &lineDict) {
    
    std::ifstream preFile (preFileName);
    std::ofstream mcrFile (mcrFileName);

    std::vector<Macro> macroList;
    
    int lineCounter = 1;
    int macroCall;
    
    while (!preFile.eof()) {
        
        // chama o parser da passagem de macros
        std::string line;
        mcrParser(line, preFile, macroList, lineCounter, macroCall);
        
        // se a linha nao estiver vazia, copia no arquivo '.mcr'        
        if (!line.empty()) {
            
            mcrFile << line << "\n";
            
            // se for chamada de macro, coloca no dicionario as linhas originais da macro
            if (macroCall > -1) {
                Macro macro = macroList[macroCall];
                for (int i = macro.initLine; i < macro.initLine+macro.numLines; ++i)
                    lineDict.push_back(i);
            } else
                lineDict.push_back(lineCounter);
        }
        
        lineCounter++;
        
    }
    
    preFile.close();
    mcrFile.close();
    
    // futuramente, indicara erros no valor de retorno
    return 0;
}