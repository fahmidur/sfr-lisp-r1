#!/usr/bin/env ruby

##
# Prototype implementation.

require 'readline'

class ParseError < Exception; end
OPERATORS = ['+', '-', '/', '*', '>', '<', '=', '==']

class Env
  def initialize(parent=nil, map={})
    @parent = parent
    @map = map.clone
  end
  def [](key)
    return @map[key]
  end
  def []=(key, val)
    return @map[key.to_sym] = val
  end
  def to_s
    return @map.to_s
  end
  def inspect
    return @map.inspect
  end
end

$env_global = Env.new(nil, {
  :+        => lambda {|a, b| a + b },
  :-        => lambda {|a, b| a - b },
  :*        => lambda {|a, b| a * b },
  :/        => lambda {|a, b| a / b },
  :>        => lambda {|a, b| a > b },
  :<        => lambda {|a, b| a < b },
  :>=       => lambda {|a, b| a >= b },
  :<=       => lambda {|a, b| a <= b },
  :print    => lambda {|*args| print(*args);puts },
  :begin    => lambda {|*args| args[-1] },
})

class TokenSeg
  attr_reader :label
  def initialize(label)
    @label = label
    @arr = []
  end
  def size
    @arr.size
  end
  def push(ch)
    @arr.push(ch)
  end
  def to_s
    @arr.join
  end
end

class LispString < String; end;

def tokenize(str)
  segments_arr = []
  instring = false
  segment = TokenSeg.new(:normal)
  str.split('').each do |ch|
    if !instring && ch == '"'
      instring = true
      segments_arr << segment if segment.size > 0
      segment = TokenSeg.new(:string)
    elsif instring
      if ch == '"'
        instring = false
        segments_arr << segment
        segment = TokenSeg.new(:normal)
        next
      end
      segment.push(ch)
    else
      segment.push(ch)
    end
  end
  segments_arr << segment if segment.size > 0
  tokens = []
  segments_arr.each do |seg|
    if seg.label == :normal
      tar = seg.to_s.gsub('(', ' ( ').gsub(')', ' ) ').
        strip.
        split(/\s+/)
      tokens.concat(tar)
    elsif seg.label == :string
      tokens << LispString.new(seg.to_s)
    else
      raise "Invalid Segment Type"
    end
  end
  return tokens
end

def token_to_atom(token)
  if token.is_a?(LispString)
    return token
  elsif token =~ /^[0-9\.]+$/
    return token.to_f
  elsif token =~ /^[a-z][a-z0-9]*$/i || OPERATORS.member?(token)
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
    tokens.shift
    return toklist
  elsif token == ')'
    raise ParseError.new("unexpected ')'")
  else
    token_to_atom(token)
  end
end

def lisp_parse(str)
  tokens = tokenize(str)
  puts "--- { tokens { ---"
  p tokens
  puts "--- } tokens } ---"
  program = tokens_read(tokens)
  puts "--- { program parsed { ---"
  p program
  puts "--- } program parsed } ---"
  return program
end

def lisp_eval(x, env=$env_global, depth=0)
  #prefix = '  '*depth;
  #puts "#{prefix}eval. x=#{x}"
  if x.is_a?(Symbol)
    return env[x]
  elsif x.is_a?(Numeric)
    return x
  elsif x.is_a?(LispString)
    return x
  end
  op, *args = x
  if op == :define
    symbol, exp = *args
    #puts "#{prefix}eval. define. symbol=#{symbol} exp=#{exp}"
    return env[symbol] = lisp_eval(exp, env, depth+1)
  end
  if op == :if
    test, conseq, alt = *args
    exp = lisp_eval(test, env) ? conseq : alt
    return lisp_eval(exp, env)
  end
  # procedure call
  #puts "#{prefix}eval. proc-call. op=#{op} args=#{args}"
  func = lisp_eval(op, env, depth+1)
  #puts "#{prefix}eval. proc-call. func=#{func}"
  vals = args.map {|e| lisp_eval(e, env, depth+1) }
  return func.call(*vals)
end

def repl
  while(true)
    #print "> "
    #line = gets.chomp
    line = Readline.readline('> ', true)
    if line == ".exit" || line == 'exit'
      break
    end
    program = lisp_parse(line)
    p program
    puts "---"
    puts lisp_eval(program)
  end
end

def lisp_eval_file(path)
  unless File.exist?(path)
    puts "ERROR: no such file at #{path}"
    exit 1
  end
  lines = []
  File.open(path) do |f|
    f.each_line do |line|
      if line =~ /^\s*#/
        next
      end
      lines << line.chomp
    end
  end
  source = lines.join("\n")
  puts "--- { program source { ---"
  puts source
  puts "--- } program source } ---"
  program = lisp_parse(source)
  puts lisp_eval(program)
end

if ARGV.size == 0
  repl()
elsif ARGV.size == 1
  lisp_eval_file(ARGV[0])
else
  puts "ERROR: invalid number of arguments"
  exit 1
end

