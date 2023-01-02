#include <cstdio>
#include <cstring>
#include <string>
#include <istream>
#include <iostream>

class Token {
  public:
  Token(std::string istr):  
    str(istr) 
  {
    
  }
  private:
  std::string str;
};

class TokenParenBeg : Token {
};

class TokenParenEnd : Token {
};

class TokenString : Token {
};

class TokenStringDouble : TokenString {
};

class TokenStringSingle : TokenString {
};

class Tokenizer {
  public:
  Tokenizer(std::istream& is):
    istream(istream)
  {
    std::cout << "Tokenizer. constructor" << std::endl;
  }
  private:
  std::istream& istream;
};

class Lexerizer {
  public:
  Lexerizer() {
  }
  private:
  LexNode root;
};

const char* VER = "0.1.0";

void print_banner() {
  printf("-----------------------------------------------\n");
  printf("---              sfr-lisp v%s\n", VER);
  printf("-----------------------------------------------\n");
  printf("\n");
}

void print_usage(const char* progname) {
  print_banner();
  printf("Usage: %s [OPTIONS]\n", progname);
  printf("  -h,--help         Print this help page.\n");
  printf("  -v,--version      Print version.\n");
  printf("\n\n");
}

void repl() {
  Tokenizer tok(std::cin);
  Lexerizer lex;
  std::string line;
  while(!std::cin.eof() && std::cin.good()) {
    printf("> ");
    std::getline(std::cin, line);
    if(line == ".exit") {
      printf("goodbye!\n");
      return;
    }
  }
}

int strcmp_onedash(const char* a, const char* b, const char* c) {
  char* buf = (char*) malloc(strlen(a)+1);
  size_t buf_pos = 0;
  unsigned int i;
  int res;
  memset(buf, '\0', sizeof buf);
  size_t a_len = strlen(a);
  char leading = 1;
  unsigned int leading_dash_count = 0;
  for(i = 0; i < a_len; i++) {
    if(leading && a[i] == '-') { 
      if(leading_dash_count == 0) {
        buf[buf_pos++] = a[i];
      }
      leading_dash_count++;
      continue; 
    }
    leading = 0;
    buf[buf_pos++] = a[i];
  }
  //printf("dbg. buf=%s b=%s c=%s\n", buf, b, c);
  res = (
    strcmp(buf, b) == 0 ||
    strcmp(buf, c) == 0 
  ) ? 0 : -1;
  free(buf);
  return res;
}


int main(int argc, char** argv) {
  if(argc == 2) {
    if(strcmp_onedash(argv[1], "-help", "-h") == 0) {
      print_usage(argv[0]);
      return 0;
    }
    else
    if(strcmp_onedash(argv[1], "-version", "-v") == 0) {
      printf("%s\n", VER);
    }
    else {
      printf("ERROR: invalid arg\n");
      return 1;
    }
  }
  else 
  if(argc == 1) {
    // REPL
    print_banner();
    repl();
  }
  else {
    printf("ERROR: invalid args\n");
    return 1;
  }
  return 0;
}
