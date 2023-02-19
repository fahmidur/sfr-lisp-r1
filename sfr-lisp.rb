#!/usr/bin/env ruby

##
# Prototype implementation.

require 'readline'
require 'pry'

class ParseError < Exception; end
OPERATORS = ['+', '-', '/', '*', '>', '<', '=', '==']

class Env
  def initialize(parent=nil, map={})
    @parent = parent
    @map = map.clone
    #binding.pry
  end
  def [](key)
    if @map.has_key?(key)
      return @map[key]
    elsif @parent
      @parent[key]
    else
      return nil
    end
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
  def find(key)
    return self if @map.has_key?(key)
    return @parent.find(key) if @parent
    return nil
  end
  def pp
    puts "--- { env { ---"
    @map.each do |k, v|
      printf("%20s => %s\n", k.to_s, v.to_s)
    end
    puts "--- } env } ---"
    return nil
  end
end

$env_global = Env.new(nil, {
  :+           => lambda {|a, b| a + b },
  :-           => lambda {|a, b| a - b },
  :*           => lambda {|a, b| a * b },
  :/           => lambda {|a, b| a / b },
  :>           => lambda {|a, b| a > b },
  :<           => lambda {|a, b| a < b },
  :>=          => lambda {|a, b| a >= b },
  :<=          => lambda {|a, b| a <= b },
  :begin       => lambda {|*args| args[-1] },
  :print       => lambda {|*args| print(*args) },
  :println     => lambda {|*args| puts(*args)  },
  :puts        => lambda {|*args| puts(*args)  },
  :display     => lambda {|*args| print(*args) },
  :displayln   => lambda {|*args| puts(*args)  },
  :newline     => lambda { puts }
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

class LispNumber
  attr_reader :rep
  def initialize(rep)
    unless rep.is_a?(Float)
      raise ArgumentError("Expecting rep of class Float")
    end
    @rep = rep
    #puts "LispNumber. @rep=#{@rep}"
  end
  def to_s
    if (@rep*10)%10 == 0
      return @rep.to_i.to_s
    else
      return @rep.to_s
    end
  end
  def method_missing(m, *args)
    resp = @rep.send(m, *args)
    if resp.is_a?(Float)
      return LispNumber.new(resp)
    end
    return resp
  end
end

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
  if token.is_a?(Symbol) || token.is_a?(LispString) || token.is_a?(LispNumber)
    return token
  elsif token =~ /^\-?[0-9\.]+$/
    return LispNumber.new(token.to_f)
  elsif token =~ /^[a-z][a-z0-9\-\_\!]*$/i || OPERATORS.member?(token)
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
  STDERR.puts "--- { tokens { ---"
  STDERR.puts tokens.inspect
  STDERR.puts "--- } tokens } ---"
  program = tokens_read(tokens)
  STDERR.puts "--- { program parsed { ---"
  STDERR.puts program.inspect
  STDERR.puts "--- } program parsed } ---"
  return program
end

class LispProcedure
  attr_reader :params
  attr_reader :body
  attr_reader :env
  @@count = 0
  def initialize(params, body, env)
    @params = params
    @body = body
    @env = env
    @id = (@@count += 1)
    #puts "LispProcedure(#{@id}). params=#{@params}"
  end
  def call(*args)
    #puts "LispProcedure(#{@id}). call. args=#{args}"
    lisp_eval(@body, Env.new(@env, @params.zip(args).to_h))
  end
end

def lisp_eval(x, env=$env_global)
  #puts "lisp_eval. x=#{x}"
  if x.is_a?(Symbol)
    return env[x]
  elsif x.is_a?(Numeric) || x.is_a?(LispNumber)
    return x
  elsif x.is_a?(LispString)
    return x
  elsif x.is_a?(LispProcedure)
    return x
  end
  op, *args = x
  if op == :define
    symbol, exp = *args
    env[symbol] = lisp_eval(exp, env)
  elsif op == :if
    test, conseq, alt = *args
    exp = lisp_eval(test, env) ? conseq : alt
    lisp_eval(exp, env)
  elsif op == :lambda
    params, body = *args
    LispProcedure.new(params, body, env)
  elsif op == :set!
    symbol, exp = *args
    tenv = env.find(symbol)
    if tenv
      tenv[symbol] = lisp_eval(exp, env)
    end
  else
    # procedure call
    func = lisp_eval(op, env)
    vals = args.map {|e| lisp_eval(e, env) }
    if func.respond_to?(:call)
      func.call(*vals)
    else
      func
    end
  end
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
  STDERR.puts "--- { program source { ---"
  STDERR.puts source
  STDERR.puts "--- } program source } ---"
  program = lisp_parse(source)
  STDERR.puts lisp_eval(program)
end

if ARGV.size == 0
  repl()
elsif ARGV.size == 1
  lisp_eval_file(ARGV[0])
else
  puts "ERROR: invalid number of arguments"
  exit 1
end

