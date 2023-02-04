require 'find'
require 'fileutils'

cc = ENV['CC'] || "clang"
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
  if Dir.exist?("./build")
    FileUtils.rm_rf("./build")
    FileUtils.mkdir_p("./build")
  end
end

task "build" do
  objects = ['Atom', 'Tokenizer']
  objects.each do |basename|
    target = "build/#{basename}.o"
    Rake::Task[target].invoke
  end
  programs = ['nassert_test', 'Tokenizer_test', 'Atom_test']
  programs.each do |name|
    target = "build/#{name}"
    Rake::Task[target].invoke
  end
end

file "build/Atom.o" =>  ["Atom.c", "Atom.h"] do
  sh "#{cc} #{cflags} -c -o build/Atom.o Atom.c"
end

file "build/Tokenizer.o" => ["Tokenizer.c", "Tokenizer.h"] do
  sh "#{cc} #{cflags} -c -o build/Tokenizer.o Tokenizer.c"
end

file "build/nassert_test" => ["nassert.h", "nassert_test.c"] do
  sh "#{cc} #{cflags} -o build/nassert_test nassert_test.c"
end

file "build/Tokenizer_test" => ["Tokenizer_test.c", "build/Tokenizer.o"] do
  sh "#{cc} #{cflags} -o build/Tokenizer_test Tokenizer_test.c build/Tokenizer.o"
end

file "build/Atom_test" => ["Atom_test.c", "build/Atom.o"] do
  sh "#{cc} #{cflags} -o build/Atom_test Atom_test.c build/Atom.o"
end
