#!/usr/bin/env ruby

##
# Prototype implementation.

class ParseError < Exception; end
OPERATORS = ['+', '-', '/', '*']

def tokenize(str)
  str.
    gsub('(', ' ( ').
    gsub(')', ' ) ').
    strip.
    split(/\s+/)
end

def token_to_atom(token)
  if token =~ /^[0-9\.]+$/
    return token.to_f
  elsif token =~ /^[a-z][a-z0-9]+$/i || OPERATORS.member?(token)
    return token.to_sym
  else
    raise ParseError.new("Invalid atom token: #{token}")
  end
end

def tokens_read(tokens)
  if tokens.size == 0
    raise ParseError.new("unexpected EOF")
  end
  token = tokens.shift
  if token == '('
    toklist = []
    while tokens.first != ')'
      toklist << tokens_read(tokens)
    end
    return toklist
  elsif token == ')'
    raise ParseError.new("unexpected ')'")
  else
    token_to_atom(token)
  end
end

def parse(str)
  tokens = tokenize(str)
  program = tokens_read(tokens)
  return program
end

def repl
  while(true)
    print "> "
    line = gets.chomp
    if line == ".exit"
      break
    end
    program = parse(line)
    p program
  end
end

def lisp_eval_file(path)
  unless File.exist?(path)
    puts "ERROR: no such file at #{path}"
    exit 1
  end
  # TODO
end

if ARGV.size == 0
  repl()
elsif ARGV.size == 1
  lisp_eval_file(ARGV[0])
else
  puts "ERROR: invalid number of arguments"
  exit 1
end

