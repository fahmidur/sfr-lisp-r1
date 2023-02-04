require 'fileutils'

def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

debug = env_truthy?(:debug) ? true : false
cc = ENV['CC'] || "clang"
cflags = ["-g"]
if debug
  cflags << "-D DEBUG"
end
cflags = cflags.join(' ')

task :default => :build

desc "Clean all build artifacts"
task :clean do
  puts "clean. cleaning..."
  if Dir.exist?("./build")
    FileUtils.rm_rf("./build")
    FileUtils.mkdir_p("./build")
  end
end

desc "Build everything"
task :build do
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

desc "Build Atom object"
file "build/Atom.o" =>  ["Atom.c", "Atom.h"] do
  sh "#{cc} #{cflags} -c -o build/Atom.o Atom.c"
end

desc "Build Tokenizer object"
file "build/Tokenizer.o" => ["Tokenizer.c", "Tokenizer.h"] do
  sh "#{cc} #{cflags} -c -o build/Tokenizer.o Tokenizer.c"
end

desc "Build nassert_test program"
file "build/nassert_test" => ["nassert.h", "nassert_test.c"] do
  sh "#{cc} #{cflags} -o build/nassert_test nassert_test.c"
end

desc "Build Tokenizer_test program"
file "build/Tokenizer_test" => ["Tokenizer_test.c", "build/Tokenizer.o"] do
  sh "#{cc} #{cflags} -o build/Tokenizer_test Tokenizer_test.c build/Tokenizer.o"
end

desc "Build Atom_test program"
file "build/Atom_test" => ["Atom_test.c", "build/Atom.o"] do
  sh "#{cc} #{cflags} -o build/Atom_test Atom_test.c build/Atom.o"
end

desc "Run all tests"
task :test => :build do
  puts "TODO: Run all tests"
end
