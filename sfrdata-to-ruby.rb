#!/usr/bin/env ruby

require 'pry'

def List(*args)
  args
end

def String(x)
  x.to_s
end

def Number(x)
  x.to_f
end

def Symbol(x)
  x.to_sym
end

binding.pry
