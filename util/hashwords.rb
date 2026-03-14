#!/usr/bin/env ruby

def hash_djb2(str)
  hash = 5381
  str.split('').each do |c|
    hash = ((hash << 5) + hash) + c.ord;
  end
  return hash
end

def die_with_usage(err=nil)
  name = File.basename($0)
  puts "Usage: #{name} <path> [mod]"
  if err != nil
    puts "ERROR: #{err}"
    exit 1
  end
  exit 0
end

path = ARGV[0]
unless path
  die_with_usage()
end
unless File.exist?(path)
  die_with_usage("no such file at: #{path}")
end
puts "path=#{path}"

mod = ARGV[1] ? ARGV[1].to_i : 4
puts "mod=#{mod}"

wordmap = Hash.new {|h,k| h[k] = [] }

File.open(path) do |f|

  while(line=f.gets)
    word = line.chomp
    next unless word =~ /^[a-z]+$/
    # puts "|#{word}| => #{hash_djb2(word) % mod}"
    index = hash_djb2(word) % mod
    wordmap[index] << word
  end

end

indices = wordmap.keys.sort
indices.each do |index|
  list = wordmap[index]
  puts "---"
  # puts "index=#{index}"
  next if list.size == 0
  list.sort.each do |w|
    # puts "i=#{index} | #{w} | #{hash_djb2(w)}"
    printf("%4d | %-20s | %d\n", index, w, hash_djb2(w))
  end
end

