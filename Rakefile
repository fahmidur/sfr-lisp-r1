require 'find'
require 'fileutils'

build_dir = "./build"
cc = "clang"
cflags = "-g"

task :efiles do
  Find.find('.') do |path|
    # Ignore hidden directories
    if path != "." && File.basename(path).start_with?(".")
      Find.prune
      next
    end
    if !File.directory?(path) && File.executable?(path)
      puts path
    end
  end
end

task :clean do
  puts "clean. cleaning..."
  FileUtils.rm_rf(build_dir)
  FileUtils.mkdir_p(build_dir)
end

# Compile object Atom.o
file "build/Atom.o" =>  ["Atom.c", "Atom.h"] do
  sh "#{cc} #{cflags} -c -o #{build_dir}/Atom.o Atom.c"
end

file "build/Tokenizer.o" => ["Tokenizer.c", "Tokenizer.h"] do
  sh "#{cc} #{cflags} -c -o #{build_dir}/Tokenizer.o Tokenizer.c"
end
