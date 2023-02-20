#!/usr/bin/env ruby

# The purpose of this is to normalize the output
# between Racket and sfr-lisp.rb so that we can use this 
# to test behavior against Racket.

while(line=gets)
  line.gsub!(/#<LispProcedure.*\>/, '#<LispProcedure>')
  line.gsub!(/#<procedure.*\>/, '#<LispProcedure>')
  print line
end
