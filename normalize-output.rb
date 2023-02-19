#!/usr/bin/env ruby

while(line=gets)
  line.gsub!(/#<LispProcedure.*\>/, '#<LispProcedure>')
  line.gsub!(/#<procedure.*\>/, '#<LispProcedure>')
  print line
end
