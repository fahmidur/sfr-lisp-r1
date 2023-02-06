require 'set'
def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

$build_targets = Set.new
def build(name)
  target = "build/#{name}"
  $build_targets << target
  return target
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
  sh "mkdir -p ./build"
  sh "rm -rf ./build/*"
  sh "rm -f *.o"
end

desc "Build everything"
task :build do
  $build_targets.each do |target|
    Rake::Task[target].invoke
  end
end

desc "Build Util object"
file build("Util.o") => ["Util.h", "Util.c"] do
  sh "#{cc} #{cflags} -c -o build/Util.o Util.c"
end

desc "Build Symbol object"
file build("Symbol.o") =>  ["Symbol.c", "Symbol.h"] do
  sh "#{cc} #{cflags} -c -o build/Symbol.o Symbol.c"
end

desc "Build Tokenizer object"
file build("Tokenizer.o") => ["Tokenizer.c", "Tokenizer.h"] do
  sh "#{cc} #{cflags} -c -o build/Tokenizer.o Tokenizer.c"
end

desc "Build nassert_test program"
file build("nassert_test") => ["nassert.h", "nassert_test.c"] do
  sh "#{cc} #{cflags} -o build/nassert_test nassert_test.c"
end

desc "Build Tokenizer_test program"
file build("Tokenizer_test") => ["Tokenizer_test.c", "build/Tokenizer.o"] do
  sh "#{cc} #{cflags} -o build/Tokenizer_test Tokenizer_test.c build/Tokenizer.o"
end

desc "Build Symbol_test program"
file build("Symbol_test") => ["Symbol_test.c", "build/Symbol.o"] do
  sh "#{cc} #{cflags} -o build/Symbol_test Symbol_test.c build/Symbol.o"
end

desc "Run all tests"
task :test => :build do
  puts "TODO: Run all tests"
end
